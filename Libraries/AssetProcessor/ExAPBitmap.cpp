/*
 *  ExAPBitmap.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Wed Apr 30 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#include "ExAPBitmapRGB24.h"
#include "ExAPBitmapRGBA64.h"
#include "ExAPBitmapPaletted.h"
#include "General\\ExAPMinMax.h"


// Constructors.

ExAPBitmap::ExAPBitmap( ExAPColourModel::ModelType colourModelType, int bitsPerPixel, int width, int height, int bytesPerLineAlignment ) throw( ExAPException )
: m_colourModel( colourModelType ),
	  m_width( width ),
	  m_height( height ),
	  m_bitsPerPixel( bitsPerPixel ),
	  m_bytesPerLineAlignment( ( bytesPerLineAlignment > 0 ) ? bytesPerLineAlignment : 4 ),
	  m_bytesPerLine( ComputeBytesPerLine( bitsPerPixel, width, m_bytesPerLineAlignment ) )
{

	if( m_width <= 0  ||  m_height <= 0 )
	{
		ThrowExAPException( ExAPException::IllegalParameterValue );
	}
}

	
// Destructor.

ExAPBitmap::~ExAPBitmap( void )
{
}


int ExAPBitmap::ComputeBytesPerLine( int bitsPerPixel, int width, int bytesPerLineAlignment )
{
	// Ensure that bytesPerLine is aligned as specified.

	//if( bytesPerLineAlignment <= 0  ||  ( bytesPerLineAlignment & ( bytesPerLineAlignment - 1 ) ) > 0 )	// Power of 2 test.
	if( !IsPowerOf2( bytesPerLineAlignment ) )
	{
		ThrowExAPException( ExAPException::IllegalParameterValue );
	}

	int bytesPerLine = ( bitsPerPixel * width + 7 ) / 8;

	bytesPerLine += bytesPerLineAlignment - 1;
	bytesPerLine &= ~( bytesPerLineAlignment - 1 );
	return( bytesPerLine );
}


bool ExAPBitmap::IsPowerOf2( int x )
{
	return( x > 0  &&  ( x & ( x - 1 ) ) == 0 );
}


void ExAPBitmap::ConvertColourModel( const ExAPPoint & dstPoint,
	const ExAPBitmap & srcBitmap, const ExAPPoint & srcPoint,
	const ExAPDimensions & dims ) const throw( ExAPException )
{
	static const int desiredIntermediateChunkSize = 200;
	bool bChunkable = true;
	const ExAPBitmapPaletted * dstPalettedBitmap = dynamic_cast<const ExAPBitmapPaletted *>( this );

	if( dstPalettedBitmap != 0 )
	{
		bChunkable = dstPalettedBitmap->getPalette().m_pDitherer->Chunkable();
	}

	const int nChunkWidth = bChunkable ? desiredIntermediateChunkSize : dims.getWidth();
	const int nChunkHeight = bChunkable ? desiredIntermediateChunkSize : dims.getHeight();
	ExAPBitmapRGBA64 intermediateBitmap( nChunkWidth, nChunkHeight );
	const ExAPPoint dstToSrcOffset = srcPoint - dstPoint;
	const ExAPPoint srcToDstOffset = dstPoint - srcPoint;
	ExAPRect dstRect( dstPoint, dims );
#if 1
	const int dstWidth = GetWidth();
	const int dstHeight = GetHeight();
	const ExAPRect bound = getBoundingRect();
#endif

	(void)dstRect.intersect( getBoundingRect() );

	ExAPRect srcRect( dstRect );

	srcRect += dstToSrcOffset;
	(void)srcRect.intersect( srcBitmap.getBoundingRect() );

	if( srcRect.isEmpty() )
	{
		return;
	}

	for( int srcChunkY = srcRect.getTop(); srcChunkY < srcRect.getBottom(); srcChunkY += nChunkHeight )
	{
		const int srcChunkHeight = min( srcRect.getBottom() - srcChunkY, nChunkHeight );

		for( int srcChunkX = srcRect.getLeft(); srcChunkX < srcRect.getRight(); srcChunkX += nChunkWidth )
		{
			const int srcChunkWidth = min( srcRect.getRight() - srcChunkX, nChunkWidth );
			ExAPRect srcChunkRect( srcChunkX, srcChunkY, srcChunkWidth, srcChunkHeight );
			unsigned short * intermediateDataPtr = intermediateBitmap.GetBuffer();

			srcBitmap.ConvertToRGBA64( intermediateDataPtr, intermediateBitmap.GetSamplesPerLine(), srcChunkRect );
			srcChunkRect += srcToDstOffset;
			ConvertFromRGBA64( intermediateDataPtr, intermediateBitmap.GetSamplesPerLine(), srcChunkRect );
		}
	}
} // ExAPBitmap::ConvertColourModel()


void ExAPBitmap::TestDisplay( void ) const throw( ExAPException )
{
	ExAPBitmapRGB24 tempBitmap( m_width, m_height );
	const ExAPPoint origin( 0, 0 );
	const ExAPDimensions dims( m_width, m_height );

	tempBitmap.ConvertColourModel( origin, *this, origin, dims );
	tempBitmap.TestDisplay();
} // ExAPBitmap::TestDisplay()


void ExAPBitmap::ExportToGIF( const std::string & dstFilename ) const
{
	ExAPBitmapRGB24 tempBitmap( m_width, m_height );
	const ExAPPoint origin( 0, 0 );
	const ExAPDimensions dims( m_width, m_height );

	tempBitmap.ConvertColourModel( origin, *this, origin, dims );
	tempBitmap.ExportToGIF( dstFilename );
} // ExAPBitmap::ExportToGIF()


// **** End of File ****
