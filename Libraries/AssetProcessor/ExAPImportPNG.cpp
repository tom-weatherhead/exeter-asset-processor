/*
 *  ExAPImportPNG.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Wed Jul 30 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

//#define TAW_TRACE		1

#ifdef TAW_TRACE
#include <cstdio>
#endif

#include "CodecLib\\ExCodecBitByteSourceSink.h"
#include "UtilLib\\ExUtilBitByteSourceSinkInterface.h"
#include "UtilLib\\ExUtilFileReaderBaseInterface.h"

#include "ExAPBitmapRGB24.h"

#include "..\\3rdParty\\zlib\\zlib.h"

extern "C"
{
int ZlibInflate(
	unsigned char * compr, unsigned int comprLen,
	unsigned char * uncompr, unsigned int uncomprLen );
}


// **** Class PNGChunkLabel ****


class PNGChunkLabel
{
public:
	bool m_valid;
	unsigned int m_chunkDataLength;
	char m_chunkLabel[5];

	PNGChunkLabel( void );

	inline bool IsValid( void ) const
	{
		return( m_valid );
	}

	inline unsigned int GetChunkDataLength( void ) const
	{
		return( m_chunkDataLength );
	}

	inline char * GetChunkLabel( void ) const
	{
		return( (char *)m_chunkLabel );
	}

	inline void Set( unsigned int chunkDataLength, char * chunkLabel )
	{
		m_valid = true;
		m_chunkDataLength = chunkDataLength;
		strcpy( m_chunkLabel, chunkLabel );
	}

	inline void Reset( void )
	{
		m_valid = false;
		m_chunkDataLength = 0;
		m_chunkLabel[0] = '\0';
	}
}; // class PNGChunkLabel


PNGChunkLabel::PNGChunkLabel( void )
	: m_valid( false ),
		m_chunkDataLength( 0 )
{
	m_chunkLabel[0] = '\0';
} // PNGChunkLabel::PNGChunkLabel()


// **** Class PNGImporterByteSource ****


class PNGImporterByteSource : public ExUtilByteSourceInterface
{
private:
	ExAPAutoPtr<ExUtilFileReaderBaseInterface> m_file;
	z_stream m_decompressionStream;			// This is a C structure.
	int m_decompressedDataBufferSize;
	ExAPAutoPtr< ExAPAutoArray<unsigned char> > m_decompressedDataBuffer;
	unsigned char * m_readHead;
	bool m_EOF;
	bool m_EndOfDecompressionStreamReached;
	bool m_DecompressionStreamInitialized;
	PNGChunkLabel * m_chunkLabel;
	ExAPAutoPtr< ExAPAutoArray<unsigned char> > m_compressedDataBuffer;

	bool NextDataByteIsReady( void ) const;
	bool ReadNextIDATChunkFromFile( void );
	bool FindNextDataByte( void );

public:
	PNGImporterByteSource( ExAPAutoPtr<ExUtilFileReaderBaseInterface> file );
	virtual ~PNGImporterByteSource( void );
	void SetChunkLabelPointer( PNGChunkLabel * chunkLabel );
	void AllocateDecompressionBuffer( int decompressedDataBufferSize );
	virtual unsigned char ReadByte( void );
	virtual bool EndOfFile( void );
	virtual void AdvanceToNextLine( void );
}; // class PNGImporterByteSource


PNGImporterByteSource::PNGImporterByteSource( ExAPAutoPtr<ExUtilFileReaderBaseInterface> file )
	: m_file( file ),
		m_decompressedDataBufferSize( 0 ),
		m_readHead( 0 ),
		m_EOF( false ),
		m_EndOfDecompressionStreamReached( false ),
		m_DecompressionStreamInitialized( false )
{
	m_decompressionStream.zalloc = (alloc_func)0;
	m_decompressionStream.zfree = (free_func)0;
	m_decompressionStream.opaque = (voidpf)0;

	m_decompressionStream.next_in  = 0; //compr;
	m_decompressionStream.avail_in = 0; //(uInt)comprLen;
	m_decompressionStream.next_out = 0; //m_decompressedDataBuffer;
	m_decompressionStream.avail_out = 0; //(uInt)m_decompressedDataBufferSize;
} // PNGImporterByteSource::PNGImporterByteSource()


PNGImporterByteSource::~PNGImporterByteSource( void )
{

	if( m_DecompressionStreamInitialized )
	{
		inflateEnd( &m_decompressionStream );
	}
} // PNGImporterByteSource::~PNGImporterByteSource()


void PNGImporterByteSource::SetChunkLabelPointer( PNGChunkLabel * chunkLabel )
{
	m_chunkLabel = chunkLabel;
} // PNGImporterByteSource::SetChunkLabelPointer()


void PNGImporterByteSource::AllocateDecompressionBuffer( int decompressedDataBufferSize )
{
	
	if( m_decompressedDataBufferSize >= decompressedDataBufferSize )
	{
		return;
	}

	m_decompressedDataBuffer = 0;
	m_decompressedDataBufferSize = 0;

	try
	{
		m_decompressedDataBuffer = new ExAPAutoArray<unsigned char>( decompressedDataBufferSize );
	}
	catch( ExAPException & )
	{
		throw;
	}
	catch( ... )
	{
		// Do nothing.
	}

	if( m_decompressedDataBuffer == 0 )
	{
		ThrowExAPException( ExAPException::ResourceAcquisitionFailed );
	}

	m_decompressedDataBufferSize = decompressedDataBufferSize;
	m_readHead = *m_decompressedDataBuffer;
	m_decompressionStream.next_out = m_readHead;
	m_decompressionStream.avail_out = (uInt)m_decompressedDataBufferSize;
} // PNGImporterByteSource::AllocateDecompressionBuffer()


bool PNGImporterByteSource::NextDataByteIsReady( void ) const
{
	return( m_readHead != 0  &&  m_readHead < m_decompressionStream.next_out );
} // PNGImporterByteSource::NextDataByteIsReady()


bool PNGImporterByteSource::ReadNextIDATChunkFromFile( void )
{

	if( m_EOF )
	{
		return( false );		// There is no next (compressed) data byte in the file.
	}

	// Read the next IDAT chunk (if any) from the file.
	unsigned int chunkLength = 0;
	char chunkType[5];

	ExAssert( m_chunkLabel != 0 )

	if( m_chunkLabel->IsValid() )
	{
		chunkLength = m_chunkLabel->GetChunkDataLength();
		strcpy( chunkType, m_chunkLabel->GetChunkLabel() );
		m_chunkLabel->Reset();
#ifdef TAW_TRACE
		printf( "ByteSource: Using cached chunk type: '%s'.\n", chunkType );
#endif
	}
	else
	{
		chunkLength = m_file->GetDoubleWord();

		if( m_file->EndOfFile() )
		{
			m_EOF = true;
			return( false );
		}

		m_file->GetBytes( (unsigned char *)chunkType, 4 );
		chunkType[4] = '\0';
#ifdef TAW_TRACE
		printf( "ByteSource: Using non-cached chunk type: '%s'.\n", chunkType );
#endif
	}

	if( strcmp( chunkType, "IDAT" ) != 0 )
	{
		// It's not a data chunk, so there are no more IDAT chunks in the file.
		m_chunkLabel->Set( chunkLength, chunkType );
		m_EOF = true;
		return( false );
	}

	const int chunkLengthAsInt = int( chunkLength );

#ifdef TAW_TRACE
	printf( "ByteSource: IDAT chunk contains %d bytes of compressed data.\n", chunkLengthAsInt );
#endif

	m_compressedDataBuffer = new ExAPAutoArray<unsigned char>( chunkLengthAsInt );

	m_file->GetBytes( *m_compressedDataBuffer, chunkLengthAsInt );

	// Skip the CRC.
	m_file->SkipBytes( 4 );

	m_decompressionStream.next_in = *m_compressedDataBuffer;
	m_decompressionStream.avail_in = chunkLength;
	return( true );
} // PNGImporterByteSource::ReadNextIDATChunkFromFile()


bool PNGImporterByteSource::FindNextDataByte( void )
{

#if 1
	if( m_decompressionStream.avail_in == 0 )
	{
		ReadNextIDATChunkFromFile();
	}
#else
	if( m_decompressionStream.avail_in == 0  &&  !ReadNextIDATChunkFromFile() )
	{
		return( false );
	}
#endif

	if( m_EndOfDecompressionStreamReached )
	{
		return( false );		// There is no more data to decompress.
	}

	if( !m_DecompressionStreamInitialized )
	{
#ifdef TAW_TRACE
		printf( "ByteSource: Calling inflateInit()\n" );
#endif
    const int zlibInitErr = inflateInit( &m_decompressionStream );

		ExAssert( zlibInitErr == Z_OK )
		m_DecompressionStreamInitialized = true;
	}

#ifdef TAW_TRACE
	printf( "ByteSource: Calling inflate()\n" );
#endif

	const int zlibInflateErr = inflate( &m_decompressionStream, Z_NO_FLUSH );

#ifdef TAW_TRACE
	printf( "ByteSource: Returned from inflate()\n" );
#endif

	ExAssert( zlibInflateErr == Z_OK  ||  zlibInflateErr == Z_STREAM_END )

	if( zlibInflateErr == Z_STREAM_END )
	{
		m_EndOfDecompressionStreamReached = true;
	}

	// We are always at the beginning of the next chunk.
	//m_readHead = m_decompressedDataBuffer;
	return( true );
} // PNGImporterByteSource::FindNextDataByte()


unsigned char PNGImporterByteSource::ReadByte( void )
{

	if( !NextDataByteIsReady()  &&  !FindNextDataByte() )
	{
		return( 0 );
	}

	return( *m_readHead++ );
} // PNGImporterByteSource::ReadByte()


bool PNGImporterByteSource::EndOfFile( void )
{
	return( m_EOF );
} // PNGImporterByteSource::EndOfFile()


void PNGImporterByteSource::AdvanceToNextLine( void )
{
}


// **** Class PNGImporter ****


class PNGImporter
{
private:
	ExAPAutoPtr<ExUtilFileReaderBaseInterface> m_file;
	PNGImporterByteSource m_byteSource;
	ExAPAutoPtr<ExAPBitmap> m_image;
	unsigned char * m_imageBuffer;
	int m_interlaceMethod;

	static void UnfilterSub( unsigned char * currentPixel, int imageBytesPerPixel, int xStep, int imageWidth );
	static void UnfilterUp( unsigned char * currentPixel, const unsigned char * abovePixel, int imageBytesPerPixel, int xStep, int imageWidth );
	static void UnfilterAverage( unsigned char * currentPixel, const unsigned char * abovePixel, int imageBytesPerPixel, int xStep, int imageWidth );
	static unsigned char PaethPredictor( unsigned char left, unsigned char above, unsigned char aboveLeft );
	static void UnfilterPaeth( unsigned char * currentPixel, const unsigned char * abovePixel, int imageBytesPerPixel, int xStep, int imageWidth );

	void ReadImage( void );

public:
	PNGImporter( ExAPAutoPtr<ExUtilFileReaderBaseInterface> file );
	void ReadSignature( void );
	void ReadHeader( void );
	void ReadPostHeaderChunks( void );

	inline ExAPAutoPtr<ExAPBitmap> GetImagePtr( void ) const
	{
		return( m_image );
	}
}; // class PNGImporter


PNGImporter::PNGImporter( ExAPAutoPtr<ExUtilFileReaderBaseInterface> file )
	: m_file( file ),
		m_byteSource( file ),
		m_imageBuffer( 0 ),
		m_interlaceMethod( 0 )
{
} // PNGImporter::PNGImporter()


void PNGImporter::ReadSignature( void )
{
	// Read the file signature.
	// PNG header bytes: 137 80 78 71 13 10 26 10 (decimal).
	unsigned char signature[8];

	m_file->GetBytes( signature, 8 );

#ifdef TAW_TRACE
	if( signature[0] == 137  &&  signature[1] == 80  &&
		signature[2] == 78  &&  signature[3] == 71  &&
		signature[4] == 13  &&  signature[5] == 10  &&
		signature[6] == 26  &&  signature[7] == 10 )
	{
		printf( "PNG signature detected.\n" );
	}
	else
	{
		printf( "Error: PNG signature not detected.\n" );
	}
#endif

	ExAssert( signature[0] == 137 )
	ExAssert( signature[1] == 80 )
	ExAssert( signature[2] == 78 )
	ExAssert( signature[3] == 71 )
	ExAssert( signature[4] == 13 )
	ExAssert( signature[5] == 10 )
	ExAssert( signature[6] == 26 )
	ExAssert( signature[7] == 10 )
} // PNGImporter::ReadSignature()


void PNGImporter::ReadHeader( void )
{
	const unsigned int chunkLength = m_file->GetDoubleWord();

	//ExAssert( chunkLength == ? )
	ExAssert( !m_file->EndOfFile() )

	char chunkType[5];

	m_file->GetBytes( (unsigned char *)chunkType, 4 );
	chunkType[4] = '\0';

#ifdef TAW_TRACE
	printf( "Chunk type: '%s'.\n", chunkType );
#endif

	ExAssert( strcmp( chunkType, "IHDR" ) == 0 )

	const int imageWidth = int( m_file->GetDoubleWord() );
	const int imageHeight = int( m_file->GetDoubleWord() );
	const int bitDepth = m_file->GetByteAsInt();
	const int colourType = m_file->GetByteAsInt();
	const int compressionMethod = m_file->GetByteAsInt();
	const int filterMethod = m_file->GetByteAsInt();

	m_interlaceMethod = m_file->GetByteAsInt();		// m_interlaceMethod

	const bool paletteUsed = ( colourType & 1 ) != 0;
	const bool colourUsed = ( colourType & 2 ) != 0;
	const bool alphaChannelUsed = ( colourType & 4 ) != 0;

#ifdef TAW_TRACE
	printf( "  Image Width == %d.\n", imageWidth );
	printf( "  Image Height == %d.\n", imageHeight );
	printf( "  Bit Depth == %d.\n", bitDepth );
	printf( "  Colour Type == %d.\n", colourType );
	printf( "  Compression Method == %d.\n", compressionMethod );
	printf( "  Filter Method == %d.\n", filterMethod );
	printf( "  Interlace Method == %d.\n", m_interlaceMethod );
#endif

	ExAssert( !m_file->EndOfFile() )
	ExAssert( imageWidth > 0 )
	ExAssert( imageHeight > 0 )
	ExAssert( bitDepth == 8 )			// Temporary restriction.
	ExAssert( colourType == 2 )   // Temporary restriction.
	ExAssert( compressionMethod == 0 )
	ExAssert( filterMethod == 0 )
	ExAssert( m_interlaceMethod < 2 )

	try
	{

		switch( colourType )
		{
			case 2:
				{
					// We asserted above that bitDepth == 8.
					ExAPBitmapRGB24 * imageRGB24 = new ExAPBitmapRGB24( imageWidth, imageHeight );

					m_image = (ExAPBitmap *)imageRGB24;
					m_imageBuffer = imageRGB24->getBuffer();
				}

				break;

			default:
				break;
		}
	}
	catch( ExAPException & )
	{
		throw;
	}
	catch( ... )
	{
		// Do nothing.
	}

	ExAssert( m_image != 0 )

	// Skip the CRC.
	m_file->SkipBytes( 4 );

	// At most one filter-type byte per row per interlace pass.
	const int decompressedDataBufferSize = ( m_image->getBytesPerLine() + 7 ) * m_image->GetHeight();

	m_byteSource.AllocateDecompressionBuffer( decompressedDataBufferSize );
} // PNGImporter::ReadHeader()


void PNGImporter::UnfilterSub( unsigned char * currentPixel, int imageBytesPerPixel, int xStep, int imageWidth )
{
	const int bytesPerStep = xStep * imageBytesPerPixel;
	unsigned char * leftPixel = currentPixel;

	// Leave the first pixel unmodified.

	for( ; ; )
	{
		currentPixel += bytesPerStep;
		imageWidth -= xStep;

		if( imageWidth <= 0 )
		{
			break;
		}

		for( int i = 0; i < imageBytesPerPixel; ++i )
		{
			currentPixel[i] += leftPixel[i];
		}

		leftPixel = currentPixel;
	}
} // PNGImporter::UnfilterSub()


void PNGImporter::UnfilterUp( unsigned char * currentPixel, const unsigned char * abovePixel, int imageBytesPerPixel, int xStep, int imageWidth )
{

	if( abovePixel == 0 )
	{
		// Nothing to do.
		return;
	}

	const int bytesPerStep = xStep * imageBytesPerPixel;

	while( imageWidth > 0 )
	{

		for( int i = 0; i < imageBytesPerPixel; ++i )
		{
			currentPixel[i] += abovePixel[i];
		}

		currentPixel += bytesPerStep;
		abovePixel += bytesPerStep;
		imageWidth -= xStep;
	}
} // PNGImporter::UnfilterUp()


void PNGImporter::UnfilterAverage( unsigned char * currentPixel, const unsigned char * abovePixel, int imageBytesPerPixel, int xStep, int imageWidth )
{
	const int bytesPerStep = xStep * imageBytesPerPixel;
	int bytesPerStepAbove = bytesPerStep;
	static const unsigned char zeroByte = 0;
	static const int zeroOffsets[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };		// WARNING: Limit of 8 bytes per pixel.
	static const int normalOffsets[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
	const unsigned char * leftPixel = &zeroByte;
	const int * leftOffsets = zeroOffsets;
	const int * aboveOffsets = normalOffsets;

	if( abovePixel == 0 )
	{
		abovePixel = &zeroByte;
		aboveOffsets = zeroOffsets;
		bytesPerStepAbove = 0;
	}

	while( imageWidth > 0 )
	{

		for( int i = 0; i < imageBytesPerPixel; ++i )
		{
			currentPixel[i] += ( leftPixel[leftOffsets[i]] + abovePixel[aboveOffsets[i]] ) / 2;
		}

		leftPixel = currentPixel;
		leftOffsets = normalOffsets;
		currentPixel += bytesPerStep;
		abovePixel += bytesPerStepAbove;
		imageWidth -= xStep;
	}
} // PNGImporter::UnfilterAverage()


unsigned char PNGImporter::PaethPredictor( unsigned char left, unsigned char above, unsigned char aboveLeft )
{
	const int p = int( left ) + int( above ) - int( aboveLeft );
	const int pa = abs( p - int( left ) );
	const int pb = abs( p - int( above ) );
	const int pc = abs( p - int( aboveLeft ) );

	if( pa <= pb  &&  pa <= pc )
	{
		return( left );
	}

	if( pb <= pc )
	{
		return( above );
	}

	return( aboveLeft );
} // PNGImporter::PaethPredictor()


void PNGImporter::UnfilterPaeth( unsigned char * currentPixel, const unsigned char * abovePixel, int imageBytesPerPixel, int xStep, int imageWidth )
{
	const int bytesPerStep = xStep * imageBytesPerPixel;
	int bytesPerStepAbove = bytesPerStep;
	static const unsigned char zeroByte = 0;
	static const int zeroOffsets[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };		// WARNING: Limit of 8 bytes per pixel.
	static const int normalOffsets[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
	const unsigned char * leftPixel = &zeroByte;
	const int * leftOffsets = zeroOffsets;
	const unsigned char * aboveLeftPixel = &zeroByte;
	const int * aboveLeftOffsets = zeroOffsets;
	const int * aboveOffsets = normalOffsets;

	if( abovePixel == 0 )
	{
		abovePixel = &zeroByte;
		aboveOffsets = zeroOffsets;
		bytesPerStepAbove = 0;
	}

	while( imageWidth > 0 )
	{

		for( int i = 0; i < imageBytesPerPixel; ++i )
		{
			currentPixel[i] += PaethPredictor( leftPixel[leftOffsets[i]], abovePixel[aboveOffsets[i]], aboveLeftPixel[aboveLeftOffsets[i]] );
		}

		leftPixel = currentPixel;
		leftOffsets = normalOffsets;
		aboveLeftPixel = abovePixel;
		aboveLeftOffsets = aboveOffsets;
		currentPixel += bytesPerStep;
		abovePixel += bytesPerStepAbove;
		imageWidth -= xStep;
	}
} // PNGImporter::UnfilterPaeth()


void PNGImporter::ReadImage( void )
{
	// For each interlace pass, read filtered pixels from the decompressor
	// and then unfilter them.
	static const int adam7XOffset[7]	= { 0, 4, 0, 2, 0, 1, 0 };
	static const int adam7XStep[7]		= { 8, 8, 4, 4, 2, 2, 1 };
	static const int adam7YOffset[7]	= { 0, 0, 4, 0, 2, 0, 1 };
	static const int adam7YStep[7]		= { 8, 8, 8, 4, 4, 2, 2 };
	int numInterlacePasses = 1;
	int xOffset = 0;
	int xStep = 1;
	int yOffset = 0;
	int yStep = 1;

	if( m_interlaceMethod == 1 )
	{
		// Adam7 interlacing.
		numInterlacePasses = 7;
	}

	ExAssert( m_image != 0 )
	ExAssert( m_imageBuffer != 0 )

	const int imageWidth = m_image->GetWidth();
	const int imageHeight = m_image->GetHeight();
	const int imageBytesPerLine = m_image->getBytesPerLine();
	const int imageBytesPerPixel = m_image->GetBitsPerPixel() / 8;
	std::vector<unsigned char> filterType( imageHeight, 0 );

	for( int numInterlacePass = 0; numInterlacePass < numInterlacePasses; ++numInterlacePass )
	{
		// Read filtered pixels from the decompressor.

		if( m_interlaceMethod == 1 )
		{
			xOffset = adam7XOffset[numInterlacePass];
			xStep = adam7XStep[numInterlacePass];
			yOffset = adam7YOffset[numInterlacePass];
			yStep = adam7YStep[numInterlacePass];

			if( xOffset >= imageWidth  ||  yOffset >= imageHeight )
			{
#ifdef TAW_TRACE
				printf( "Skipping interlace pass %d.\n", numInterlacePass );
#endif
				continue;
			}
		}

#ifdef TAW_TRACE
		printf( "Starting interlace pass %d.\n", numInterlacePass );
#endif

		unsigned char * imageRow = m_imageBuffer + yOffset * imageBytesPerLine + xOffset * imageBytesPerPixel;
		int y = 0;

		for( y = yOffset; y < imageHeight; y += yStep )
		{
			// Read the filter byte.
			filterType[y] = m_byteSource.ReadByte();

#ifdef TAW_TRACE
			printf( "Row %d filter type: %d.\n", y, int( filterType[y] ) );
#endif

			ExAssert( int( filterType[y] ) < 5 )

			unsigned char * imagePixel = imageRow;

			imageRow += yStep * imageBytesPerLine;

			for( int x = xOffset; x < imageWidth; x += xStep )
			{
				
				for( int i = 0; i < imageBytesPerPixel; ++i )
				{
					imagePixel[i] = m_byteSource.ReadByte();
				}

				imagePixel += xStep * imageBytesPerPixel;
			}
		}

		// Unfilter the pixels, if necessary.
		// Filter type 0 : None
		// Filter type 1 : Sub
		// Filter type 2 : Up
		// Filter type 3 : Average
		// Filter type 4 : Paeth
#ifdef TAW_TRACE
		printf( "Starting unfiltering for interlace pass %d.\n", numInterlacePass );
#endif

		unsigned char * imagePreviousRow = 0;

		imageRow = m_imageBuffer + yOffset * imageBytesPerLine + xOffset * imageBytesPerPixel;

		for( y = yOffset; y < imageHeight; y += yStep )
		{
#ifdef TAW_TRACE
			printf( "Unfiltering row %d.\n", y );
#endif

			switch( filterType[y] )
			{
				case 0:		// None: do nothing.
					break;

				case 1:		// Sub
					UnfilterSub( imageRow, imageBytesPerPixel, xStep, imageWidth - xOffset );
					break;

				case 2:		// Up
					UnfilterUp( imageRow, imagePreviousRow, imageBytesPerPixel, xStep, imageWidth - xOffset );
					break;

				case 3:		// Average
					UnfilterAverage( imageRow, imagePreviousRow, imageBytesPerPixel, xStep, imageWidth - xOffset );
					break;

				case 4:		// Paeth
					UnfilterPaeth( imageRow, imagePreviousRow, imageBytesPerPixel, xStep, imageWidth - xOffset );
					break;

				default:
					ExSignal()
			}

			imagePreviousRow = imageRow;
			imageRow += yStep * imageBytesPerLine;
		}

#ifdef TAW_TRACE
		printf( "End of interlace pass %d.\n", numInterlacePass );
#endif
	}
} // PNGImporter::ReadImage()


void PNGImporter::ReadPostHeaderChunks( void )
{
	PNGChunkLabel chunkLabel;

	for( ; ; )
	{
		unsigned int chunkLength = 0;
		char chunkType[5];

		if( chunkLabel.IsValid() )
		{
			chunkLength = chunkLabel.GetChunkDataLength();
			strcpy( chunkType, chunkLabel.GetChunkLabel() );
			chunkLabel.Reset();
		}
		else
		{
			chunkLength = m_file->GetDoubleWord();

			if( m_file->EndOfFile() )
			{
				break;
			}

			m_file->GetBytes( (unsigned char *)chunkType, 4 );
			chunkType[4] = '\0';
		}

#ifdef TAW_TRACE
		printf( "Chunk type: '%s'.\n", chunkType );
#endif

		if( strcmp( chunkType, "IDAT" ) == 0 )
		{
			// A data chunk.
			chunkLabel.Set( chunkLength, chunkType );
			m_byteSource.SetChunkLabelPointer( &chunkLabel );
#ifdef TAW_TRACE
			printf( "Entering ReadImage()\n" );
#endif
			ReadImage();
#ifdef TAW_TRACE
			printf( "Exiting ReadImage()\n" );
#endif
			m_byteSource.SetChunkLabelPointer( 0 );
		}
		else if( chunkLength > 0 )	// Unsupported chunk type.
		{
			// Skip the chunk data.
			m_file->SkipBytes( int( chunkLength ) );
		}

		if( strcmp( chunkType, "IDAT" ) != 0 )
		{
			// Skip the CRC.
			m_file->SkipBytes( 4 );
		}
	}
} // PNGImporter::ReadPostHeaderChunks()


// **** Non-class Functions ****


ExAPAutoPtr<ExAPBitmap> ImportPNG( ExAPAutoPtr<ExUtilFileReaderBaseInterface> file ) throw( ExAPException )
{
	PNGImporter importer( file );

	importer.ReadSignature();
	importer.ReadHeader();
	importer.ReadPostHeaderChunks();

	return( importer.GetImagePtr() );
} // ImportPNG()


// **** End of File ****
