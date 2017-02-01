/*
 *  ExAPBitmapXXXA32.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Fri May 02 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#include "ExAPBitmapXXXA32.h"
#include "ExAPBitmapXXX24.h"


ExAPBitmapXXXA32::ExAPBitmapXXXA32( ExAPColourModel::ModelType colourModelType, int width, int height ) throw( ExAPException )
	: ExAPBitmap8bpchWithAlpha( colourModelType, width, height )
{
}


// Destructor.

ExAPBitmapXXXA32::~ExAPBitmapXXXA32( void )
{
}


void ExAPBitmapXXXA32::Blit( int dstX, int dstY, const ExAPBitmapXXX24 & srcImage, const ExAPRect & srcRectParam )
{
	const int srcToDstX = dstX - srcRectParam.getLeft();
	const int srcToDstY = dstY - srcRectParam.getTop();
	ExAPRect srcRect( srcRectParam );
	
	(void)srcRect.intersect( srcImage.getBoundingRect() );	// rect &= rect
	(void)srcRect.offset( srcToDstX, srcToDstY );				// rect += vector
	(void)srcRect.intersect( getBoundingRect() );				// rect &= rect
	
	const ExAPRect dstRect( srcRect );
	
	(void)srcRect.offset( -srcToDstX, -srcToDstY );			// rect -= vector
	
	if( srcRect.isEmpty() )
	{
		return;
	}

	unsigned char * dstRow = m_buffer + dstRect.getTop() * m_bytesPerLine + dstRect.getLeft() * 4;
	const unsigned char * srcRow = srcImage.getBuffer() + srcRect.getTop() * srcImage.getBytesPerLine() + srcRect.getLeft() * 3;
	
	for( int nRows = dstRect.getHeight(); nRows-- > 0; )
	{
		unsigned char * dstPixel = dstRow;
		const unsigned char * srcPixel = srcRow;
		
		for( int nCols = dstRect.getWidth(); nCols-- > 0; )
		{
			// Here's a good place for SIMD optimization.
			dstPixel[0] = srcPixel[0];
			dstPixel[1] = srcPixel[1];
			dstPixel[2] = srcPixel[2];
			dstPixel[3] = 255;
			dstPixel += 4;
			srcPixel += 3;
		}
		
		dstRow += m_bytesPerLine;
		srcRow += srcImage.getBytesPerLine();
	}
} // ExAPBitmapXXXA32::Blit()


// Straight copy of pixel data, including alpha.  No compositing.

void ExAPBitmapXXXA32::Blit( int dstX, int dstY, const ExAPBitmapXXXA32 & srcImage, const ExAPRect & srcRectParam )
{
	const int srcToDstX = dstX - srcRectParam.getLeft();
	const int srcToDstY = dstY - srcRectParam.getTop();
	ExAPRect srcRect( srcRectParam );
	
	(void)srcRect.intersect( srcImage.getBoundingRect() );	// rect &= rect
	(void)srcRect.offset( srcToDstX, srcToDstY );						// rect += vector
	(void)srcRect.intersect( getBoundingRect() );						// rect &= rect
	
	const ExAPRect dstRect( srcRect );
	
	(void)srcRect.offset( -srcToDstX, -srcToDstY );					// rect -= vector
	
	if( srcRect.isEmpty() )
	{
		return;
	}

	unsigned char * dstRow = m_buffer + dstRect.getTop() * m_bytesPerLine + dstRect.getLeft() * 4;
	const unsigned char * srcRow = srcImage.m_buffer + srcRect.getTop() * srcImage.m_bytesPerLine + srcRect.getLeft() * 4;

	ExAPBitmap8bpch::Blit(
		dstRow, m_bytesPerLine,
		srcRow, srcImage.m_bytesPerLine,
		4, dstRect.getWidth(), dstRect.getHeight() );
}


void ExAPBitmapXXXA32::Composite( int dstX, int dstY, const ExAPBitmapXXXA32 & srcImage, const ExAPRect & srcRectParam )
{
	const int srcToDstX = dstX - srcRectParam.getLeft();
	const int srcToDstY = dstY - srcRectParam.getTop();
	ExAPRect srcRect( srcRectParam );
	
	(void)srcRect.intersect( srcImage.getBoundingRect() );	// rect &= rect
	(void)srcRect.offset( srcToDstX, srcToDstY );				// rect += vector
	(void)srcRect.intersect( getBoundingRect() );				// rect &= rect
	
	const ExAPRect dstRect( srcRect );
	
	(void)srcRect.offset( -srcToDstX, -srcToDstY );			// rect -= vector
	
	if( srcRect.isEmpty() )
	{
		return;
	}

	unsigned char * dstRow = m_buffer + dstRect.getTop() * m_bytesPerLine + dstRect.getLeft() * 4;
	const unsigned char * srcRow = srcImage.m_buffer + srcRect.getTop() * srcImage.m_bytesPerLine + srcRect.getLeft() * 4;
	
	for( int nRows = dstRect.getHeight(); nRows-- > 0; )
	{
		unsigned char * dstPixel = dstRow;
		const unsigned char * srcPixel = srcRow;
		
		for( int nCols = dstRect.getWidth(); nCols-- > 0; )
		{
			const unsigned int srcAlpha = static_cast<unsigned int>( srcPixel[3] );
			const unsigned int dstAlpha = static_cast<unsigned int>( dstPixel[3] );
			// Porter-Duff operation: Over: a + ( 1 - a ) * b.
			const unsigned int srcWeight = 255 * srcAlpha;
			const unsigned int dstWeight = ( 255 - srcAlpha ) * dstAlpha;
			const unsigned int totalWeight = srcWeight + dstWeight;
			const unsigned char newAlpha = static_cast<unsigned char>( totalWeight / 255 );
			
			dstPixel[3] = newAlpha;
			
			if( newAlpha > 0 )
			{
				const unsigned int halfTotalWeight = totalWeight / 2;	// For rounding.

				// Here's a good place for SIMD optimization.
				dstPixel[0] = (unsigned char)( ( srcPixel[0] * srcWeight + dstPixel[0] * dstWeight + halfTotalWeight ) / totalWeight );
				dstPixel[1] = (unsigned char)( ( srcPixel[1] * srcWeight + dstPixel[1] * dstWeight + halfTotalWeight ) / totalWeight );
				dstPixel[2] = (unsigned char)( ( srcPixel[2] * srcWeight + dstPixel[2] * dstWeight + halfTotalWeight ) / totalWeight );
			}

			dstPixel += 4;
			srcPixel += 4;
		}
		
		dstRow += m_bytesPerLine;
		srcRow += srcImage.m_bytesPerLine;
	}
} // ExAPBitmapXXXA32::Composite()


// **** End of File ****
