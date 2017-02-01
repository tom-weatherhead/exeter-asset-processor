/*
 *  ExAPBitmapRGBA32.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Wed May 07 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#include "ExAPBitmapRGBA32.h"
#include "ExAPConversionTemplates.h"


ExAPBitmapRGBA32::ExAPBitmapRGBA32( int width, int height ) throw( ExAPException )
	: ExAPBitmapXXXA32( ExAPColourModel::RGBA32, width, height )
{
}


ExAPBitmapRGBA32::~ExAPBitmapRGBA32( void )
{
}


#ifdef MACINTOSH
void ExAPBitmapRGBA32::DrawIntoQuartzContext( CGContextRef context ) const
{
	void * pBits = (void *)m_buffer;
	const size_t width = (size_t)m_width;
	const size_t height = (size_t)m_height;
	const size_t bytesPerRow = (size_t)m_bytesPerLine;
	const size_t size = bytesPerRow * height;
	CGRect rectangle;
	CGDataProviderRef provider;
	CGColorSpaceRef colorspace;
	CGImageRef image;

	/* Create a data provider with a pointer to the memory bits */
	provider = CGDataProviderCreateWithData( NULL, pBits, size, NULL );

	/* Colorspace can be device, calibrated, or ICC profile based */
	colorspace = CGColorSpaceCreateDeviceRGB();

	/* Create the image */
	image = CGImageCreate( width, height, 8 /* bitsPerComponent */,
		32 /* bitsPerPixel */,
		bytesPerRow, colorspace,
		kCGImageAlphaFirst, provider, NULL, 0,
		kCGRenderingIntentDefault );

	/* Once the image is created we can release our reference to the
	** provider and the colorspace.  They will be retained by the image.
	*/
	CGDataProviderRelease( provider );
	CGColorSpaceRelease( colorspace );

	/* Determine the location where the image will be drawn in userspace. */
	rectangle = CGRectMake( 0, 0, width, height );

	/* Draw the image to the Core Graphics context. */
	CGContextDrawImage( context, rectangle, image );

	CGImageRelease( image );
}


void ExAPBitmapRGBA32::DrawIntoMacWindow( WindowRef window ) const
{
	// Acquire a display context.
	CGContextRef context;
	
	// Get the window's port, and use it to create the CGContext.
	/* OSStatus stat = */ CreateCGContextForPort( GetWindowPort( window ), &context );

	// Display the image to the device using the display context.
	DrawIntoQuartzContext( context );

	CGContextFlush( context );

	// Release all acquired resources.
	CGContextRelease( context );
}


void ExAPBitmapRGBA32::PixelByteSwap( void ) const
{
	unsigned char * pRow = m_buffer;
	
	for( int y = 0; y < m_height; ++y )
	{
		unsigned char * pPixel = pRow;
		
		pRow += m_bytesPerLine;
		
		for( int x = 0; x < m_width; ++x )
		{
			// Ideal for AltiVec optimization.
			unsigned char temp = pPixel[0];
			
			pPixel[0] = pPixel[3];
			pPixel[3] = temp;
			temp = pPixel[1];
			pPixel[1] = pPixel[2];
			pPixel[2] = temp;
			pPixel += 4;
		}
	}
}
#endif // MACINTOSH


ExAPAutoPtr<ExAPBitmap> ExAPBitmapRGBA32::CreateCompatibleBitmap( int width, int height ) const throw( ExAPException )
{
#ifdef EXAP_USE_CREATEIMAGE_FUNCTION_TEMPLATE
	return( CreateImage<ExAPBitmapRGBA32,ExAPBitmap>( width, height ) );
#else
	ExAPAutoPtr<ExAPBitmap> image;	// Initialized to zero.

	try
	{
		image = new ExAPBitmapRGBA32( width, height );
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
} // ExAPBitmapRGBA32::CreateCompatibleBitmap()


void ExAPBitmapRGBA32::ConvertToRGBA64( unsigned short * dstRow, int dstSamplesPerRow,
	const ExAPRect & srcRect ) const
{
	// dstRow is the upper-left corner of the dst image chunk.
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
			dstPixel[0] = (unsigned short)( 257 * srcPixel[0] );
			dstPixel[1] = (unsigned short)( 257 * srcPixel[1] );
			dstPixel[2] = (unsigned short)( 257 * srcPixel[2] );
			dstPixel[3] = (unsigned short)( 257 * srcPixel[3] );
			srcPixel += 4;
			dstPixel += 4;
		}
	}
} // ExAPBitmapRGBA32::ConvertToRGBA64()


void ExAPBitmapRGBA32::ConvertFromRGBA64( const unsigned short * srcRow, int srcSamplesPerRow,
	const ExAPRect & dstRect ) const
{
	// srcRow is the upper-left corner of the src image chunk.
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
			dstPixel[0] = (unsigned char)( srcPixel[0] / 256 );
			dstPixel[1] = (unsigned char)( srcPixel[1] / 256 );
			dstPixel[2] = (unsigned char)( srcPixel[2] / 256 );
			dstPixel[3] = (unsigned char)( srcPixel[3] / 256 );
			srcPixel += 4;
			dstPixel += 4;
		}
	}
} // ExAPBitmapRGBA32::ConvertFromRGBA64()


// **** End of File ****
