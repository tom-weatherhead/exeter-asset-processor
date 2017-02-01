/*
 *  ExAPBitmapXXXA32.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Fri May 02 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#ifndef _EXAPBITMAPXXXA32_H_
#define _EXAPBITMAPXXXA32_H_

#include "ExAPBitmap8bpchWithAlpha.h"


class ExAPBitmapXXX24;


// 32-bit RGBA or 32-bit LabA.

class ExAPBitmapXXXA32 : public ExAPBitmap8bpchWithAlpha
{
protected:

	static void BilinearDownsampleRow(
		int dstPixelsToRender,
		unsigned char * dst, int bytesPerDstPixel,
		int dstCoord, int dstContextStart, int dstContextSize,
		unsigned char * src, int bytesPerSrcPixel,
		int srcCoord, int srcContextStart, int srcContextSize );

public:
	// Constructor.
	ExAPBitmapXXXA32( ExAPColourModel::ModelType colourModelType, int width, int height ) throw( ExAPException );
	
	// Destructor.
	virtual ~ExAPBitmapXXXA32( void );
	
	// Operations.

	// TO_DO: Make these functions protected to avoid inappropriate user calls?

	// Copy pixel data from corresponding no-alpha colour model; new alpha set to 255.
	void Blit( int dstX, int dstY, const ExAPBitmapXXX24 & srcImage, const ExAPRect & srcRectParam );

	// Straight copy of pixel data, including alpha.  No compositing.
	void Blit( int dstX, int dstY, const ExAPBitmapXXXA32 & srcImage, const ExAPRect & srcRectParam );

	// Composite two layers together into one using Porter-Duff OVER.
	void Composite( int dstX, int dstY, const ExAPBitmapXXXA32 & srcImage, const ExAPRect & srcRectParam );

	void BilinearResample( const ExAPRect & dstRect, const ExAPBitmapXXXA32 & src, const ExAPRect & srcRect );
	//virtual void BilinearResample( const ExAPRect & dstRect, const ExAPBitmap8bpchWithAlpha & src, const ExAPRect & srcRect );
	
	//ExAPBitmapXXXA32 * CreateBilinearResampledImage( const ExAPRect & dstRect, const ExAPRect & srcRect ) const;
}; // class ExAPBitmapXXXA32


#endif //_EXAPBITMAPXXXA32_H_


// **** End of File ****
