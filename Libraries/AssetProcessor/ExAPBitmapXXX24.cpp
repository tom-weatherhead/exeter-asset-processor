/*
 *  ExAPBitmapXXX24.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Thu May 01 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#include "ExAPBitmapXXX24.h"
#include "ExAPBitmapXXXA32.h"


// Constructor.

ExAPBitmapXXX24::ExAPBitmapXXX24( ExAPColourModel::ModelType colourModelType, int width, int height ) throw( ExAPException )
	: ExAPBitmap8bpchNoAlpha( colourModelType, width, height )
{
}


// Destructor.

ExAPBitmapXXX24::~ExAPBitmapXXX24( void )
{
}


void ExAPBitmapXXX24::Blit( int dstX, int dstY, const ExAPBitmapXXX24 & srcImage, const ExAPRect & srcRectParam )
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

	unsigned char * dstRow = m_buffer + dstRect.getTop() * m_bytesPerLine + dstRect.getLeft() * 3;
	const unsigned char * srcRow = srcImage.m_buffer + srcRect.getTop() * srcImage.m_bytesPerLine + srcRect.getLeft() * 3;

	ExAPBitmap8bpch::Blit(
		dstRow, m_bytesPerLine,
		srcRow, srcImage.m_bytesPerLine,
		3, dstRect.getWidth(), dstRect.getHeight() );
} // ExAPBitmapXXX24::Blit()


void ExAPBitmapXXX24::Composite( int dstX, int dstY, const ExAPBitmapXXXA32 & srcImage, const ExAPRect & srcRectParam )
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

	unsigned char * dstRow = m_buffer + dstRect.getTop() * m_bytesPerLine + dstRect.getLeft() * 3;
	const unsigned char * srcRow = srcImage.getBuffer() + srcRect.getTop() * srcImage.getBytesPerLine() + srcRect.getLeft() * 4;
	
	for( int nRows = dstRect.getHeight(); nRows-- > 0; )
	{
		unsigned char * dstPixel = dstRow;
		const unsigned char * srcPixel = srcRow;
		
		for( int nCols = dstRect.getWidth(); nCols-- > 0; )
		{
			const unsigned int alpha = static_cast<unsigned int>( srcPixel[3] );
			const unsigned int inverseAlpha = 255 - alpha;
			
			// Here's a good place for SIMD optimization.
			dstPixel[0] = (unsigned char)( ( srcPixel[0] * alpha + dstPixel[0] * inverseAlpha + 127 ) / 255 );
			dstPixel[1] = (unsigned char)( ( srcPixel[1] * alpha + dstPixel[1] * inverseAlpha + 127 ) / 255 );
			dstPixel[2] = (unsigned char)( ( srcPixel[2] * alpha + dstPixel[2] * inverseAlpha + 127 ) / 255 );
			dstPixel += 3;
			srcPixel += 4;
		}
		
		dstRow += m_bytesPerLine;
		srcRow += srcImage.getBytesPerLine();
	}
} // ExAPBitmapXXX24::Composite()


void ExAPBitmapXXX24::BilinearDownsampleRow(
	int dstPixelsToRender,
	unsigned char * dst, int bytesPerDstPixel,
	int dstCoord, int dstContextStart, int dstContextSize,
	const unsigned char * src, int bytesPerSrcPixel,
	int srcCoord, int srcContextStart, int srcContextSize )
{
	// In a downsample, dstContextSize < srcContextSize.
	// TO_DO: This could be made faster by sampling just 2 src pixels per dst pixel; perhaps by calling a modified version of the upsample function.
	const int product1 = ( dstCoord - dstContextStart ) * srcContextSize;
	const int srcCoord2 = product1 / dstContextSize + srcContextStart;
	int firstSrcPixelWeight = dstContextSize - product1 % dstContextSize;
	const int halfSrcContextSize = srcContextSize / 2;	// For rounding.

	src += ( srcCoord2 - srcCoord ) * bytesPerSrcPixel;
	
	while( dstPixelsToRender-- > 0 )
	{
		unsigned int accumulator[3];
		int totalWeightRemaining = srcContextSize - firstSrcPixelWeight;

		accumulator[0] = (unsigned int)( src[0] * firstSrcPixelWeight + halfSrcContextSize );
		accumulator[1] = (unsigned int)( src[1] * firstSrcPixelWeight + halfSrcContextSize );
		accumulator[2] = (unsigned int)( src[2] * firstSrcPixelWeight + halfSrcContextSize );
		src += bytesPerSrcPixel;
		
		while( totalWeightRemaining > dstContextSize )
		{
			accumulator[0] += (unsigned int)( src[0] * dstContextSize );
			accumulator[1] += (unsigned int)( src[1] * dstContextSize );
			accumulator[2] += (unsigned int)( src[2] * dstContextSize );
			src += bytesPerSrcPixel;
			totalWeightRemaining -= dstContextSize;
		}

		accumulator[0] += (unsigned int)( src[0] * totalWeightRemaining );
		accumulator[1] += (unsigned int)( src[1] * totalWeightRemaining );
		accumulator[2] += (unsigned int)( src[2] * totalWeightRemaining );
		firstSrcPixelWeight = dstContextSize - totalWeightRemaining;	// This may be zero.
		dst[0] = (unsigned char)( accumulator[0] / (unsigned int)srcContextSize );
		dst[1] = (unsigned char)( accumulator[1] / (unsigned int)srcContextSize );
		dst[2] = (unsigned char)( accumulator[2] / (unsigned int)srcContextSize );
		dst += bytesPerDstPixel;
	}
} // ExAPBitmapXXX24::BilinearDownsampleRow()


void ExAPBitmapXXX24::BilinearUpsampleRow(
	int dstPixelsToRender,
	unsigned char * dst, int bytesPerDstPixel,
	int dstCoord, int dstContextStart, int dstContextSize,
	const unsigned char * src, int bytesPerSrcPixel,
	int srcCoord, int srcContextStart, int srcContextSize )
{
	// In an upsample, dstContextSize > srcContextSize.
	const int srcContextEnd = srcContextStart + srcContextSize - 1;
	const int product1 = ( dstCoord - dstContextStart ) * srcContextSize + ( srcContextSize - dstContextSize ) / 2;
	const unsigned char * srcPixel = 0;
	const unsigned char * nextSrcPixel = 0;
	int nextSrcPixelCoord = 0;
	int nextSrcPixelWeight = 0;
	
	if( product1 < 0 )
	{
		// Both srcPixel and nextSrcPixel are in the near end.
		srcPixel = src + ( srcContextStart - srcCoord ) * bytesPerSrcPixel;
		nextSrcPixel = srcPixel;
		nextSrcPixelCoord = srcContextStart;
		nextSrcPixelWeight = product1 + dstContextSize;
	}
	else if( product1 >= ( srcContextSize - 1 ) * dstContextSize )
	{
		// Both srcPixel and nextSrcPixel are in the far end.
		srcPixel = src + ( srcContextEnd - srcCoord ) * bytesPerSrcPixel;
		nextSrcPixel = srcPixel;
		nextSrcPixelCoord = srcContextEnd;
		nextSrcPixelWeight = 0;	// The weight is irrelevant in the far end zone.
	}
	else
	{
		const int srcPixelCoord = product1 / dstContextSize + srcContextStart;
		
		srcPixel = src + ( srcPixelCoord - srcCoord ) * bytesPerSrcPixel;
		nextSrcPixel = srcPixel + bytesPerSrcPixel;
		nextSrcPixelCoord = srcPixelCoord + 1;
		nextSrcPixelWeight = product1 % dstContextSize;
	}
	
	while( dstPixelsToRender-- > 0 )
	{
		const int srcPixelWeight = dstContextSize - nextSrcPixelWeight;
		
		dst[0] = (unsigned char)( ( srcPixel[0] * srcPixelWeight + nextSrcPixel[0] * nextSrcPixelWeight ) / dstContextSize );
		dst[1] = (unsigned char)( ( srcPixel[1] * srcPixelWeight + nextSrcPixel[1] * nextSrcPixelWeight ) / dstContextSize );
		dst[2] = (unsigned char)( ( srcPixel[2] * srcPixelWeight + nextSrcPixel[2] * nextSrcPixelWeight ) / dstContextSize );
		dst += bytesPerDstPixel;
		nextSrcPixelWeight += srcContextSize;
		
		if( nextSrcPixelWeight >= dstContextSize )
		{
			// Move the src pixel pointers.
			nextSrcPixelWeight -= dstContextSize;
			srcPixel = nextSrcPixel;
			
			if( ++nextSrcPixelCoord < srcContextEnd )
			{
				nextSrcPixel += bytesPerSrcPixel;
			}
		}
	}
} // ExAPBitmapXXX24::BilinearUpsampleRow()


//lint -e715 : Symbols not referenced.

ExAPRect ExAPBitmapXXX24::GetBilinearSrcRect(
	const ExAPRect & dstRect, const ExAPRect & dstContextRect, const ExAPRect & srcContextRect )
{
	// TO_DO: Fix this so that the minimal src rect is returned.
	return( srcContextRect );
}

//lint +e715


void ExAPBitmapXXX24::BilinearResample(
	const ExAPRect & dstRect, const ExAPRect & dstContextRect,
	const ExAPBitmapXXX24 & srcImage, const ExAPRect & srcContextRect ) throw( ExAPException )
{
	ExAPRect srcRect = ExAPBitmapXXX24::GetBilinearSrcRect( dstRect, dstContextRect, srcContextRect );
	ExAPAutoPtr<ExAPBitmap> tempImageBase = CreateCompatibleBitmap( dstRect.getWidth(), srcRect.getHeight() );
	ExAPBitmap * tempImageBaseRaw = tempImageBase;
	ExAPBitmapXXX24 * tempImage = dynamic_cast<ExAPBitmapXXX24 *>( tempImageBaseRaw );

	if( tempImage == 0 )
	{
		ThrowExAPException( ExAPException::DynamicCastFailed );
	}

	// Horizontal resampling.
	
	if( dstContextRect.getWidth() < srcContextRect.getWidth() )
	{
		// Horizontal downsampling.
		unsigned char * tempRow = tempImage->m_buffer;
		const unsigned char * srcRow = srcImage.m_buffer + srcRect.getTop() * srcImage.m_bytesPerLine;
		
		for( int numRows = srcRect.getHeight(); numRows > 0; numRows-- )
		{
			BilinearDownsampleRow( dstRect.getWidth(),
				tempRow, 3, dstRect.getLeft(), dstContextRect.getLeft() - dstRect.getLeft(), dstContextRect.getWidth(),
				srcRow, 3, 0, srcContextRect.getLeft(), srcContextRect.getWidth() );
			tempRow += tempImage->m_bytesPerLine;
			srcRow += srcImage.m_bytesPerLine;
		}
	}
	else if( dstContextRect.getWidth() > srcContextRect.getWidth() )
	{
		// Horizontal upsampling.
		unsigned char * tempRow = tempImage->m_buffer;
		const unsigned char * srcRow = srcImage.m_buffer + srcRect.getTop() * srcImage.m_bytesPerLine;
		
		for( int numRows = srcRect.getHeight(); numRows > 0; numRows-- )
		{
			BilinearUpsampleRow( dstRect.getWidth(),
				tempRow, 3, dstRect.getLeft(), dstContextRect.getLeft() - dstRect.getLeft(), dstContextRect.getWidth(),
				srcRow, 3, 0, srcContextRect.getLeft(), srcContextRect.getWidth() );
			tempRow += tempImage->m_bytesPerLine;
			srcRow += srcImage.m_bytesPerLine;
		}
	}
	else
	{
		// Blit.
		tempImage->Blit( 0, 0, srcImage, srcRect );
	}
	
	// Vertical resampling.
	
	if( dstContextRect.getHeight() < srcContextRect.getHeight() )
	{
		// Vertical downsampling.
		unsigned char * dstRow = m_buffer + dstRect.getTop() * m_bytesPerLine + dstRect.getLeft() * 3;
		const unsigned char * tempRow = tempImage->m_buffer;
		
		for( int numCols = dstRect.getWidth(); numCols > 0; numCols-- )
		{
			BilinearDownsampleRow( dstRect.getHeight(),
				dstRow, m_bytesPerLine, dstRect.getTop(), dstContextRect.getTop() - dstRect.getTop(), dstContextRect.getHeight(),
				tempRow, tempImage->m_bytesPerLine, srcRect.getTop(), srcContextRect.getTop() - srcRect.getTop(), srcContextRect.getHeight() );
			dstRow += 3;
			tempRow += 3;
		}
	}
	else if( dstContextRect.getHeight() > srcContextRect.getHeight() )
	{
		// Vertical upsampling.
		unsigned char * dstRow = m_buffer + dstRect.getTop() * m_bytesPerLine + dstRect.getLeft() * 3;
		const unsigned char * tempRow = tempImage->m_buffer;
		
		for( int numCols = dstRect.getWidth(); numCols > 0; numCols-- )
		{
			BilinearUpsampleRow( dstRect.getHeight(),
				dstRow, m_bytesPerLine, dstRect.getTop(), dstContextRect.getTop() - dstRect.getTop(), dstContextRect.getHeight(),
				tempRow, tempImage->m_bytesPerLine, srcRect.getTop(), srcContextRect.getTop() - srcRect.getTop(), srcContextRect.getHeight() );
			dstRow += 3;
			tempRow += 3;
		}
	}
	else
	{
		// Blit.
		Blit( dstRect.getLeft(), dstRect.getTop(), *tempImage, tempImage->getBoundingRect() );
	}
} // ExAPBitmapXXX24::BilinearResample()


// **** End of File ****
