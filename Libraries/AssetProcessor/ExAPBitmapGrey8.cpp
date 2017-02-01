/*
 *  ExAPBitmapGrey8.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Sun Jun 22 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#include "ExAPBitmapGrey8.h"
#include "ExAPConversionTemplates.h"


// Constructor.

ExAPBitmapGrey8::ExAPBitmapGrey8( int width, int height ) throw( ExAPException )
	: ExAPBitmap8bpchNoAlpha( ExAPColourModel::Grey8, width, height )
{
}
	

// Destructor.

ExAPBitmapGrey8::~ExAPBitmapGrey8( void )
{
}


ExAPAutoPtr<ExAPBitmap> ExAPBitmapGrey8::CreateCompatibleBitmap( int width, int height ) const throw( ExAPException )
{
#ifdef EXAP_USE_CREATEIMAGE_FUNCTION_TEMPLATE
	return( CreateImage<ExAPBitmapGrey8,ExAPBitmap>( width, height ) );
#else
	ExAPAutoPtr<ExAPBitmap> image;	// Initialized to zero.

	try
	{
		image = new ExAPBitmapGrey8( width, height );
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
} // ExAPBitmapGrey8::CreateCompatibleBitmap()


void ExAPBitmapGrey8::ConvertToRGBA64( unsigned short * dstRow, int dstSamplesPerRow,
	const ExAPRect & srcRect ) const
{
	// dstRow is the upper-left corner of the dst image chunk.
	const unsigned char * srcRow = m_buffer + srcRect.getTop() * m_bytesPerLine + srcRect.getLeft();
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
			const unsigned short luminance = (unsigned short)( 257 * *srcPixel++ );

			dstPixel[0] = luminance;
			dstPixel[1] = luminance;
			dstPixel[2] = luminance;
			dstPixel[3] = 65535;
			dstPixel += 4;
		}
	}
} // ExAPBitmapGrey8::ConvertToRGBA64()


void ExAPBitmapGrey8::ConvertFromRGBA64( const unsigned short * srcRow, int srcSamplesPerRow,
	const ExAPRect & dstRect ) const
{
	// srcRow is the upper-left corner of the src image chunk.
	unsigned char * dstRow = m_buffer + dstRect.getTop() * m_bytesPerLine + dstRect.getLeft();
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
			// Grey == 0.30 * Red + 0.59 * Green + 0.11 * Blue.
			*dstPixel++ = (unsigned char)( ( 30 * srcPixel[0] + 59 * srcPixel[1] + 11 * srcPixel[2] ) / ( 100 * 256 ) );
			srcPixel += 4;
		}
	}
} // ExAPBitmapGrey8::ConvertFromRGBA64()


// **** End of File ****
