/*
 *  ExAPImportBMP.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Wed Jul 30 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#define TAW_TRACE		1

#ifdef TAW_TRACE
#include <cstdio>
#endif

#include "UtilLib\\ExUtilFileReaderBaseInterface.h"
#include "ExAPBitmap8BitPaletted.h"
#include "ExAPBitmapRGB24.h"


ExAPAutoPtr<ExAPBitmap> ImportBMP( ExAPAutoPtr<ExUtilFileReaderBaseInterface> file ) throw( ExAPException )
{
	// Read the file header (14 bytes).
	const int headerByte0 = file->GetByteAsInt();
	const int headerByte1 = file->GetByteAsInt();

#ifdef TAW_TRACE
	if( headerByte0 == 'B'  &&  headerByte1 == 'M' )
	{
		printf( "File is of type BMP.\n" );
	}
	else
	{
		printf( "Error: File is not of type BMP.\n" );
	}
#endif

	ExAssert( headerByte0 == 'B' )
	ExAssert( headerByte1 == 'M' )
	file->SkipBytes( 8 );

	const unsigned int offset = file->GetDoubleWord();

	// Read the info header (40 bytes in a Windows BMP).
	// The infoHeaderSize is part of the header whose size it indicates.
	const unsigned int infoHeaderSize = file->GetDoubleWord();

#ifdef TAW_TRACE
	if( infoHeaderSize == 40 )
	{
		printf( "Info header is the right size for a Windows BMP (40 bytes).\n" );
	}
	else
	{
		printf( "Error: Info header is %u bytes; it should be 40 bytes.\n", infoHeaderSize );
	}
#endif

	ExAssert( infoHeaderSize == 40 )

	const unsigned int imageWidth = file->GetDoubleWord();
	const unsigned int imageHeight = file->GetDoubleWord();
	const unsigned short imageNumPlanes = file->GetWord();
	const unsigned short imageBitsPerPixel = file->GetWord();
	const unsigned int imageCompression = file->GetDoubleWord();

	// SizeImage and ClrImportant are skipped.
	file->SkipBytes( 4 );

	const unsigned int imageNumXPixelsPerMeter = file->GetDoubleWord();
	const unsigned int imageNumYPixelsPerMeter = file->GetDoubleWord();

	// Number of colours in the palette.
	unsigned int numPaletteColours = file->GetDoubleWord();

	// SizeImage and ClrImportant are skipped.
	file->SkipBytes( 4 );

#ifdef TAW_TRACE
	printf( "Image width: %u\n", imageWidth );
	printf( "Image height: %u\n", imageHeight );
	printf( "Number of planes: %hu\n", imageNumPlanes );
	printf( "Bits per pixel: %hu\n", imageBitsPerPixel );
	printf( "Compression: %u\n", imageCompression );
	printf( "X pixels per meter: %u\n", imageNumXPixelsPerMeter );
	printf( "Y pixels per meter: %u\n", imageNumYPixelsPerMeter );
	printf( "Number of colours in the palette: %u\n", numPaletteColours );
#endif

	ExAssert( imageWidth > 0 )
	ExAssert( imageHeight > 0 )
	ExAssert( imageNumPlanes == 1 )
	ExAssert( imageBitsPerPixel == 8  ||  imageBitsPerPixel == 24 )
	ExAssert( imageCompression == 0 )

	// End of info header.

	// Read the palette, if any.
	ExAPBitmapPalette palette( ExAPBitmapPalette::PalCustom );

	if( imageBitsPerPixel == 8 )
	{
		ExAssert( numPaletteColours <= 256 )

		if( numPaletteColours == 0 )
		{
			numPaletteColours = 256;
		}

		unsigned char palColourBuf[4];

		for( unsigned int i = 0; i < numPaletteColours; ++i )
		{
			file->GetBytes( palColourBuf, 4 );
			// Add the colour to the palette.
			palette.m_entries.push_back( ExAPBitmapRGBPaletteEntry( palColourBuf[2], palColourBuf[1], palColourBuf[0] ) );
		}
	}

	// Create the image.
	ExAPBitmap * imageBaseRaw = 0;
	ExAPBitmap8BitPaletted * imagePalettedRaw = 0;
	ExAPBitmapRGB24 * imageRGB24Raw = 0;

	try
	{

		switch( imageBitsPerPixel )
		{
			case 8:
				imagePalettedRaw = new ExAPBitmap8BitPaletted( palette, int( imageWidth ), int( imageHeight ) );
				imageBaseRaw = imagePalettedRaw;
				break;

			case 24:
				imageRGB24Raw = new ExAPBitmapRGB24( int( imageWidth ), int( imageHeight ) );
				imageBaseRaw = imageRGB24Raw;
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
		// Do nothing here.
	}

	if( imageBaseRaw == 0 )
	{
		ThrowExAPException( ExAPException::ResourceAcquisitionFailed );
	}

	// Set the AutoPtr now so that the image will be freed if an exception is thrown.
	ExAPAutoPtr<ExAPBitmap> image = imageBaseRaw;

	// Read the image data.
	file->SetOffset( long( offset ) );

	// srcBytesPerRow is 4-byte aligned.
	const int srcBytesPerRow = ( ( int( imageWidth ) * imageBitsPerPixel / 8 ) + 3 ) & ~3;

	if( imageBitsPerPixel == 8 )
	{
		ExAPAutoArray<unsigned char> srcBuf( srcBytesPerRow );
		const int dstBytesPerRow = imagePalettedRaw->getBytesPerLine();
		unsigned char * dstRow = imagePalettedRaw->getBuffer() + ( int( imageHeight ) - 1 ) * dstBytesPerRow;

		for( unsigned int y = 0; y < imageHeight; ++y )
		{
			file->GetBytes( srcBuf, srcBytesPerRow );
			memcpy( dstRow, srcBuf, imageWidth );
			dstRow -= dstBytesPerRow;
		}
	}
	else
	{
		// 24-bit RGB.  The Red and Blue channels must be swapped.
		const int dstBytesPerRow = imageRGB24Raw->getBytesPerLine();
		unsigned char * dstRow = imageRGB24Raw->getBuffer() + ( int( imageHeight ) - 1 ) * dstBytesPerRow;
		const int srcBytesToSkipPerRow = srcBytesPerRow - 3 * int( imageWidth );

		for( unsigned int y = 0; y < imageHeight; ++y )
		{
			unsigned char * dstPixel = dstRow;
			
			dstRow -= dstBytesPerRow;

			for( unsigned int x = 0; x < imageWidth; ++x )
			{
				dstPixel[2] = (unsigned char)( file->GetByteAsInt() );
				dstPixel[1] = (unsigned char)( file->GetByteAsInt() );
				dstPixel[0] = (unsigned char)( file->GetByteAsInt() );
				dstPixel += 3;
			}

			file->SkipBytes( srcBytesToSkipPerRow );
		}
	}

#ifdef TAW_TRACE
	printf( "End of ImportBMP()\n" );
#endif

	return( image );
} // ImportBMP()


// **** End of File ****
