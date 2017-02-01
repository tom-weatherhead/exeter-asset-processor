/*
 *  ExAPBitmapRGBA64.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Tue Jul 15 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

// Note: In an ExAPBitmapRGBA64 pixel:
// Word 0 is Red.
// Word 1 is Green.
// Word 2 is Blue.
// Word 3 is Alpha.
// (where a Word is two bytes.)


#ifndef _EXAPBITMAPRGBA64_H_
#define _EXAPBITMAPRGBA64_H_

#include "ExAPBitmap16bpch.h"


class ExAPBitmapRGBA64 : public ExAPBitmap16bpch
{
protected:

	virtual void ConvertToRGBA64(
		unsigned short * dstRow,
		int dstSamplesPerRow,
		const ExAPRect & srcRect ) const;

	virtual void ConvertFromRGBA64(
		const unsigned short * srcRow,
		int srcSamplesPerRow,
		const ExAPRect & dstRect ) const;

public:

	// Constructor.
	ExAPBitmapRGBA64( int width, int height ) throw( ExAPException );

	// Destructor.
	virtual ~ExAPBitmapRGBA64( void );

	virtual ExAPAutoPtr<ExAPBitmap> CreateCompatibleBitmap( int width, int height ) const throw( ExAPException );
}; // class ExAPBitmapRGBA64


#endif // #ifndef _EXAPBITMAPRGBA64_H_


// **** End of File ****
