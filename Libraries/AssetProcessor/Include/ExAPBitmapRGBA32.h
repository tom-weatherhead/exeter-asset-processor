/*
 *  ExAPBitmapRGBA32.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Wed May 07 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#ifndef _EXAPBITMAPRGBA32_H_
#define _EXAPBITMAPRGBA32_H_

#ifdef MACINTOSH
#include <Carbon/Carbon.h>
#endif

#include "ExAPBitmapXXXA32.h"


class ExAPBitmapRGBA32 : public ExAPBitmapXXXA32
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
	ExAPBitmapRGBA32( int width, int height ) throw( ExAPException );
	
	// Destructor.
	virtual ~ExAPBitmapRGBA32( void );
	
#ifdef MACINTOSH
	void DrawIntoQuartzContext( CGContextRef context ) const;
	void DrawIntoMacWindow( WindowRef window ) const;
	void PixelByteSwap( void ) const;
#endif

	virtual ExAPAutoPtr<ExAPBitmap> CreateCompatibleBitmap( int width, int height ) const throw( ExAPException );
}; // class class ExAPBitmapRGBA32


#endif // #ifndef _EXAPBITMAPRGBA32_H_


// **** End of File ****
