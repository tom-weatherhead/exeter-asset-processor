/*
 *  ExAPBitmapDitherNone.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Sat Apr 30 2005.
 *  Copyright (c) 2005 by Tom Weatherhead. All rights reserved.
 *
 */

#include "ExAPBitmapDither.h"


class ExAPBitmapDitherNone : public ExAPBitmapDither
{
public:
	ExAPBitmapDitherNone( void );
	virtual ~ExAPBitmapDitherNone( void );

	virtual unsigned char DitherPixel(
		const ExAPBitmapPalette & palette,
		unsigned char r, unsigned char g, unsigned char b,
		int nXBase, int nYBase, int nXOffset, int nYOffset );
};


// **** End of File ****
