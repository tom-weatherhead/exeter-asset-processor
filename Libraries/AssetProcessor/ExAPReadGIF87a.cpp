/*
 *  ExAPReadGIF87a.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Sat Jun 28 2003.
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
#include "General\\ExAPException.h"
#include "CodecLib\\ExCodecLZW.h"
#include "UtilLib\\ExUtilBitByteSourceSinkInterface.h"
#include "UtilLib\\ExUtilFileReaderBaseInterface.h"
#include "ExAPBitmap8BitPaletted.h"
//lint +e1050


#define TAW_TRACE		1
//#define BYTE_SINK_HISTOGRAM		1


// **** Class ExAPGIF87aReader ****


class ExAPGIF87aReader
{
private:
	// The default constructor is never defined; it is declared here in order to satisfy lint.
	//lint -esym( 1526, ExAPGIF87aReader::ExAPGIF87aReader )
	//lint -e1704 : Constructor has private access specification.
	ExAPGIF87aReader( void );
	//lint +e1704

public:
	//lint -e1725 : Class member is a reference.
	ExAPAutoPtr<ExUtilFileReaderBaseInterface> m_file;
	//lint +e1725
	bool m_isGIF89a;
	bool m_EOF;
	bool m_headerHasBeenRead;
	ExAPAutoPtr<ExAPBitmap> m_image;
	ExAPBitmap8BitPaletted * m_palettedImage;

	// Header info.
	int m_screenWidth;
	int m_screenHeight;
	int m_bitsPerPixel;
	int m_bitsOfColourResolution;
	int m_globalColourMapIncluded;
	int m_indexOfScreenBackground;

	int m_currentImageLeft;
	int m_currentImageTop;
	int m_currentImageWidth;
	int m_currentImageHeight;
	int m_currentImageBitsPerPixel;
	bool m_isCurrentImageInterlaced;
	bool m_localColourMapIncluded;
	int m_imageWriteRowOffset;
	int m_imageWriteRowsRemaining;
	int m_imageWriteColsRemaining;
	unsigned char * m_imageWriteRow;
	unsigned char * m_imageWritePixel;

	int m_numBytesLeftInDataBlock;
	bool m_insideImageBlock;

	int m_interlacingPass;
	int m_interlacingRowStride;

	ExAPGIF87aReader( ExAPAutoPtr<ExUtilFileReaderBaseInterface> file, bool isGIF89a ) throw( ExAPException );
	void ReadHeader( void ) throw( ExAPException );
	bool FindNextDataByte( void ) throw( ExAPException );
	int GetNextDataByte( void ) throw( ExAPException );
	void WritePixelToImage( unsigned char pixel ) throw( ExAPException );
}; // class ExAPGIF87aReader


ExAPGIF87aReader::ExAPGIF87aReader( ExAPAutoPtr<ExUtilFileReaderBaseInterface> file, bool isGIF89a ) throw( ExAPException )
	: m_file( file ),
		m_isGIF89a( isGIF89a ),
		m_EOF( false ),
		m_headerHasBeenRead( false ),
		m_image( 0 ),
		m_palettedImage( 0 ),
		m_screenWidth( 0 ),
		m_screenHeight( 0 ),
		m_bitsPerPixel( 0 ),
		m_bitsOfColourResolution( 0 ),
		m_globalColourMapIncluded( 0 ),
		m_indexOfScreenBackground( 0 ),
		m_currentImageLeft( 0 ),
		m_currentImageTop( 0 ),
		m_currentImageWidth( 0 ),
		m_currentImageHeight( 0 ),
		m_currentImageBitsPerPixel( 0 ),
		m_isCurrentImageInterlaced( false ),
		m_localColourMapIncluded( false ),
		m_imageWriteRowOffset( 0 ),
		m_imageWriteRowsRemaining( 0 ),
		m_imageWriteColsRemaining( 0 ),
		m_imageWriteRow( 0 ),
		m_imageWritePixel( 0 ),
		m_numBytesLeftInDataBlock( 0 ),
		m_insideImageBlock( false ),
		m_interlacingPass( 0 ),
		m_interlacingRowStride( 0 )
{
	ReadHeader();
} // ExAPGIF87aReader::ExAPGIF87aReader()


void ExAPGIF87aReader::ReadHeader( void ) throw( ExAPException )
{
	// Read screen descriptor.
	unsigned char screenDescriptor[7];
	static const int sizeofScreenDescriptor = int( sizeof( screenDescriptor ) );

	if( m_file->GetBytes( screenDescriptor, sizeofScreenDescriptor ) != sizeofScreenDescriptor )
	{
#ifdef TAW_TRACE
		printf( "ReadGIF: Unable to read the screen descriptor.\n" );
#endif
		return;
	}

	m_screenWidth = int( (unsigned int)( screenDescriptor[1] ) * 256 + (unsigned int)( screenDescriptor[0] ) );
	m_screenHeight = int( (unsigned int)( screenDescriptor[3] ) * 256 + (unsigned int)( screenDescriptor[2] ) );
	m_bitsPerPixel = ( int( screenDescriptor[4] ) & 7 ) + 1;

#ifdef TAW_TRACE
	const int screenDescriptorByte4Bit3 = int( screenDescriptor[4] ) & 8;
#endif

	m_bitsOfColourResolution = ( ( int( screenDescriptor[4] ) >> 4 ) & 7 ) + 1;
	m_globalColourMapIncluded = ( int( screenDescriptor[4] ) & 128 ) != 0;
	m_indexOfScreenBackground = int( screenDescriptor[5] );

#ifdef TAW_TRACE
	const int screenDescriptorByte6 = int( screenDescriptor[6] );	// Must be zero.
#endif

#ifdef TAW_TRACE
	printf( "\nInfo in screen descriptor:\n" );
	printf( "  Screen width: %d\n", m_screenWidth );
	printf( "  Screen height: %d\n", m_screenHeight );
	printf( "  Bits per pixel: %d\n", m_bitsPerPixel );

	if( screenDescriptorByte4Bit3 != 0 )
	{
		printf( "  Warning: Bit 3 of byte 4 is not zero.\n" );
	}

	printf( "  Bits of colour resolution: %d\n", m_bitsOfColourResolution );
	printf( "  Global colour map is %sincluded.\n", m_globalColourMapIncluded ? "" : "not " );
	printf( "  Index of screen background: %d\n", m_indexOfScreenBackground );

	if( screenDescriptorByte6 != 0 )
	{
		printf( "  Warning: Byte 6 is not zero.\n" );
	}

	printf( "\n" );
#endif

	// TO_DO: Handle greyscale images?

	try
	{
		m_palettedImage = new ExAPBitmap8BitPaletted( ExAPBitmapPalette::PalCustom, m_screenWidth, m_screenHeight );
	}
	catch( ExAPException & )
	{
		throw;
	}
	catch( ... )
	{
	}

	if( m_palettedImage == 0 )
	{
		ThrowExAPException( ExAPException::ResourceAcquisitionFailed );
	}

	m_image = static_cast<ExAPBitmap *>( m_palettedImage );

	// Read the global colour map, if it exists.

	if( m_globalColourMapIncluded )
	{
#ifdef TAW_TRACE
		printf( "Reading global colour map.\n" );
#endif

		unsigned char colourMapEntry[3];
		static const int sizeofColourMapEntry = int( sizeof( colourMapEntry ) );
		const int globalColourMapSize = 1 << m_bitsPerPixel;

		for( int i = 0; i < globalColourMapSize; ++i )
		{

			if( m_file->GetBytes( colourMapEntry, sizeofColourMapEntry ) != sizeofColourMapEntry )
			{
#ifdef TAW_TRACE
				printf( "Error: Failed to read global colour map entry #%d.\n", i );
#endif
				return;
			}

#ifdef TAW_TRACE
			//if( i <= 8 )
			{
				printf( "Entry %d: Red = %d, Green = %d, Blue = %d\n", i,
					static_cast<int>( colourMapEntry[0] ), static_cast<int>( colourMapEntry[1] ), static_cast<int>( colourMapEntry[2] ) );
			}
#endif

			// Add the colour to the palette.
			m_palettedImage->getPalette().AddEntry( colourMapEntry[0], colourMapEntry[1], colourMapEntry[2] );
		}
	}
} // ExAPGIF87aReader::ReadHeader()


bool ExAPGIF87aReader::FindNextDataByte( void ) throw( ExAPException )
{

	if( m_EOF )
	{
		return( false );
	}

	if( m_numBytesLeftInDataBlock > 0 )
	{
		// More data follows immediately at the file pointer.
		return( true );
	}

#ifdef TAW_TRACE
	printf( "FindNextDataByte: Reached end of current data block.\n" );
#endif

	if( m_insideImageBlock )
	{
		// Look for the next data block.
		m_numBytesLeftInDataBlock = m_file->GetByteAsInt();

		if( m_numBytesLeftInDataBlock > 0 )
		{
			// More data follows immediately at the file pointer.
#ifdef TAW_TRACE
			printf( "FindNextDataByte: Next data block in this image contains %d bytes.\n", m_numBytesLeftInDataBlock );
#endif
			return( true );
		}
	}

#ifdef TAW_TRACE
	printf( "FindNextDataByte: Reached end of current image.\n" );
#endif

	// Read image block, extension block, or terminator.

	for( ; ; )
	{
		int chAsInt = 0;
		unsigned char ch = 0;
		
		for( ; ; )
		{
			chAsInt = m_file->GetByteAsInt();
			ch = static_cast<unsigned char>( chAsInt );

			if( chAsInt == EOF  ||  ch == ','  ||  ch == '!'  ||  ch == ';' )
			{
				break;
			}

#ifdef TAW_TRACE
			printf( "Searching for next block: Ignoring character 0x%02X.\n", chAsInt );
#endif
		}

		if( chAsInt == EOF )
		{
#ifdef TAW_TRACE
			printf( "Unexpected EOF.\n" );
#endif
			m_EOF = true;
			break;
		}
		else if( ch == ';' )
		{
#ifdef TAW_TRACE
			printf( "GIF Terminator found.\n" );
#endif
			m_EOF = true;
			break;
		}
		else if( ch == '!' )
		{
			// Read GIF Extension Block.
#ifdef TAW_TRACE
			printf( "Reading GIF Extension Block.\n" );
#endif

#ifdef TAW_TRACE
			const int functionCode = m_file->GetByteAsInt();
#else
			(void)m_file->GetByteAsInt();
#endif

#ifdef TAW_TRACE
			switch( functionCode )
			{
				case 0x01:
					printf( "  Extension block: Plain text extension.\n" );
					break;

				case 0xF9:
					printf( "  Extension block: Graphic control extension.\n" );
					break;

				case 0xFE:
					printf( "  Extension block: Comment extension.\n" );
					break;

				case 0xFF:
					printf( "  Extension block: Application extension.\n" );
					break;

				default:
					printf( "  Extension block: Unknown function code.\n" );
					break;
			}
#endif

			for( ; ; )
			{
				int chunkByteCount = m_file->GetByteAsInt();

				if( chunkByteCount == 0 )
				{
					break;
				}

				// TO_DO: Use a relative seek here.

				while( chunkByteCount-- > 0 )
				{
					// Eat the data; don't interpret it.
					(void)m_file->GetByteAsInt();
				}
			}
		}
		else if( ch == ',' )
		{
			// Read GIF Image Block.
#ifdef TAW_TRACE
			printf( "Reading GIF Image Block.\n" );
#endif

			m_insideImageBlock = true;
			m_interlacingPass = 0;
			m_imageWriteRowOffset = 0;
			m_interlacingRowStride = 8;

			// Read this block's Image Descriptor.
			// The comma has already been read.
			unsigned char imageDescriptor[9];
			static const int sizeofImageDescriptor = int( sizeof( imageDescriptor ) );

			if( m_file->GetBytes( imageDescriptor, sizeofImageDescriptor ) != sizeofImageDescriptor )
			{
#ifdef TAW_TRACE
				printf( "ReadGIF: Unable to read the image descriptor.\n" );
#endif
				break;
			}

			m_currentImageLeft = static_cast<int>( static_cast<unsigned int>( imageDescriptor[1] ) * 256 + static_cast<unsigned int>( imageDescriptor[0] ) );
			m_currentImageTop = static_cast<int>( static_cast<unsigned int>( imageDescriptor[3] ) * 256 + static_cast<unsigned int>( imageDescriptor[2] ) );
			m_currentImageWidth = static_cast<int>( static_cast<unsigned int>( imageDescriptor[5] ) * 256 + static_cast<unsigned int>( imageDescriptor[4] ) );
			m_currentImageHeight = static_cast<int>( static_cast<unsigned int>( imageDescriptor[7] ) * 256 + static_cast<unsigned int>( imageDescriptor[6] ) );

			const int imageDescriptorByte8 = static_cast<int>( imageDescriptor[8] );

			m_currentImageBitsPerPixel = ( imageDescriptorByte8 & 7 ) + 1;

#ifdef TAW_TRACE
			const int bits3to5 = imageDescriptorByte8 & 56;	// Must be zero.
#endif

			m_isCurrentImageInterlaced = ( imageDescriptorByte8 & 64 ) != 0;
			m_localColourMapIncluded = ( imageDescriptorByte8 & 128 ) != 0;

#ifdef TAW_TRACE
			printf( "\nInfo in image descriptor:\n" );
			printf( "  Image left: %d\n", m_currentImageLeft );
			printf( "  Image top: %d\n", m_currentImageTop );
			printf( "  Image width: %d\n", m_currentImageWidth );
			printf( "  Image height: %d\n", m_currentImageHeight );

			if( bits3to5 != 0 )
			{
				printf( "  Warning: Bits 3 to 5 of byte 8 are not all zero.\n" );
			}

			printf( "  Image is %sinterlaced.\n", m_isCurrentImageInterlaced ? "" : "not " );
			printf( "  Local colour map is %sincluded.\n", m_localColourMapIncluded ? "" : "not " );

			if( m_localColourMapIncluded )
			{
				printf( "  Image bits per pixel: %d\n", m_currentImageBitsPerPixel );
			}

			printf( "\n" );
#endif

			ExAssert( m_palettedImage != 0 )

			m_imageWriteRowOffset = 0;
			m_imageWriteRowsRemaining = m_currentImageHeight;
			m_imageWriteColsRemaining = m_currentImageWidth;
			m_imageWriteRow = m_palettedImage->getBuffer() + m_currentImageTop * m_palettedImage->getBytesPerLine() + m_currentImageLeft;
			m_imageWritePixel = m_imageWriteRow;

			// Read this block's image data.
#ifdef TAW_TRACE
			const int codeSize = m_file->GetByteAsInt();
#else
			(void)m_file->GetByteAsInt();
#endif

#ifdef TAW_TRACE
			printf( "This data block's code size: %d bits.\n", codeSize );
#endif

			m_numBytesLeftInDataBlock = m_file->GetByteAsInt();

#ifdef TAW_TRACE
			printf( "This data block contains %d bytes.\n", m_numBytesLeftInDataBlock );
#endif

			if( m_numBytesLeftInDataBlock > 0 )
			{
				return( true );
			}
		}
	}

	return( false );
} // ExAPGIF87aReader::FindNextDataByte()


int ExAPGIF87aReader::GetNextDataByte( void ) throw( ExAPException )
{

	if( m_numBytesLeftInDataBlock <= 0  &&  !FindNextDataByte() )
	{
		m_EOF = true;		// This is probably redundant.
		return( -1 );
	}

	const int rtn = m_file->GetByteAsInt();

	if( m_file->EndOfFile() )
	{
		m_EOF = true;
		m_numBytesLeftInDataBlock = 0;
	}
	else
	{
		m_numBytesLeftInDataBlock--;
	}

	return( rtn );
} // ExAPGIF87aReader::GetNextDataByte()


void ExAPGIF87aReader::WritePixelToImage( unsigned char pixel ) throw( ExAPException )
{

	if( m_image == 0  ||  m_imageWriteRowsRemaining <= 0 )
	{
		return;
	}

	ExAssert( m_imageWriteRow != 0 )
	ExAssert( m_imageWritePixel != 0 )

	*m_imageWritePixel++ = pixel;

	if( --m_imageWriteColsRemaining <= 0 )
	{
		m_imageWriteRowsRemaining--;

		if( !m_isCurrentImageInterlaced )
		{
			m_imageWriteRowOffset++;
			m_imageWriteRow += m_image->getBytesPerLine();
		}
		else
		{
			m_imageWriteRowOffset += m_interlacingRowStride;
			m_imageWriteRow += m_interlacingRowStride * m_image->getBytesPerLine();

			while( m_imageWriteRowOffset >= m_currentImageHeight )
			{
				static const int interlacingStartRows[5] = { 0, 4, 2, 1, 0 };
				static const int interlacingStrides[5] = { 8, 8, 4, 2, 0 };

				ExAssert( m_palettedImage != 0 )

				m_interlacingPass++;
				m_imageWriteRowOffset = interlacingStartRows[m_interlacingPass];
				m_interlacingRowStride = interlacingStrides[m_interlacingPass];
				m_imageWriteRow = m_palettedImage->getBuffer() + ( m_currentImageTop + m_imageWriteRowOffset ) * m_palettedImage->getBytesPerLine() + m_currentImageLeft;
			}
		}

		m_imageWriteColsRemaining = m_currentImageWidth;
		m_imageWritePixel = m_imageWriteRow;
	}
} // ExAPGIF87aReader::WritePixelToImage()


// **** Class ExAPBitSourceGIF87a ****


class ExAPBitSourceGIF87a : public ExUtilBitSourceInterface
{
private:
	// The default constructor is never defined; it is declared here in order to satisfy lint.
	//lint -esym( 1526, ExAPBitSourceGIF87a::ExAPBitSourceGIF87a )
	//lint -e1704 : Constructor has private access specification.
	ExAPBitSourceGIF87a( void );
	//lint +e1704

public:
	//lint -e1725 : Class member is a reference.
	ExAPGIF87aReader & m_reader;
	//lint +e1725
	unsigned int m_buffer;
	int m_numBitsInBuffer;

	explicit ExAPBitSourceGIF87a( ExAPGIF87aReader & reader );
	virtual int ReadBits( int numBits ) throw( ExAPException );	// Max of 31 bits.
	virtual int ReadBits( int numBits, int & bits );
	virtual bool EndOfFile( void );
}; // class ExAPBitSourceGIF87a


ExAPBitSourceGIF87a::ExAPBitSourceGIF87a( ExAPGIF87aReader & reader )
	: m_reader( reader ),
		m_buffer( 0 ),
		m_numBitsInBuffer( 0 )
{
}


int ExAPBitSourceGIF87a::ReadBits( int numBits ) throw( ExAPException )
{
	// Accumulate bytes in m_buffer until the requested number of bits are available.

	while( m_numBitsInBuffer < numBits )
	{
		const int x = m_reader.GetNextDataByte();

		if( /* x == -1  || */ m_reader.m_EOF )
		{
			return( -1 );
		}

		m_buffer |= (unsigned int)x << m_numBitsInBuffer;
		m_numBitsInBuffer += 8;		// 8 bits per byte.
	}

	const int rtn = m_buffer & (unsigned int)( ( 1 << numBits ) - 1 );

	m_buffer >>= numBits;
	m_numBitsInBuffer -= numBits;
#if 1 //def TAW_TRACE
	printf( "Bit source: Reading bits: %d; numBits: %d\n", rtn, numBits );
#endif
	return( rtn );
}


int ExAPBitSourceGIF87a::ReadBits( int numBits, int & bits )
{
	bits = ReadBits( numBits );
	return( numBits );
}


bool ExAPBitSourceGIF87a::EndOfFile( void )
{
	return( m_reader.m_EOF );
}


// **** Class ExAPByteSinkGIF87a ****


class ExAPByteSinkGIF87a : public ExUtilByteSinkInterface
{
private:
	// The default constructor is never defined; it is declared here in order to satisfy lint.
	//lint -esym( 1526, ExAPByteSinkGIF87a::ExAPByteSinkGIF87a )
	//lint -e1704 : Constructor has private access specification.
	ExAPByteSinkGIF87a( void );
	//lint +e1704

public:
	//lint -e1725 : Class member is a reference.
	ExAPGIF87aReader & m_reader;
	//lint +e1725
#ifdef BYTE_SINK_HISTOGRAM
	std::vector<int> m_hist;
	int m_vectorsWritten;
	int m_bytesToPrint;
#endif

	explicit ExAPByteSinkGIF87a( ExAPGIF87aReader & reader );
	virtual void WriteByte( unsigned char ch );
	virtual void WriteBytes( const std::vector<unsigned char> & bytes ) throw( ExAPException );
	virtual void AdvanceToNextLine( void );
#ifdef BYTE_SINK_HISTOGRAM
	void ReportMaxHistogramEntry( void ) const;
#endif
}; // class ExAPByteSinkGIF87a


ExAPByteSinkGIF87a::ExAPByteSinkGIF87a( ExAPGIF87aReader & reader )
	: m_reader( reader )
#ifdef BYTE_SINK_HISTOGRAM
	, m_hist( 256, 0 )
	, m_vectorsWritten( 0 )
	, m_bytesToPrint( 100 )
#endif
{
}


void ExAPByteSinkGIF87a::WriteByte( unsigned char ch )
{
	m_reader.WritePixelToImage( ch );
} // ExAPByteSinkGIF87a::WriteByte()


void ExAPByteSinkGIF87a::WriteBytes( const std::vector<unsigned char> & bytes ) throw( ExAPException )
{
	// Write the vector of bytes to the right part of the image buffer,
	// taking interlacing into account, if necessary.
	std::vector<unsigned char>::const_iterator i = bytes.begin();
	const std::vector<unsigned char>::const_iterator iEnd = bytes.end();

#ifdef BYTE_SINK_HISTOGRAM
	m_vectorsWritten++;
#endif

	while( i != iEnd )
	{
		const unsigned char ch = *i++;

#ifdef BYTE_SINK_HISTOGRAM
		const int chAsInt = static_cast<int>( ch );

		m_hist[chAsInt]++;
#endif
		m_reader.WritePixelToImage( ch );
	}
} // ExAPByteSinkGIF87a::WriteBytes()


void ExAPByteSinkGIF87a::AdvanceToNextLine( void )
{
}


#ifdef BYTE_SINK_HISTOGRAM
void ExAPByteSinkGIF87a::ReportMaxHistogramEntry( void ) const
{
	int maxIndex = 0;
	int maxEntry = -1;
	int totalPopulation = 0;
	int numZeroEntries = 0;

	for( int i = 0; i < 256; ++i )
	{

		if( m_hist[i] == 0 )
		{
			numZeroEntries++;
		}
		else
		{
			totalPopulation += m_hist[i];
			printf( "Histogram: Entry %d is %d.\n", i, m_hist[i] );
		}

		if( m_hist[i] > maxEntry )
		{
			maxEntry = m_hist[i];
			maxIndex = i;
		}
	}

	printf( "Most common output byte: %d (%d%%)\n", maxIndex, maxEntry * 100 / totalPopulation );
	printf( "  A total of %d bytes were written.\n", totalPopulation );
	printf( "  A total of %d vectors were written.\n", m_vectorsWritten );
	printf( "  Average vector length: %d.\n", totalPopulation / m_vectorsWritten );
	printf( "  A total of %d histogram entries are zero.\n", numZeroEntries );
} // ExAPByteSinkGIF87a::ReportMaxHistogramEntry()
#endif


// **** Non-class functions ****


static ExAPAutoPtr<ExAPBitmap> ImportGIF87a( ExAPAutoPtr<ExUtilFileReaderBaseInterface> file, bool isGIF89a ) throw( ExAPException )
{
	ExAPGIF87aReader reader( file, isGIF89a );
	ExAPAutoPtr<ExUtilBitSourceInterface> bitSource = new ExAPBitSourceGIF87a( reader );
	ExAPAutoPtr<ExUtilByteSinkInterface> byteSink = new ExAPByteSinkGIF87a( reader );
	ExAPAutoPtr<ExAPLZWCodec> codec = ExAPLZWCodec::CreateCodec( ExAPLZWCodec::TypeGIF, reader.m_bitsPerPixel );

	//while( !reader.m_EOF )
	{
		//ExAPLZWCodecGIF87a::DecompressData( bitSource, byteSink, reader.m_bitsPerPixel );
		codec->DecompressData( bitSource, byteSink );
	}

#ifdef BYTE_SINK_HISTOGRAM
	byteSink.ReportMaxHistogramEntry();
#endif

#ifdef TAW_TRACE
	printf( "Finished reading %s file.\n", isGIF89a ? "GIF89a" : "GIF87a" );
#endif

	return( reader.m_image );
} // ImportGIF87a()


ExAPAutoPtr<ExAPBitmap> ImportGIF( ExAPAutoPtr<ExUtilFileReaderBaseInterface> file ) throw( ExAPException )
{
	ExAPAutoPtr<ExAPBitmap> image = 0;
	unsigned char signature[6];
	static const int sizeofSignature = int( sizeof( signature ) );

	memset( signature, 0, sizeof( signature ) );

	if( file->GetBytes( signature, sizeofSignature ) != sizeofSignature )
	{
#ifdef TAW_TRACE
		printf( "ReadGIF: Unable to read GIF file signature.\n" );
#endif
	}
	else if( !memcmp( signature, "GIF87a", 6 ) )
	{
#ifdef TAW_TRACE
		printf( "ReadGIF: Recognized GIF87a file signature.\n" );
#endif
		image = ImportGIF87a( file, false );
	}
	else if( !memcmp( signature, "GIF89a", 6 ) )
	{
#ifdef TAW_TRACE
		printf( "ReadGIF: Recognized GIF89a file signature.\n" );
#endif
		image = ImportGIF87a( file, true );
	}
	else
	{
#ifdef TAW_TRACE
		printf( "ReadGIF: File signature not recognized.\n" );

		for( int i = 0; i < 6; ++i )
		{
			printf( "0x%02X ", signature[i] );
		}

		printf( "\n" );
#endif
	}

	return( image );
} // ImportGIF()


// **** End of File ****
