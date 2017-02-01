/*
 *  ExAPExportTIFF.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Thu Jul 24 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#ifdef _MSC_VER			// TAW 2002/03/11 : ie. if MSVC
#ifndef MACINTOSH		// TAW 2002/02/23 : ie. if MSVC

// Suppress warning C4786: Identifier truncated to 255 chars in the debug info.
#pragma warning(disable : 4786)

#endif
#endif


#include <stack>

#include "General\\ExAPMinMax.h"
#include "CodecLib\\ExCodecLZW.h"
#include "UtilLib\\ExUtilBitByteSourceSinkInterface.h"
#include "UtilLib\\ExUtilFileWriterBaseInterface.h"
#include "ExAPBitmapDocument.h"
#include "ExAPBitmap8bpch.h"
#include "ExAPTIFFTags.h"


// **** Class ExAPExportTIFFByteSource ****


class ExAPExportTIFFByteSource : public ExUtilByteSourceInterface
{
private:
	const unsigned char * m_dataPtr;
	int m_numBytesRemaining;
	bool m_endOfFile;

public:
	ExAPExportTIFFByteSource( const unsigned char * dataPtr, int numBytesRemaining );
	virtual unsigned char ReadByte( void );
	virtual bool EndOfFile( void );
	virtual void AdvanceToNextLine( void );
}; // class ExAPExportTIFFByteSource


ExAPExportTIFFByteSource::ExAPExportTIFFByteSource( const unsigned char * dataPtr, int numBytesRemaining )
	: m_dataPtr( dataPtr ),
		m_numBytesRemaining( numBytesRemaining ),
		m_endOfFile( false )
{
} // ExAPExportTIFFByteSource::ExAPExportTIFFByteSource()


unsigned char ExAPExportTIFFByteSource::ReadByte( void )
{

	if( m_endOfFile )
	{
		return( 0 );
	}

	if( m_numBytesRemaining <= 0 )
	{
		m_endOfFile = true;
		return( 0 );
	}

	unsigned char rtn = *m_dataPtr++;

	m_numBytesRemaining--;

	return( rtn );
} // ExAPExportTIFFByteSource::ReadByte()


bool ExAPExportTIFFByteSource::EndOfFile( void )
{
	return( m_endOfFile );
} // ExAPExportTIFFByteSource::EndOfFile()


void ExAPExportTIFFByteSource::AdvanceToNextLine( void )
{
}


// **** Class ExAPExportTIFFBitSink ****


class ExAPExportTIFFBitSink : public ExUtilBitSinkInterface
{
private:
	//lint -e1725 : Class member is a reference.
	ExAPAutoPtr<ExUtilFileWriterBaseInterface> m_file;
	//lint +e1725
	unsigned int m_buffer;
	int m_numBitsInBuffer;
	unsigned int m_numBytesWritten;

public:
	ExAPExportTIFFBitSink( ExAPAutoPtr<ExUtilFileWriterBaseInterface> file );
	virtual void WriteBits( int bits, int numBits );
	virtual void Flush( void );
	unsigned int GetNumBytesWritten( void ) const;
}; // class ExAPExportTIFFBitSink


ExAPExportTIFFBitSink::ExAPExportTIFFBitSink( ExAPAutoPtr<ExUtilFileWriterBaseInterface> file )
	: m_file( file ),
		m_buffer( 0 ),
		m_numBitsInBuffer( 0 ),
		m_numBytesWritten( 0 )
{
} // ExAPExportTIFFBitSink::ExAPExportTIFFBitSink()


void ExAPExportTIFFBitSink::WriteBits( int bits, int numBits )
{
#ifdef TAW_TRACE
	printf( "Bit sink: Writing bits: %d; numBits: %d\n", bits, numBits );
#endif

	m_buffer <<= numBits;
	m_buffer |= (unsigned int)bits;	// We could mask 'bits' her to ensure not too many bits are present.
	m_numBitsInBuffer += numBits;

	while( m_numBitsInBuffer >= 8 )
	{
		const int ch = int( m_buffer >> ( m_numBitsInBuffer - 8 ) );

		m_file->WriteByte( ch );
		m_numBytesWritten++;
		m_numBitsInBuffer -= 8;
	}
} // ExAPExportTIFFBitSink::WriteBits()


void ExAPExportTIFFBitSink::Flush( void )
{

	if( m_numBitsInBuffer > 0 )
	{
		m_file->WriteByte( int( m_buffer << ( 8 - m_numBitsInBuffer ) ) );
		m_numBytesWritten++;
		m_numBitsInBuffer = 0;
	}
} // ExAPExportTIFFBitSink::Flush()


unsigned int ExAPExportTIFFBitSink::GetNumBytesWritten( void ) const
{
	return( m_numBytesWritten );
} // ExAPExportTIFFBitSink::GetNumBytesWritten()


// **** Class ExAPTIFFExporter ****


class ExAPTIFFExporter
{
private:
	typedef std::pair<unsigned int, unsigned int> IFDOffsetsPairType;

	struct TIFFImageInfo
	{
		// Members of a struct are public by default.
		// The default constructor should suffice here.
		std::vector<IFDFieldType> m_ifdFields;
		std::vector<unsigned int> m_stripOffsets;
		std::vector<unsigned int> m_stripByteCounts;
		int m_desiredStripSize;
		int m_imageBytesPerRow;
		int m_maxStripHeight;
	}; // struct TIFFImageInfo

	ExAPAutoPtr<ExUtilFileWriterBaseInterface> m_file;
	const bool m_useLZWCompression;
	const bool m_littleEndian;
	std::stack<IFDOffsetsPairType> m_ifdOffsets;

	void WriteHeader( void );
	void ExportImageDataLZWCompressed( TIFFImageInfo & imageInfo, const ExAPBitmap8bpch & image );
	void ExportImageDataUncompressed( TIFFImageInfo & imageInfo, const ExAPBitmap8bpch & image );
	void GenerateImageTags( TIFFImageInfo & imageInfo, const ExAPBitmap8bpch & image );
	void WriteImageTags( TIFFImageInfo & imageInfo );
	void ExportImage( ExAPAutoPtr<ExAPBitmap> imageBase );
	void WriteIFDOffsets( void );

public:

	// Constructor.
	ExAPTIFFExporter(
		const std::string & dstFilename,
		bool useLZWCompression,
		bool littleEndian );

	void ExportDocument( const ExAPBitmapDocument & doc );
}; // class ExAPTIFFExporter


ExAPTIFFExporter::ExAPTIFFExporter( const std::string & dstFilename, bool useLZWCompression, bool littleEndian )
	: m_file( ExUtilFileWriterBaseInterface::StdioCreate( dstFilename, littleEndian ) ),
		m_useLZWCompression( useLZWCompression ),
		m_littleEndian( littleEndian )
{
} // ExAPTIFFExporter::ExAPTIFFExporter()


void ExAPTIFFExporter::WriteHeader( void )
{
	const int endianByte = m_littleEndian ? 'I' : 'M';

	m_file->WriteByte( endianByte );
	m_file->WriteByte( endianByte );
	m_file->WriteWord( 42 );					// The magic 42 in every TIFF header.
	m_file->WriteDoubleWord( 0 );			// Placeholder of the offset of the first IFD.

	m_ifdOffsets.push( IFDOffsetsPairType( 4, 0 ) );
} // ExAPTIFFExporter::WriteHeader()


void ExAPTIFFExporter::ExportImageDataLZWCompressed( TIFFImageInfo & imageInfo, const ExAPBitmap8bpch & image )
{
	ExAPAutoArray<unsigned char> differenceBuf( imageInfo.m_imageBytesPerRow * imageInfo.m_maxStripHeight );
	int rowNum = 0;
	unsigned char * rowData = image.getBuffer();

	// Write out the uncompressed image data in strips.

	while( rowNum < image.GetHeight() )
	{
		const int numRowsInThisStrip = min( image.GetHeight() - rowNum, imageInfo.m_maxStripHeight );

		m_file->Align( 2 );
		imageInfo.m_stripOffsets.push_back( m_file->GetOffset() );
		//imageInfo.m_stripByteCounts.push_back( (unsigned int)( numRowsInThisStrip * imageInfo.m_imageBytesPerRow ) );
		
		for( int y = 0; y < numRowsInThisStrip; ++y )
		{
			unsigned char * differenceBufRow = differenceBuf + y * imageInfo.m_imageBytesPerRow;

			memcpy( differenceBufRow, rowData, size_t( imageInfo.m_imageBytesPerRow ) );
			rowData += image.getBytesPerLine();

			// Horizontal differencing.
			int bytesToDifference = ( image.GetWidth() - 1 ) * image.GetBytesPerPixel();
			unsigned char * srcPtr = differenceBufRow + bytesToDifference - 1;
			unsigned char * dstPtr = srcPtr + image.GetBytesPerPixel();

			while( bytesToDifference-- > 0 )
			{
				*dstPtr-- -= *srcPtr--;
			}
		}

		// LZW-comp,ress the differences.
		ExAPAutoPtr<ExUtilByteSourceInterface> byteSource = new ExAPExportTIFFByteSource( differenceBuf, numRowsInThisStrip * imageInfo.m_imageBytesPerRow );
		ExAPExportTIFFBitSink * pBitSinkRaw = new ExAPExportTIFFBitSink( m_file );
		ExAPAutoPtr<ExUtilBitSinkInterface> bitSink = pBitSinkRaw;
		ExAPAutoPtr<ExAPLZWCodec> codec = ExAPLZWCodec::CreateCodec( ExAPLZWCodec::TypeTIFF, 8 );	// Assume 8 bits per pixel for now.

		codec->CompressData( byteSource, bitSink );

		imageInfo.m_stripByteCounts.push_back( pBitSinkRaw->GetNumBytesWritten() );
		rowNum += numRowsInThisStrip;
	}
} // ExAPTIFFExporter::ExportImageDataLZWCompressed()


void ExAPTIFFExporter::ExportImageDataUncompressed( TIFFImageInfo & imageInfo, const ExAPBitmap8bpch & image )
{
	int rowNum = 0;
	unsigned char * rowData = image.getBuffer();

	// Write out the uncompressed image data in strips.

	while( rowNum < image.GetHeight() )
	{
		const int numRowsInThisStrip = min( image.GetHeight() - rowNum, imageInfo.m_maxStripHeight );

		m_file->Align( 2 );
		imageInfo.m_stripOffsets.push_back( m_file->GetOffset() );
		imageInfo.m_stripByteCounts.push_back( (unsigned int)( numRowsInThisStrip * imageInfo.m_imageBytesPerRow ) );
		
		for( int y = 0; y < numRowsInThisStrip; ++y )
		{
			m_file->WriteBytes( rowData, size_t( imageInfo.m_imageBytesPerRow ) );
			rowData += image.getBytesPerLine();
		}

		rowNum += numRowsInThisStrip;
	}
} // ExAPTIFFExporter::ExportImageDataUncompressed()


void ExAPTIFFExporter::GenerateImageTags( TIFFImageInfo & imageInfo, const ExAPBitmap8bpch & image )
{
	// Write out the strip offsets.
	m_file->Align( 2 );

	const unsigned int stripOffsetsOffset = m_file->GetOffset();
	const unsigned int numStrips = imageInfo.m_stripOffsets.size();
	unsigned int i;

	for( i = 0; i < numStrips; ++i )
	{
		m_file->WriteDoubleWord( imageInfo.m_stripOffsets[i] );
	}

	// Write out the strip byte counts.
	const unsigned int stripByteCountsOffset = m_file->GetOffset();

	for( i = 0; i < numStrips; ++i )
	{
		m_file->WriteDoubleWord( imageInfo.m_stripByteCounts[i] );
	}

	// Write out the bits per sample.
	static const unsigned short bitsPerSample = 8;
	const unsigned int samplesPerPixel = (unsigned int)( image.GetBytesPerPixel() );
	const unsigned int bitsPerSampleOffset = m_file->GetOffset();

	for( i = 0; i < samplesPerPixel; ++i )
	{
		m_file->WriteWord( bitsPerSample );
	}

	// Write out the X resolution.
	const unsigned int xResolutionOffset = m_file->GetOffset();

	m_file->WriteDoubleWord( 72 );
	m_file->WriteDoubleWord( 1 );

	// Write out the Y resolution.

	const unsigned int yResolutionOffset = m_file->GetOffset();

	m_file->WriteDoubleWord( 72 );
	m_file->WriteDoubleWord( 1 );

	// Construct the IFD in memory.
	const unsigned int compressionType = m_useLZWCompression ? 5 : 1;
	unsigned int photometricInterpretation = 0;

	switch( image.GetColourModelType() )
	{
		case ExAPColourModel::RGB24:
			photometricInterpretation = 2;
			break;

		default:
			ThrowExAPException( ExAPException::UnsupportedColourModel );
	}

	// New Subfile Type (254).
	imageInfo.m_ifdFields.push_back( IFDFieldType( eTag_NewSubfileType, eTagDataType_Long, 1, 0 ) );
	// Image Width (256).
	imageInfo.m_ifdFields.push_back( IFDFieldType( eTag_ImageWidth, eTagDataType_Long, 1, (unsigned int)image.GetWidth() ) );
	// Image Length (257).
	imageInfo.m_ifdFields.push_back( IFDFieldType( eTag_ImageLength, eTagDataType_Long, 1, (unsigned int)image.GetHeight() ) );
	// Bits Per Sample (258).
	imageInfo.m_ifdFields.push_back( IFDFieldType( eTag_BitsPerSample, eTagDataType_Short, samplesPerPixel, bitsPerSampleOffset ) );
	// Compression (259).
	imageInfo.m_ifdFields.push_back( IFDFieldType( eTag_Compression, eTagDataType_Short, 1, compressionType ) );
	// Photometric Interpretation (262).
	imageInfo.m_ifdFields.push_back( IFDFieldType( eTag_PhotometricInterpretation, eTagDataType_Short, 1, photometricInterpretation ) );
	// Strip Offsets (273).
	imageInfo.m_ifdFields.push_back( IFDFieldType( eTag_StripOffsets, eTagDataType_Long, numStrips, stripOffsetsOffset ) );
	// Samples Per Pixel (277).
	imageInfo.m_ifdFields.push_back( IFDFieldType( eTag_SamplesPerPixel, eTagDataType_Short, 1, samplesPerPixel ) );
	// Rows Per Strip (278).
	imageInfo.m_ifdFields.push_back( IFDFieldType( eTag_RowsPerStrip, eTagDataType_Long, 1, (unsigned int)imageInfo.m_maxStripHeight ) );
	// Strip Byte Counts (279).
	imageInfo.m_ifdFields.push_back( IFDFieldType( eTag_StripByteCounts, eTagDataType_Long, numStrips, stripByteCountsOffset ) );
	// X Resolution (282).
	imageInfo.m_ifdFields.push_back( IFDFieldType( eTag_XResolution, eTagDataType_Rational, 1, xResolutionOffset ) );
	// Y Resolution (283).
	imageInfo.m_ifdFields.push_back( IFDFieldType( eTag_YResolution, eTagDataType_Rational, 1, yResolutionOffset ) );
	// Resolution Unit (296).
	imageInfo.m_ifdFields.push_back( IFDFieldType( eTag_ResolutionUnit, eTagDataType_Short, 1, 2 ) );	// Default == 2 (inch).

	if( m_useLZWCompression )
	{
		// Differencing Predictor (317).
		imageInfo.m_ifdFields.push_back( IFDFieldType( eTag_DifferencingPredictor, eTagDataType_Short, 1, 2 ) );	// 2 == Horizontal differencing (for LZW compression only).
	}
} // ExAPTIFFExporter::GenerateImageTags()


void ExAPTIFFExporter::WriteImageTags( TIFFImageInfo & imageInfo )
{
	// Change the previous IFD link to point to the start of this IFD.
	IFDOffsetsPairType & x = m_ifdOffsets.top();

	//m_ifdOffsets.pop();
	x.second = m_file->GetOffset();

	// Write the IFD.
	//const unsigned int ifdOffset = m_file->GetOffset();
	const unsigned int numIFDFields = imageInfo.m_ifdFields.size();

	m_file->WriteWord( (unsigned short)numIFDFields );

	for( unsigned int i = 0; i < numIFDFields; ++i )
	{
		const IFDFieldType & ifdField = imageInfo.m_ifdFields[i];

		m_file->WriteWord( ifdField.m_tag );
		m_file->WriteWord( ifdField.m_fieldType );
		m_file->WriteDoubleWord( ifdField.m_dataCount );
		m_file->WriteDoubleWord( ifdField.m_valueOffset );
	}

	// Place this location on the IFD offsets stack so we can find it later.
	m_ifdOffsets.push( IFDOffsetsPairType( m_file->GetOffset(), 0 ) );

	// Terminate with a zero IFD offset.
	m_file->WriteDoubleWord( 0 );
} // ExAPTIFFExporter::WriteImageTags()


void ExAPTIFFExporter::ExportImage( ExAPAutoPtr<ExAPBitmap> imageBase )
{
	TIFFImageInfo imageInfo;

	imageInfo.m_desiredStripSize = 8192;		// Bytes.
	// imageBytesPerRow is the number of utilized bytes in an image row;
	// it may be less than srcImage->GetBytesPerLine().
	//imageInfo.m_imageBytesPerRow = imageBase->GetBytesPerPixel() * imageBase->GetWidth();
	imageInfo.m_imageBytesPerRow = imageBase->getUnalignedBytesPerLine();
	imageInfo.m_maxStripHeight = max( imageInfo.m_desiredStripSize / imageInfo.m_imageBytesPerRow, 1 );

	ExAPBitmap * imageBaseRaw = imageBase;
	ExAPBitmap8bpch * imageRaw = dynamic_cast<ExAPBitmap8bpch *>( imageBaseRaw );

	if( imageRaw == 0 )
	{
		// We currently only support 8 bit per channel images.
		ThrowExAPException( ExAPException::DynamicCastFailed );
	}

	if( m_useLZWCompression )
	{
		// This will do horizontal differencing first.
		ExportImageDataLZWCompressed( imageInfo, *imageRaw );
	}
	else
	{
		ExportImageDataUncompressed( imageInfo, *imageRaw );
	}

	GenerateImageTags( imageInfo, *imageRaw );
	WriteImageTags( imageInfo );
} // ExAPTIFFExporter::ExportImage()


void ExAPTIFFExporter::WriteIFDOffsets( void )
{
	
	while( !m_ifdOffsets.empty() )
	{
		const IFDOffsetsPairType x = m_ifdOffsets.top();

		m_ifdOffsets.pop();

		m_file->SetOffset( (long)x.first );
		m_file->WriteDoubleWord( x.second );
	}
} // ExAPTIFFExporter::WriteIFDOffsets()


void ExAPTIFFExporter::ExportDocument( const ExAPBitmapDocument & doc )
{
	WriteHeader();

	const ExAPBitmapDocument::BitmapVectorType::size_type numImagesInDocument = doc.m_v.size();
	ExAPBitmapDocument::BitmapVectorType::size_type i;

	for( i = 0; i < numImagesInDocument; ++i )
	{
		ExportImage( doc.m_v[i] );
	}

	WriteIFDOffsets();
} // ExAPTIFFExporter::ExportDocument()


// **** Class ExAPBitmapDocument ****


void ExAPBitmapDocument::ExportToTIFF(
	const std::string & dstFilename,
	bool useLZWCompression,
	bool littleEndian ) const
{
#if 1
	ExAPTIFFExporter exporter( dstFilename, useLZWCompression, littleEndian );

	exporter.ExportDocument( *this );
#else
	// 0) Open the file.
	ExAPAutoPtr<ExAPFileWriterBase> file = ExAPFileWriterBase::Create( dstFilename, littleEndian );

	// 1) Write the TIFF header.
	const int endianByte = littleEndian ? 'I' : 'M';

	file->WriteByte( endianByte );
	file->WriteByte( endianByte );
	file->WriteWord( 42 );					// The magic 42 in every TIFF header.
	file->WriteDoubleWord( 0 );			// Placeholder of the offset of the first IFD.

	// 2) Write the image data, in strips.
	ExAPAutoPtr<ExAPBitmap> srcImageBase = GetComposite();
	ExAPBitmap * srcImageBaseRaw = srcImageBase;
	ExAPBitmap8bpch * srcImageRaw = dynamic_cast<ExAPBitmap8bpch *>( srcImageBaseRaw );

	if( srcImageRaw == 0 )
	{
		// We currently only support 8 bit per channel images.
		ThrowExAPException( ExAPException::DynamicCastFailed );
	}

	ExAPAutoPtr<ExAPBitmap8bpch> srcImage = srcImageRaw;

	srcImage.SetResourceOwnership( false );

	static const int desiredStripSize = 8192;		// Bytes.
	// imageBytesPerRow is the number of utilized bytes in an image row;
	// it may be less than srcImage->GetBytesPerLine().
	const int imageBytesPerRow = srcImage->GetBytesPerPixel() * srcImage->GetWidth();
	const int maxStripHeight = max( desiredStripSize / imageBytesPerRow, 1 );
	unsigned int numStrips = 0;
	int rowNum = 0;
	unsigned char * rowData = srcImage->getBuffer();
	std::vector<unsigned int> stripOffsets;
	std::vector<unsigned int> stripByteCounts;

	// Write out the uncompressed image data in strips.

	while( rowNum < srcImage->GetHeight() )
	{
		const int numRowsInThisStrip = min( srcImage->GetHeight() - rowNum, maxStripHeight );

		file->Align( 2 );
		stripOffsets.push_back( file->GetOffset() );
		stripByteCounts.push_back( (unsigned int)( numRowsInThisStrip * imageBytesPerRow ) );
		
		for( int y = 0; y < numRowsInThisStrip; ++y )
		{
			file->WriteBytes( rowData, size_t( imageBytesPerRow ) );
			rowData += srcImage->getBytesPerLine();
		}

		rowNum += numRowsInThisStrip;
		numStrips++;
	}

	// Write out the strip offsets.
	file->Align( 2 );

	const unsigned int stripOffsetsOffset = file->GetOffset();
	unsigned int i;

	for( i = 0; i < numStrips; ++i )
	{
		file->WriteDoubleWord( stripOffsets[i] );
	}

	// Write out the strip byte counts.
	const unsigned int stripByteCountsOffset = file->GetOffset();

	for( i = 0; i < numStrips; ++i )
	{
		file->WriteDoubleWord( stripByteCounts[i] );
	}

	// Write out the bits per sample.
	static const unsigned short bitsPerSample = 8;
	const unsigned int samplesPerPixel = (unsigned int)( srcImage->GetBytesPerPixel() );
	const unsigned int bitsPerSampleOffset = file->GetOffset();

	for( i = 0; i < samplesPerPixel; ++i )
	{
		file->WriteWord( bitsPerSample );
	}

	// Write out the X resolution.
	const unsigned int xResolutionOffset = file->GetOffset();

	file->WriteDoubleWord( 72 );
	file->WriteDoubleWord( 1 );

	// Write out the Y resolution.

	const unsigned int yResolutionOffset = file->GetOffset();

	file->WriteDoubleWord( 72 );
	file->WriteDoubleWord( 1 );

	// Construct the IFD in memory.
	static const unsigned int compressionType = 1;	// No compression.
	unsigned int photometricInterpretation = 0;

	switch( srcImage->GetColourModelType() )
	{
		case ExAPColourModel::RGB24:
			photometricInterpretation = 2;
			break;

		default:
			ThrowExAPException( ExAPException::UnsupportedColourModel );
	}

	std::vector<IFDFieldType> ifdFields;

	// New Subfile Type (254).
	ifdFields.push_back( IFDFieldType( eTag_NewSubfileType, eTagDataType_Long, 1, 0 ) );
	// Image Width (256).
	ifdFields.push_back( IFDFieldType( eTag_ImageWidth, eTagDataType_Long, 1, (unsigned int)srcImage->GetWidth() ) );
	// Image Length (257).
	ifdFields.push_back( IFDFieldType( eTag_ImageLength, eTagDataType_Long, 1, (unsigned int)srcImage->GetHeight() ) );
	// Bits Per Sample (258).
	ifdFields.push_back( IFDFieldType( eTag_BitsPerSample, eTagDataType_Short, samplesPerPixel, bitsPerSampleOffset ) );
	// Compression (259).
	ifdFields.push_back( IFDFieldType( eTag_Compression, eTagDataType_Short, 1, compressionType ) );
	// Photometric Interpretation (262).
	ifdFields.push_back( IFDFieldType( eTag_PhotometricInterpretation, eTagDataType_Short, 1, photometricInterpretation ) );
	// Strip Offsets (273).
	ifdFields.push_back( IFDFieldType( eTag_StripOffsets, eTagDataType_Long, numStrips, stripOffsetsOffset ) );
	// Samples Per Pixel (277).
	ifdFields.push_back( IFDFieldType( eTag_SamplesPerPixel, eTagDataType_Short, 1, samplesPerPixel ) );
	// Rows Per Strip (278).
	ifdFields.push_back( IFDFieldType( eTag_RowsPerStrip, eTagDataType_Long, 1, (unsigned int)maxStripHeight ) );
	// Strip Byte Counts (279).
	ifdFields.push_back( IFDFieldType( eTag_StripByteCounts, eTagDataType_Long, numStrips, stripByteCountsOffset ) );
	// X Resolution (282).
	ifdFields.push_back( IFDFieldType( eTag_XResolution, eTagDataType_Rational, 1, xResolutionOffset ) );
	// Y Resolution (283).
	ifdFields.push_back( IFDFieldType( eTag_YResolution, eTagDataType_Rational, 1, yResolutionOffset ) );
	// Resolution Unit (296).
	ifdFields.push_back( IFDFieldType( eTag_ResolutionUnit, eTagDataType_Short, 1, 2 ) );	// Default == 2 (inch).

	// Write the IFD.
	const unsigned int ifdOffset = file->GetOffset();
	const unsigned int numIFDFields = ifdFields.size();

	file->WriteWord( (unsigned short)numIFDFields );

	for( i = 0; i < numIFDFields; ++i )
	{
		const IFDFieldType & ifdField = ifdFields[i];

		file->WriteWord( ifdField.m_tag );
		file->WriteWord( ifdField.m_fieldType );
		file->WriteDoubleWord( ifdField.m_dataCount );
		file->WriteDoubleWord( ifdField.m_valueOffset );
	}

	// Terminate with a zero IFD offset.
	file->WriteDoubleWord( 0 );

	// Go back and write the offset of the IFD starting at byte 4.
	file->SetOffset( 4 );
	file->WriteDoubleWord( ifdOffset );

	// All done!
#endif
} // ExAPBitmapDocument::ExportToTIFF()


// **** End of File ****
