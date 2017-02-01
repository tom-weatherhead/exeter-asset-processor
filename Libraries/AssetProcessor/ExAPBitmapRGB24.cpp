/*
 *  ExAPBitmapRGB24.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Sat May 03 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#include "ExAPBitmapRGB24.h"
#include "ExAPConversionTemplates.h"
#include "ExAPMedianCut.h"


// Constructor.

ExAPBitmapRGB24::ExAPBitmapRGB24( int width, int height ) throw( ExAPException )
	: ExAPBitmapXXX24( ExAPColourModel::RGB24, width, height )
{
}
	

// Destructor.

ExAPBitmapRGB24::~ExAPBitmapRGB24( void )
{
}


ExAPAutoPtr<ExAPBitmap> ExAPBitmapRGB24::CreateCompatibleBitmap( int width, int height ) const throw( ExAPException )
{
#ifdef EXAP_USE_CREATEIMAGE_FUNCTION_TEMPLATE
	return( CreateImage<ExAPBitmapRGB24,ExAPBitmap>( width, height ) );
#else
	ExAPAutoPtr<ExAPBitmap> image;	// Initialized to zero.

	try
	{
		image = new ExAPBitmapRGB24( width, height );
	}
	catch( ExAPException & )
	{
		// We caught a known exception; re-throw it.
		throw;
	}
	catch( ... )
	{
		// We caught an unknown exception.
		ThrowExAPException( ExAPException::UnknownException );
	}

	if( image == 0 )
	{
		ThrowExAPException( ExAPException::ResourceAcquisitionFailed );
	}

	return( image );
#endif
} // ExAPBitmapRGB24::CreateCompatibleBitmap()


void ExAPBitmapRGB24::ConvertToRGBA64( unsigned short * dstRow, int dstSamplesPerRow,
	const ExAPRect & srcRect ) const
{
	// dstRow is the upper-left corner of the dst image chunk.
	const unsigned char * srcRow = m_buffer + srcRect.getTop() * m_bytesPerLine + srcRect.getLeft() * 3;
	const int srcRectWidth = srcRect.getWidth();
	int srcRectHeight = srcRect.getHeight();

	while( srcRectHeight-- > 0 )
	{
		unsigned short * dstPixel = dstRow;
		const unsigned char * srcPixel = srcRow;

		dstRow += dstSamplesPerRow;
		srcRow += m_bytesPerLine;

		for( int i = 0; i < srcRectWidth; ++i )
		{
			dstPixel[0] = (unsigned short)( 257 * srcPixel[0] );
			dstPixel[1] = (unsigned short)( 257 * srcPixel[1] );
			dstPixel[2] = (unsigned short)( 257 * srcPixel[2] );
			dstPixel[3] = 65535;
			srcPixel += 3;
			dstPixel += 4;
		}
	}
} // ExAPBitmapRGB24::ConvertToRGBA64()


void ExAPBitmapRGB24::ConvertFromRGBA64( const unsigned short * srcRow, int srcSamplesPerRow,
	const ExAPRect & dstRect ) const
{
	// srcRow is the upper-left corner of the src image chunk.
	unsigned char * dstRow = m_buffer + dstRect.getTop() * m_bytesPerLine + dstRect.getLeft() * 3;
	const int dstRectWidth = dstRect.getWidth();
	int dstRectHeight = dstRect.getHeight();

	while( dstRectHeight-- > 0 )
	{
		unsigned char * dstPixel = dstRow;
		const unsigned short * srcPixel = srcRow;

		dstRow += m_bytesPerLine;
		srcRow += srcSamplesPerRow;

		for( int i = 0; i < dstRectWidth; ++i )
		{
			dstPixel[0] = (unsigned char)( srcPixel[0] / 256 );
			dstPixel[1] = (unsigned char)( srcPixel[1] / 256 );
			dstPixel[2] = (unsigned char)( srcPixel[2] / 256 );
			srcPixel += 4;
			dstPixel += 3;
		}
	}
} // ExAPBitmapRGB24::ConvertFromRGBA64()


void ExAPBitmapRGB24::ExportToGIF( const std::string & dstFilename ) const
{
	ExAPAutoPtr<ExAPBitmap8BitPaletted> dstPalettedImage = CreatePalettedImageViaMedianCut(
		*this, getBoundingRect(), 256 );

	dstPalettedImage->ExportToGIF( dstFilename );
} // ExAPBitmapRGB24::ExportToGIF()


// **** End of File ****
