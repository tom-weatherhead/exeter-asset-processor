/*
 *  ExAPBitmapDither.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Fri Apr 29 2005.
 *  Copyright (c) 2005 by Tom Weatherhead. All rights reserved.
 *
 */

#ifndef EXAPBITMAPDITHER
#define EXAPBITMAPDITHER

#include "General\ExAPAutoPtr.h"
#include "General\ExAPException.h"
//#include "ExAPBitmapPalette.h"


class ExAPBitmapPalette;


class ExAPBitmapDither
{
public:

	enum DithererType
	{
		DitherNone,
		DitherBayer,
		DitherErrorDiffusion	// Floyd-Steinberg?  What about Jarvis and Stucki?
	};

	const DithererType m_type;

	ExAPBitmapDither( DithererType type );
	virtual ~ExAPBitmapDither( void );

	inline DithererType GetDithererType( void ) const
	{
		return( m_type );
	}

	virtual bool Chunkable( void ) const;

	virtual unsigned char DitherPixel(
		const ExAPBitmapPalette & palette,
		unsigned char r, unsigned char g, unsigned char b,
		int nXBase, int nYBase, int nXOffset, int nYOffset ) = 0;
};


#endif


// **** End of File ****
