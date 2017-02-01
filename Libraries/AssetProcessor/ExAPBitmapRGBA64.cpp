/*
 *  ExAPBitmapRGBA64.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Tue Jul 15 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#include "ExAPBitmapRGBA64.h"
#include "ExAPConversionTemplates.h"


// Constructor.

ExAPBitmapRGBA64::ExAPBitmapRGBA64( int width, int height ) throw( ExAPException )
	: ExAPBitmap16bpch( ExAPColourModel::RGBA64, width, height )
{
}
	

// Destructor.

ExAPBitmapRGBA64::~ExAPBitmapRGBA64( void )
{
}


ExAPAutoPtr<ExAPBitmap> ExAPBitmapRGBA64::CreateCompatibleBitmap( int width, int height ) const throw( ExAPException )
{
#ifdef EXAP_USE_CREATEIMAGE_FUNCTION_TEMPLATE
	return( CreateImage<ExAPBitmapRGBA64,ExAPBitmap>( width, height ) );
#else
	ExAPAutoPtr<ExAPBitmap> image;	// Initialized to zero.

	try
	{
		image = new ExAPBitmapRGBA64( width, height );
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
} // ExAPBitmapRGBA64::CreateCompatibleBitmap()


void ExAPBitmapRGBA64::ConvertToRGBA64(
	unsigned short * dstRow,
	int dstSamplesPerRow,
	const ExAPRect & srcRect ) const
{
	const int srcSamplesPerRow = GetSamplesPerLine();
	const unsigned short * srcRow = GetBuffer()
		+ srcRect.getTop() * srcSamplesPerRow
		+ srcRect.getLeft() * m_samplesPerPixel;

	ExAPBitmap16bpch::Blit( dstRow, dstSamplesPerRow, srcRow, srcSamplesPerRow,
		m_samplesPerPixel, srcRect.getWidth(), srcRect.getHeight() );
} // ExAPBitmapRGBA64::ConvertToRGBA64()


void ExAPBitmapRGBA64::ConvertFromRGBA64(
	const unsigned short * srcRow,
	int srcSamplesPerRow,
	const ExAPRect & dstRect ) const
{
	const int dstSamplesPerRow = GetSamplesPerLine();
	unsigned short * dstRow = GetBuffer()
		+ dstRect.getTop() * dstSamplesPerRow
		+ dstRect.getLeft() * m_samplesPerPixel;

	ExAPBitmap16bpch::Blit( dstRow, dstSamplesPerRow, srcRow, srcSamplesPerRow,
		m_samplesPerPixel, dstRect.getWidth(), dstRect.getHeight() );
} // ExAPBitmapRGBA64::ConvertFromRGBA64()


// **** End of File ****
