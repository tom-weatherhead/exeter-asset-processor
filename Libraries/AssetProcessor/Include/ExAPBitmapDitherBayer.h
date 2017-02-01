/*
 *  ExAPBitmapDitherBayer.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Fri Apr 29 2005.
 *  Copyright (c) 2005 by Tom Weatherhead. All rights reserved.
 *
 */

#include "ExAPBitmapDither.h"


class ExAPBitmapDitherBayer : public ExAPBitmapDither
{
#if 0
private:
	int ExAPBitmapDitherBayer::ComputeMaxPossibleError( unsigned char r, unsigned char g, unsigned char b );
#endif

public:
	ExAPBitmapDitherBayer( void );
	virtual ~ExAPBitmapDitherBayer( void );

	virtual unsigned char DitherPixel(
		const ExAPBitmapPalette & palette,
		unsigned char r, unsigned char g, unsigned char b,
		int nXBase, int nYBase, int nXOffset, int nYOffset );
};


// **** End of File ****
