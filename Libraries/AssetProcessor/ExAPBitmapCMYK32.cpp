/*
 *  ExAPBitmapCMYK32.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Sun Jun 22 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#include "General\\ExAPMinMax.h"
#include "ExAPBitmapCMYK32.h"
#include "ExAPConversionTemplates.h"


// Constructor.

ExAPBitmapCMYK32::ExAPBitmapCMYK32( int width, int height ) throw( ExAPException )
	: ExAPBitmap8bpchNoAlpha( ExAPColourModel::CMYK32, width, height )
{
}
	

// Destructor.

ExAPBitmapCMYK32::~ExAPBitmapCMYK32( void )
{
}


ExAPAutoPtr<ExAPBitmap> ExAPBitmapCMYK32::CreateCompatibleBitmap( int width, int height ) const throw( ExAPException )
{
#ifdef EXAP_USE_CREATEIMAGE_FUNCTION_TEMPLATE
	return( CreateImage<ExAPBitmapCMYK32,ExAPBitmap>( width, height ) );
#else
	ExAPAutoPtr<ExAPBitmap> image;	// Initialized to zero.

	try
	{
		image = new ExAPBitmapCMYK32( width, height );
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
} // ExAPBitmapCMYK32::CreateCompatibleBitmap()


void ExAPBitmapCMYK32::ConvertToRGBA64( unsigned short * dstRow, int dstSamplesPerRow,
	const ExAPRect & srcRect ) const
{
	const unsigned char * srcRow = m_buffer + srcRect.getTop() * m_bytesPerLine + srcRect.getLeft() * 4;
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
			const int base = 255 - static_cast<int>( srcPixel[3] );

			dstPixel[0] = static_cast<unsigned short>( 257 * max<int>( 0, base - static_cast<int>( srcPixel[0] ) ) );
			dstPixel[1] = static_cast<unsigned short>( 257 * max<int>( 0, base - static_cast<int>( srcPixel[1] ) ) );
			dstPixel[2] = static_cast<unsigned short>( 257 * max<int>( 0, base - static_cast<int>( srcPixel[2] ) ) );
			dstPixel[3] = 65535;
			dstPixel += 4;
			srcPixel += 4;
		}
	}
}


void ExAPBitmapCMYK32::ConvertFromRGBA64( const unsigned short * srcRow, int srcSamplesPerRow,
	const ExAPRect & dstRect ) const
{
	unsigned char * dstRow = m_buffer + dstRect.getTop() * m_bytesPerLine + dstRect.getLeft() * 4;
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
			// TAW TO_DO: Implement RGB to CMYK conversion here.
			const int cyan = ( 65535 - static_cast<int>( srcPixel[0] ) ) / 256;
			const int magenta = ( 65535 - static_cast<int>( srcPixel[1] ) ) / 256;
			const int yellow = ( 65535 - static_cast<int>( srcPixel[2] ) ) / 256;
			int black = cyan;

			if( black > magenta )
			{
				black = magenta;
			}

			if( black > yellow )
			{
				black = yellow;
			}

			dstPixel[0] = static_cast<unsigned char>( cyan - black );
			dstPixel[1] = static_cast<unsigned char>( magenta - black );
			dstPixel[2] = static_cast<unsigned char>( yellow - black );
			dstPixel[3] = static_cast<unsigned char>( black );
			dstPixel += 4;
			srcPixel += 4;
		}
	}
}


// **** End of File ****
