/*
 *  ExAPImportTIFF.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Mon Jun 30 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#ifdef _MSC_VER			// TAW 2002/03/11 : ie. if MSVC
#ifndef MACINTOSH		// TAW 2002/02/23 : ie. if MSVC

// Suppress warning C4786: Identifier truncated to 255 chars in the debug info.
#pragma warning(disable : 4786)

#endif
#endif


//lint -e1050 : Expected a template argument list '<...>' for template 'std::iterator<<1>,<2>,<3>>'
#include <cstring>

#include "General\\ExAPMinMax.h"

#include "CodecLib\\ExCodecLZW.h"

#include "UtilLib\\ExUtilBitByteSourceSinkInterface.h"

#include "ExAPImportTIFF.h"

#include "ExAPBitmapDocument.h"
#include "ExAPBitmapGrey8.h"
#include "ExAPBitmapRGB24.h"
#include "ExAPBitmapCMYK32.h"
//lint +e1050


//#define TAW_TRACE		1


// **** Class ExAPTIFFImageDataBitSource ****


class ExAPTIFFImageDataBitSource : public ExUtilBitSourceInterface
{
private:
	// The default constructor is never defined; it is declared here in order to satisfy lint.
	//lint -esym( 1526, ExAPTIFFImageDataBitSource::ExAPTIFFImageDataBitSource )
	//lint -e1704 : Constructor has private access specification.
	ExAPTIFFImageDataBitSource( void );
	//lint +e1704

public:
	ExAPAutoPtr<ExUtilFileReaderBaseInterface> m_file;

	unsigned int m_buffer;
	int m_numBitsInBuffer;
	int m_numBytesLeftToRead;

	ExAPTIFFImageDataBitSource( ExAPAutoPtr<ExUtilFileReaderBaseInterface> file, int numBytesLeftToRead );
	virtual ~ExAPTIFFImageDataBitSource( void );
	virtual int ReadBits( int numBits );	// Max of 31 bits.
	virtual int ReadBits( int numBits, int & bits );	// Max of 31 bits.
	virtual bool EndOfFile( void );
}; // class ExAPTIFFImageDataBitSource


ExAPTIFFImageDataBitSource::ExAPTIFFImageDataBitSource( ExAPAutoPtr<ExUtilFileReaderBaseInterface> file, int numBytesLeftToRead )
	: m_file( file ),
		m_buffer( 0 ),
		m_numBitsInBuffer( 0 ),
		m_numBytesLeftToRead( numBytesLeftToRead )
{
} // ExAPTIFFImageDataBitSource::ExAPTIFFImageDataBitSource()


ExAPTIFFImageDataBitSource::~ExAPTIFFImageDataBitSource( void )
{
} // ExAPTIFFImageDataBitSource::~ExAPTIFFImageDataBitSource()


int ExAPTIFFImageDataBitSource::ReadBits( int numBits )
{
	// Accumulate bytes in m_buffer until the requested number of bits are available.

	while( m_numBitsInBuffer < numBits )
	{

		if( m_numBytesLeftToRead <= 0 )
		{
#ifdef TAW_TRACE
			printf( "ExAPTIFFImageDataBitSource::ReadBits: No more bytes.\n" );
#endif
			return( -1 );
		}

		const int x = m_file->GetByteAsInt(); //fgetc( m_fp );

#if 0 //def TAW_TRACE
		printf( "ExAPTIFFImageDataBitSource::ReadBits: Read byte %d.\n", x );
#endif

		--m_numBytesLeftToRead;

		if( m_file->EndOfFile() /* x == EOF */ )
		{
#ifdef TAW_TRACE
			printf( "ExAPTIFFImageDataBitSource::ReadBits: End of File.\n" );
#endif
			return( -1 );
		}

		m_buffer <<= 8;
		m_buffer |= (unsigned int)( x & 0xFF );
		m_numBitsInBuffer += 8;		// 8 bits per byte.
	}

	m_numBitsInBuffer -= numBits;

	const int rtn = int( m_buffer >> m_numBitsInBuffer );

	m_buffer &= (unsigned int)( ( 1 << m_numBitsInBuffer ) - 1 );
	return( rtn );
} // ExAPTIFFImageDataBitSource::ReadBits()


int ExAPTIFFImageDataBitSource::ReadBits( int numBits, int & bits )	// Max of 31 bits.
{
	bits = ReadBits( numBits );
	return( numBits );
}


bool ExAPTIFFImageDataBitSource::EndOfFile( void )
{
	return( m_numBytesLeftToRead <= 0  ||  m_file->EndOfFile() /* feof( m_fp ) != 0 */ );
} // ExAPTIFFImageDataBitSource::EndOfFile()


// **** Class ExAPTIFFImageDataByteSink ****


class ExAPTIFFImageDataByteSink : public ExUtilByteSinkInterface
{
private:
	// The default constructor is never defined; it is declared here in order to satisfy lint.
	//lint -esym( 1526, ExAPTIFFImageDataByteSink::ExAPTIFFImageDataByteSink )
	//lint -e1704 : Constructor has private access specification.
	ExAPTIFFImageDataByteSink( void );
	//lint +e1704

public:
	//lint -e1725 : Class member is a reference.
	const ExAPBitmap & m_image;
	//lint +e1725

	unsigned char * m_rowPtr;
	unsigned char * m_bytePtr;
	int m_numRowsRemaining;
	int m_numColsRemainingInRow;

	ExAPTIFFImageDataByteSink( const ExAPBitmap & image, unsigned char * rowPtr, int numRowsRemaining );
	virtual ~ExAPTIFFImageDataByteSink( void );
	virtual void WriteByte( unsigned char ch );
	virtual void WriteBytes( const std::vector<unsigned char> & bytes );
	virtual void AdvanceToNextLine( void );
}; // class ExAPTIFFImageDataByteSink


ExAPTIFFImageDataByteSink::ExAPTIFFImageDataByteSink( const ExAPBitmap & image, unsigned char * rowPtr, int numRowsRemaining )
	: m_image( image ),
		m_rowPtr( rowPtr ),
		m_bytePtr( rowPtr ),
		m_numRowsRemaining( numRowsRemaining ),
		m_numColsRemainingInRow( 0 )
{
} // ExAPTIFFImageDataByteSink::ExAPTIFFImageDataByteSink()


ExAPTIFFImageDataByteSink::~ExAPTIFFImageDataByteSink( void )
{
	m_rowPtr = 0;		// To satisfy lint.
	m_bytePtr = 0;	// To satisfy lint.
} // ExAPTIFFImageDataByteSink::~ExAPTIFFImageDataByteSink()


void ExAPTIFFImageDataByteSink::WriteByte( unsigned char ch )
{
	ExAssert( ExAPException::IllegalFunctionCall )
} // ExAPTIFFImageDataByteSink::WriteByte()


void ExAPTIFFImageDataByteSink::WriteBytes( const std::vector<unsigned char> & bytes )
{
	std::vector<unsigned char>::const_iterator it = bytes.begin();
	const std::vector<unsigned char>::const_iterator itEnd = bytes.end();

	while( it != itEnd )
	{

		if( m_numColsRemainingInRow <= 0 )
		{

			if( m_numRowsRemaining-- <= 0 )
			{
				break;
			}

			//m_numColsRemainingInRow = m_image.GetWidth() * m_image.GetBytesPerPixel();
			m_numColsRemainingInRow = m_image.getUnalignedBytesPerLine();
			m_bytePtr = m_rowPtr;
			m_rowPtr += m_image.getBytesPerLine();
		}

		*m_bytePtr++ = *it++;
		--m_numColsRemainingInRow;
	}
} // ExAPTIFFImageDataByteSink::WriteBytes()


void ExAPTIFFImageDataByteSink::AdvanceToNextLine( void )
{
} // ExAPTIFFImageDataByteSink::AdvanceToNextLine()


// **** Class ExAPTIFFReader ****


ExAPTIFFReader::ExAPTIFFReader( ExAPAutoPtr<ExUtilFileReaderBaseInterface> file ) throw( ExAPException )
	: m_file( file ),
		m_imageFileDirectoryOffset( 0 )
{
	unsigned char header[8];
#if 1
	const size_t bytesRead = size_t( m_file->GetBytes( header, int( sizeof( header ) ) ) );
#else
	const size_t bytesRead = fread( header, 1, sizeof( header ), fp );
#endif

	if( bytesRead != sizeof( header ) )
	{
#ifdef TAW_TRACE
		printf( "Error: The header was not fully read.\n" );
#endif
		// Throw an exception.
	}

	const bool littleEndian = ( header[0] == 'I'  &&  header[1] == 'I' );

	// "file" is a non-const reference;
	// "m_file" is a const reference to the same object.
	file->SetEndianness( littleEndian );

	const unsigned short magic42 = m_file->GetWord( header + 2 );

	m_imageFileDirectoryOffset = m_file->GetDoubleWord( header + 4 );

#ifdef TAW_TRACE
	if( header[0] == 'I'  &&  header[1] == 'I' )
	{
		printf( "The file's data is in little-endian format.\n" );
	}
	else if( header[0] == 'M'  &&  header[1] == 'M' )
	{
		printf( "The file's data is in big-endian format.\n" );
	}
	else
	{
		printf( "Error: The file's byte ordering is not properly indicated.\n" );
		// Throw an exception.
	}

	if( magic42 == 42 )
	{
		printf( "The magic 42 was found.\n" );
	}
	else
	{
		printf( "Error: The magic 42 was not found.\n" );
	}
#endif

	ExAssert( magic42 == 42 )
	ExAssert( m_imageFileDirectoryOffset != 0 )
} // ExAPTIFFReader::ExAPTIFFReader()


bool ExAPTIFFReader::NextIFDExists( void ) const
{
	return( m_imageFileDirectoryOffset != 0 );
} // ExAPTIFFReader::NextIFDExists()


ExAPBitmap * ExAPTIFFReader::ImportImage( void ) throw( ExAPException )
{
	ExAPBitmap * rtn = 0;
	ExAPTIFFImageMetadata tiffImageMetadata;

#ifdef TAW_TRACE
	printf( "ImportImage: fseek()ing to offset %u...\n", m_imageFileDirectoryOffset );
#endif

	m_file->SetOffset( static_cast<long>( m_imageFileDirectoryOffset ) );

	// Read all of the tags in this IFD.
	const int numIFDFields = static_cast<int>( m_file->GetWord() );

#ifdef TAW_TRACE
	printf( "ImportImage: IFD has %d fields.\n", numIFDFields );
#endif

	m_IFDFields.clear();

	for( int i = 0; i < numIFDFields; ++i )
	{
		const unsigned short tag = m_file->GetWord();
		const unsigned short fieldType = m_file->GetWord();				// The type of data that constitute the field.
		const unsigned int dataCount = m_file->GetDoubleWord();		// The number of data in the field.
		const unsigned int valueOffset = m_file->GetDoubleWord();

#if 0 //def TAW_TRACE
		printf( "IFD field %d:\n", i );
		printf( "  Tag: %hu\n", tag );
		printf( "  Field data type: %hu\n", fieldType );
		printf( "  Data count: %u\n", dataCount );
		printf( "  Value offset in file: %u\n\n", valueOffset );
#endif

		m_IFDFields.push_back( IFDFieldType( tag, fieldType, dataCount, valueOffset ) );
	}

	// Prepare to point to the next IFD upon the next call.
	m_imageFileDirectoryOffset = m_file->GetDoubleWord();

	std::vector<IFDFieldType>::const_iterator it = m_IFDFields.begin();
	const std::vector<IFDFieldType>::const_iterator itEnd = m_IFDFields.end();

	while( it != itEnd )
	{
		const IFDFieldType & IFDField = *it;

		m_tagReaderMaster.ReadTag(
			m_file,
			IFDField,
			tiffImageMetadata );
		++it;
	}

	// Use the metadata to create the image.
	const bool imageIsUncompressed = tiffImageMetadata.m_compressionType == 1;
	const bool imageIsLZWCompressed = tiffImageMetadata.m_compressionType == 5;
	const bool imageIsDecompressable = imageIsUncompressed  ||  imageIsLZWCompressed;

	if( tiffImageMetadata.m_imageLength <= 0  ||
			tiffImageMetadata.m_imageWidth <= 0  ||
			tiffImageMetadata.m_rowsPerStrip <= 0  ||
			tiffImageMetadata.m_stripOffsets.size() == 0  ||	// Unsigned; can't be less than zero.
			tiffImageMetadata.m_stripOffsets.size() != tiffImageMetadata.m_stripByteCounts.size() )
	{
#ifdef TAW_TRACE
		printf( "ImportImage: Image metadata is invalid.\n" );
#endif
	}
	else if(
			tiffImageMetadata.m_bitsPerSample != 8  ||
			!imageIsDecompressable )
	{
#ifdef TAW_TRACE
		printf( "ImportImage: This filter is not yet capable of importing this image.\n" );
#endif
	}
	else
	{
		const int imageWidth = tiffImageMetadata.m_imageWidth;
		const int imageHeight = tiffImageMetadata.m_imageLength;
		const int samplesPerPixel = tiffImageMetadata.m_samplesPerPixel;
		ExAPBitmap8bpch * image = 0;

		// We already know that each sample contains 8 bits.

		switch( tiffImageMetadata.m_photometricInterpretation )
		{
			case 0:		// White is zero.
			case 1:		// Black is zero.
			case 4:		// Transparency mask

				if( samplesPerPixel == 1 )
				{
					image = new ExAPBitmapGrey8( imageWidth, imageHeight );
				}

				break;

			case 2:		// RGB

				if( samplesPerPixel == 3 )
				{
					image = new ExAPBitmapRGB24( imageWidth, imageHeight );
				}

				break;

			case 5:		// CMYK

				if( samplesPerPixel == 4 )
				{
					image = new ExAPBitmapCMYK32( imageWidth, imageHeight );
				}

				break;

			//case 3:		// RGB Paletted
			//case 6:		// YCbCr
			//case 8:		// CIELab
			default:
				break;
		}

		if( image == 0 )
		{
#ifdef TAW_TRACE
			printf( "ImportImage: Unsupported colour model or samples per pixel.\n" );
#endif
			return( 0 );
		}

#ifdef TAW_TRACE
		int numStrip = 0;
#endif
		std::vector<long>::const_iterator itStripOffset = tiffImageMetadata.m_stripOffsets.begin();
		const std::vector<long>::const_iterator itStripOffsetEnd = tiffImageMetadata.m_stripOffsets.end();
		std::vector<int>::const_iterator itStripByteCount = tiffImageMetadata.m_stripByteCounts.begin();
		const std::vector<int>::const_iterator itStripByteCountEnd = tiffImageMetadata.m_stripByteCounts.end();
		int firstRow = 0;
		unsigned char * imageRowPtr = image->getBuffer();
		const int imageBytesPerRow = image->getBytesPerLine();
		const int srcBytesPerRow = imageWidth * image->GetBytesPerPixel();

		while( itStripOffset != itStripOffsetEnd  &&
			itStripByteCount != itStripByteCountEnd )
		{
			int numRowsInThisStrip = min( tiffImageMetadata.m_imageLength - firstRow, tiffImageMetadata.m_rowsPerStrip );

			m_file->SetOffset( *itStripOffset );

			if( imageIsUncompressed )
			{

				while( numRowsInThisStrip-- > 0 )
				{
					/* const int bytesRead = */ (void)m_file->GetBytes( imageRowPtr, srcBytesPerRow );

					imageRowPtr += imageBytesPerRow;
				}
			}
			else if( imageIsLZWCompressed )
			{
				// **** Begin handling LZW-compressed strip ****
				ExAPAutoPtr<ExUtilBitSourceInterface> bitSource = new ExAPTIFFImageDataBitSource( m_file, *itStripByteCount );
				ExAPAutoPtr<ExUtilByteSinkInterface> byteSink = new ExAPTIFFImageDataByteSink( *image, imageRowPtr, numRowsInThisStrip );
				ExAPAutoPtr<ExAPLZWCodec> codec = ExAPLZWCodec::CreateCodec( ExAPLZWCodec::TypeTIFF, 8 );	// Assume 8 bits per pixel for now.
				//static const int bitsPerChunk = 8;	// Start at code 2 + ( 1 << 8 ) == 258.
				//static const int codeResizingOffset = 1;

#if 0 //def TAW_TRACE
				const int stripByteCount = *itStripByteCount;

				printf( "Decompressing LZW strip #%d...\n", numStrip++ );
				printf( "  Strip byte count: %d.\n", stripByteCount );
#endif

				codec->DecompressData( bitSource, byteSink /*, bitsPerChunk, codeResizingOffset */ );
				imageRowPtr += numRowsInThisStrip * imageBytesPerRow;
				// **** End handling LZW-compressed strip ****
			}

			firstRow += tiffImageMetadata.m_rowsPerStrip;
			++itStripOffset;
			++itStripByteCount;
		}

		if( imageIsLZWCompressed  &&  tiffImageMetadata.m_differencingPredictor == 2 )
		{
			// Undo horizontal differencing.
#ifdef TAW_TRACE
			printf( "Undoing horizontal differencing...\n" );
#endif
			const int bytesPerPixel = image->GetBytesPerPixel();
			imageRowPtr = image->getBuffer();

			int numRowsLeft = image->GetHeight();
			
			while( numRowsLeft-- > 0 )
			{
				unsigned char * srcPtr = imageRowPtr;
				unsigned char * dstPtr = srcPtr + bytesPerPixel;
				int numBytesLeftInRow = ( image->GetWidth() - 1 ) * bytesPerPixel;

				imageRowPtr += imageBytesPerRow;

				while( numBytesLeftInRow-- > 0 )
				{
					*dstPtr++ += *srcPtr++;
				}
			}
		}

		rtn = image;
	}

	return( rtn );
} // ExAPTIFFReader::ImportImage()


// **** End of File ****
