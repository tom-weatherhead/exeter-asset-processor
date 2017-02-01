/*
 *  ExAPBitmapDitherBayer.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Fri Apr 29 2005.
 *  Copyright (c) 2005 by Tom Weatherhead. All rights reserved.
 *
 */

#include "ExAPBitmapDitherBayer.h"
#include "ExAPBitmapPalette.h"


ExAPBitmapDitherBayer::ExAPBitmapDitherBayer( void )
	: ExAPBitmapDither( ExAPBitmapDither::DitherBayer )
{
}


ExAPBitmapDitherBayer::~ExAPBitmapDitherBayer( void )
{
}


#if 0
int ExAPBitmapDitherBayer::ComputeMaxPossibleError( unsigned char r, unsigned char g, unsigned char b )
{
	return(  );
}
#endif


unsigned char ExAPBitmapDitherBayer::DitherPixel(
	const ExAPBitmapPalette & palette,
	unsigned char r, unsigned char g, unsigned char b,
	int nXBase, int nYBase, int nXOffset, int nYOffset )
{
	// GenerateClosestColourVector must have been called since the last colour was added to the palette.
	unsigned char rtn = 0;

	static const unsigned char bayerMatrix[8][8] =
	{
		{  0, 32,  8, 40,  2, 34, 10, 42 },
		{ 48, 16, 56, 24, 50, 18, 58, 26 },
		{ 12, 44,  4, 36, 14, 46,  6, 38 },
		{ 60, 28, 52, 20, 62, 30, 54, 22 },
		{  3, 35, 11, 43,  1, 33,  9, 41 },
		{ 51, 19, 59, 27, 49, 17, 57, 25 },
		{ 15, 47,  7, 39, 13, 45,  5, 37 },
		{ 63, 31, 55, 23, 61, 29, 53, 21 }
	};
	//static const int nMaxPossibleError = 3 * 255 * 255;

	const int nearestEntryIndex = palette.GetNearestEntry( r, g, b );
	const ExAPBitmapRGBPaletteEntry nearestEntry( palette.m_entries[nearestEntryIndex] );
	const int nRDiff = (int)nearestEntry.GetRed() - (int)r;
	const int nGDiff = (int)nearestEntry.GetGreen() - (int)g;
	const int nBDiff = (int)nearestEntry.GetBlue() - (int)b;
	const int nErr = nRDiff * nRDiff + nGDiff * nGDiff + nBDiff * nBDiff;

	const int closestColourVectorSize = palette.m_closestColour.size();
	const int nextNearestEntryIndex = (int)palette.m_closestColour[nearestEntryIndex];
	const ExAPBitmapRGBPaletteEntry nextNearestEntry( palette.m_entries[nextNearestEntryIndex] );
	const int nNextRDiff = (int)nextNearestEntry.GetRed() - (int)r;
	const int nNextGDiff = (int)nextNearestEntry.GetGreen() - (int)g;
	const int nNextBDiff = (int)nextNearestEntry.GetBlue() - (int)b;
	const int nMaxPossibleError = nNextRDiff * nNextRDiff + nNextGDiff * nNextGDiff + nNextBDiff * nNextBDiff;

	const int nOrderedDithNum = bayerMatrix[( nYBase + nYOffset ) & 7][( nXBase + nXOffset ) & 7];

	// Set the dest. pixel in the paletted image.
	// nMaxPossibleError is the max min dist from an RGB to the palette entries.
	// You may want to square the 15 and nOrderedDithNum.
	// 4 * 4 Bayer matrix; 15 = 4 * 4 - 1.

	if( nErr * 63 > nOrderedDithNum * nMaxPossibleError )
	{
		rtn = (unsigned char)nextNearestEntryIndex;
	}
	else
	{
		rtn = (unsigned char)nearestEntryIndex;
	}

	return( rtn );
} // DitherPixel()


// **** End of File ****
