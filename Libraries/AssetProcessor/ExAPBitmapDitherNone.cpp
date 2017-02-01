/*
 *  ExAPBitmapDitherNone.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Sat Apr 30 2005.
 *  Copyright (c) 2005 by Tom Weatherhead. All rights reserved.
 *
 */

#include "ExAPBitmapDitherNone.h"
#include "ExAPBitmapPalette.h"


ExAPBitmapDitherNone::ExAPBitmapDitherNone( void )
	: ExAPBitmapDither( ExAPBitmapDither::DitherNone )
{
}


ExAPBitmapDitherNone::~ExAPBitmapDitherNone( void )
{
}


unsigned char ExAPBitmapDitherNone::DitherPixel(
	const ExAPBitmapPalette & palette,
	unsigned char r, unsigned char g, unsigned char b,
	int nXBase, int nYBase, int nXOffset, int nYOffset )
{
	return( (unsigned char)palette.GetNearestEntry( r, g, b ) );
} // DitherPixel()


// **** End of File ****
