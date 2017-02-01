/*
 *  ExAPExportGIF.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Wed Jul 23 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

//lint -e1050 : Error -- expected a template argument list '<...>' for template 'std::iterator<<1>,<2>,<3>>'.
#include <queue>

#include "CodecLib\\ExCodecLZW.h"
#include "UtilLib\\ExUtilBitByteSourceSinkInterface.h"
#include "UtilLib\\ExUtilFileWriterBaseInterface.h"
#include "ExAPBitmap8BitPaletted.h"
//lint +e1050


//#define TAW_TRACE		1


// **** Class ExAPExportGIFByteSource ****


class ExAPExportGIFByteSource : public ExUtilByteSourceInterface
{
private:
	const ExAPBitmap8BitPaletted & m_srcImage;
	int m_numRowsRemaining;
	int m_numColsRemaining;
	unsigned char * m_imageRowPtr;
	unsigned char * m_imagePixelPtr;
	bool m_endOfFile;

public:
	ExAPExportGIFByteSource( const ExAPBitmap8BitPaletted & srcImage );
	virtual unsigned char ReadByte( void );
	virtual bool EndOfFile( void );
	virtual void AdvanceToNextLine( void );
}; // class ExAPExportGIFByteSource


ExAPExportGIFByteSource::ExAPExportGIFByteSource( const ExAPBitmap8BitPaletted & srcImage )
	: m_srcImage( srcImage ),
		m_numRowsRemaining( m_srcImage.GetHeight() ),
		m_numColsRemaining( m_srcImage.GetWidth() ),
		m_imageRowPtr( m_srcImage.getBuffer() ),
		m_imagePixelPtr( m_imageRowPtr ),
		m_endOfFile( false )
{
	m_imageRowPtr += m_srcImage.getBytesPerLine();
} // ExAPExportGIFByteSource::ExAPExportGIFByteSource()


unsigned char ExAPExportGIFByteSource::ReadByte( void )
{

	if( m_endOfFile )
	{
		return( 0 );
	}

	if( m_numRowsRemaining <= 0 )
	{
		m_endOfFile = true;
		return( 0 );
	}

	unsigned char rtn = *m_imagePixelPtr++;

	if( --m_numColsRemaining <= 0 )
	{
		m_numColsRemaining = m_srcImage.GetWidth();
		m_numRowsRemaining--;
		m_imagePixelPtr = m_imageRowPtr;
		m_imageRowPtr += m_srcImage.getBytesPerLine();
	}

	return( rtn );
} // ExAPExportGIFByteSource::ReadByte()


bool ExAPExportGIFByteSource::EndOfFile( void )
{
	return( m_endOfFile );
} // ExAPExportGIFByteSource::EndOfFile()


void ExAPExportGIFByteSource::AdvanceToNextLine( void )
{
}


// **** Class ExAPExportGIFBitSink ****


class ExAPExportGIFBitSink : public ExUtilBitSinkInterface
{
private:
	std::queue<unsigned char> m_q;
	//lint -e1725 : Class member is a reference.
	ExAPAutoPtr<ExUtilFileWriterBaseInterface> m_file;
	//lint +e1725
	unsigned int m_buffer;
	int m_numBitsInBuffer;

	void FlushBytesFromQueueToFile( size_t nBytes );

public:
	ExAPExportGIFBitSink( ExAPAutoPtr<ExUtilFileWriterBaseInterface> file );
	//~ExAPExportGIFBitSink( void );
	virtual void WriteBits( int bits, int numBits );
	virtual void Flush( void );
}; // class ExAPExportGIFBitSink


ExAPExportGIFBitSink::ExAPExportGIFBitSink( ExAPAutoPtr<ExUtilFileWriterBaseInterface> file )
	: m_file( file ),
		m_buffer( 0 ),
		m_numBitsInBuffer( 0 )
{
} // ExAPExportGIFBitSink::ExAPExportGIFBitSink()


#if 0
ExAPExportGIFBitSink::~ExAPExportGIFBitSink( void )
{
} // ExAPExportGIFBitSink::~ExAPExportGIFBitSink()
#endif


void ExAPExportGIFBitSink::FlushBytesFromQueueToFile( size_t nBytes )
{
	ExAssert( nBytes >= m_q.size() )

	while( nBytes-- > 0 )
	{
		const unsigned char ch = m_q.front();

		m_q.pop();
		m_file->WriteByte( int( ch ) );
	}
} // ExAPExportGIFBitSink::FlushBytesFromQueueToFile()


void ExAPExportGIFBitSink::WriteBits( int bits, int numBits )
{
#ifdef TAW_TRACE
	printf( "Bit sink: Writing bits: %d; numBits: %d\n", bits, numBits );
#endif

	m_buffer |= (unsigned int)bits << m_numBitsInBuffer;
	m_numBitsInBuffer += numBits;

	while( m_numBitsInBuffer >= 8 )
	{
		const unsigned char ch = (unsigned char)( m_buffer & 0xFF );

		m_buffer >>= 8;
		m_numBitsInBuffer -= 8;
		m_q.push( ch );

		if( m_q.size() == 255 )
		{
			m_file->WriteByte( 255 );
			FlushBytesFromQueueToFile( 255 );
		}
	}
} // ExAPExportGIFBitSink::WriteBits()


void ExAPExportGIFBitSink::Flush( void )
{

	if( m_numBitsInBuffer > 0 )
	{
		m_q.push( (unsigned char)m_buffer );
		m_numBitsInBuffer = 0;
	}

	if( m_q.size() > 0 )
	{
		m_file->WriteByte( int( m_q.size() ) );
		FlushBytesFromQueueToFile( m_q.size() );
	}
	
	// Write the terminating zero-length data chunk.
	m_file->WriteByte( 0 );
} // ExAPExportGIFBitSink::Flush()


// **** Class ExAPBitmap8BitPaletted ****


void ExAPBitmap8BitPaletted::ExportToGIF( const std::string & dstFilename ) const
{
	// 0) Open the file.
	ExAPAutoPtr<ExUtilFileWriterBaseInterface> file = ExUtilFileWriterBaseInterface::StdioCreate( dstFilename );

	// 1) Write the GIF signature.
	unsigned char gifSignature[6];

	memcpy( gifSignature, "GIF87a", 6 );
	file->WriteBytes( gifSignature, sizeof( gifSignature ) );

	// 2) Write the screen descriptor.
	unsigned char screenDescriptor[7];
	const unsigned short screenWidth = (unsigned short)m_width;
	const unsigned short screenHeight = (unsigned short)m_height;
	static const unsigned int imageBitsPerPixel = 8;
	static const unsigned int numBitsOfColourResolution = 8;
	static const bool globalColourMapExists = true;
	const unsigned char screenDescriptorByte4 = (unsigned char)(
		( imageBitsPerPixel - 1 ) |
		// Bit 3 == 0 (Sort flag in GIF89a)
		( ( numBitsOfColourResolution - 1 ) << 4 ) |
		( globalColourMapExists ? 128 : 0 ) );

	screenDescriptor[0] = (unsigned char)( screenWidth & 0xFF );
	screenDescriptor[1] = (unsigned char)( screenWidth >> 8 );
	screenDescriptor[2] = (unsigned char)( screenHeight & 0xFF );
	screenDescriptor[3] = (unsigned char)( screenHeight >> 8 );
	screenDescriptor[4] = screenDescriptorByte4;
	screenDescriptor[5] = 0;	// Colour index of screen background.
	screenDescriptor[6] = 0;	// Pixel aspect ratio in GIF89a.
	file->WriteBytes( screenDescriptor, sizeof( screenDescriptor ) );

	// 3) Write the palette (global colour map).
	int i = 0;
	const int numColoursInPalette = m_palette.GetNumEntries();

	for( ; i < numColoursInPalette; ++i )
	{
		const ExAPBitmapRGBPaletteEntry & paletteEntry = m_palette.GetEntry( i );

		file->WriteByte( int( paletteEntry.GetRed() ) );
		file->WriteByte( int( paletteEntry.GetGreen() ) );
		file->WriteByte( int( paletteEntry.GetBlue() ) );
	}

	int numPalettePadBytes = 3 * ( 256 - numColoursInPalette );

	while( numPalettePadBytes-- > 0 )
	{
		file->WriteByte( 0 );
	}

	// 4) Write the image descriptor.
	unsigned char imageDescriptor[10];
	static const bool imageIsInterlaced = false;
	static const bool localColourMapExists = false;
	const unsigned char imageDescriptorByte9 = (unsigned char)(
		( imageIsInterlaced ? 64 : 0 ) |
		( localColourMapExists ? 128 : 0 ) );

	imageDescriptor[0] = ',';
	imageDescriptor[1] = 0;		// LSB of image left offset.
	imageDescriptor[2] = 0;		// MSB of image left offset.
	imageDescriptor[3] = 0;		// LSB of image top offset.
	imageDescriptor[4] = 0;		// MSB of image top offset.
	imageDescriptor[5] = (unsigned char)( screenWidth & 0xFF );
	imageDescriptor[6] = (unsigned char)( screenWidth >> 8 );
	imageDescriptor[7] = (unsigned char)( screenHeight & 0xFF );
	imageDescriptor[8] = (unsigned char)( screenHeight >> 8 );
	imageDescriptor[9] = imageDescriptorByte9;
	file->WriteBytes( imageDescriptor, sizeof( imageDescriptor ) );

	// No local colour map.

	// 5) Write the LZW-compressed raster data.
	file->WriteByte( int( imageBitsPerPixel ) );	// LZW code size; must be at least 2.

	ExAPAutoPtr<ExUtilByteSourceInterface> byteSource = new ExAPExportGIFByteSource( *this );
	ExAPAutoPtr<ExUtilBitSinkInterface> bitSink = new ExAPExportGIFBitSink( file );
	ExAPAutoPtr<ExAPLZWCodec> codec = ExAPLZWCodec::CreateCodec( ExAPLZWCodec::TypeGIF, imageBitsPerPixel );

	codec->CompressData( byteSource, bitSink );

	// 6) Write the GIF terminator.
	file->WriteByte( ';' );
} // ExAPBitmap8BitPaletted::ExportToGIF()


// **** End of File ****
