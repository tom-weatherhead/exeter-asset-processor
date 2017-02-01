/*
 *  ExAPBitmapXXX24.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Thu May 01 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#ifndef _EXAPBITMAPXXX24_H_
#define _EXAPBITMAPXXX24_H_

#include "ExAPBitmap8bpchNoAlpha.h"


class ExAPBitmapXXXA32;


// 24-bit RGB or 24-bit LAB.

class ExAPBitmapXXX24 : public ExAPBitmap8bpchNoAlpha
{
#if 0
protected:

	static void BilinearDownsampleRow(
		int dstPixelsToRender,
		unsigned char * dst, int bytesPerDstPixel,
		int dstCoord, int dstContextStart, int dstContextSize,
		unsigned char * src, int bytesPerSrcPixel,
		int srcCoord, int srcContextStart, int srcContextSize );

	static void BilinearUpsampleRow(
		int dstPixelsToRender,
		unsigned char * dst, int bytesPerDstPixel,
		int dstCoord, int dstContextStart, int dstContextSize,
		const unsigned char * src, int bytesPerSrcPixel,
		int srcCoord, int srcContextStart, int srcContextSize );
#endif

public:
	// Constructor.
	ExAPBitmapXXX24( ExAPColourModel::ModelType colourModelType, int width, int height ) throw( ExAPException );
	
	// Destructor.
	virtual ~ExAPBitmapXXX24( void );
	
	// Operations.

	// TO_DO: Make these functions protected to avoid inappropriate user calls?

	void Blit( int dstX, int dstY, const ExAPBitmapXXX24 & srcImage, const ExAPRect & srcRectParam );
	void Composite( int dstX, int dstY, const ExAPBitmapXXXA32 & srcImage, const ExAPRect & srcRectParam );

	static void BilinearDownsampleRow(
		int dstPixelsToRender,
		unsigned char * dst, int bytesPerDstPixel,
		int dstCoord, int dstContextStart, int dstContextSize,
		const unsigned char * src, int bytesPerSrcPixel,
		int srcCoord, int srcContextStart, int srcContextSize );

	static void BilinearUpsampleRow(
		int dstPixelsToRender,
		unsigned char * dst, int bytesPerDstPixel,
		int dstCoord, int dstContextStart, int dstContextSize,
		const unsigned char * src, int bytesPerSrcPixel,
		int srcCoord, int srcContextStart, int srcContextSize );

	static ExAPRect GetBilinearSrcRect( const ExAPRect & dstRect, const ExAPRect & dstContextRect, const ExAPRect & srcContextRect );

	void BilinearResample(
		const ExAPRect & dstRect, const ExAPRect & dstContextRect,
		const ExAPBitmapXXX24 & srcImage, const ExAPRect & srcContextRect ) throw( ExAPException );
		
	inline void BilinearResample( const ExAPRect & dstRect, const ExAPBitmapXXX24 & srcImage, const ExAPRect & srcRect ) throw( ExAPException )
	{
		BilinearResample( dstRect, dstRect, srcImage, srcRect );
	}

	//virtual void BilinearResample( const ExAPRect & dstRect, const ExAPBitmap & src, const ExAPRect & srcRect );
	
	//ExAPBitmapXXX24 * CreateBilinearResampledImage( const ExAPRect & dstRect, const ExAPRect & srcRect ) const;
}; // class ExAPBitmapXXX24


#endif //_EXAPBITMAPXXX24_H_


// **** End of File ****
