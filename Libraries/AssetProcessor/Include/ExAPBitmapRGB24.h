/*
 *  ExAPBitmapRGB24.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Sat May 03 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

// Note: In an ExAPBitmapRGB24 pixel:
// Byte 0 is Red.
// Byte 1 is Green.
// Byte 2 is Blue.


#ifndef _EXAPBITMAPRGB24_H_
#define _EXAPBITMAPRGB24_H_

#include "ExAPBitmapXXX24.h"


class ExAPBitmapRGB24 : public ExAPBitmapXXX24
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
	ExAPBitmapRGB24( int width, int height ) throw( ExAPException );
	
	// Destructor.
	virtual ~ExAPBitmapRGB24( void );

	virtual void TestDisplay( void ) const;

	virtual ExAPAutoPtr<ExAPBitmap> CreateCompatibleBitmap( int width, int height ) const throw( ExAPException );

	// **** File Import and Export ****

	virtual void ExportToGIF( const std::string & dstFilename ) const;

	// **** Bitmap Effects ****

	//virtual ExAPAutoPtr<ExAPBitmap> AutoEqualize( void ) const;
}; // class ExAPBitmapRGB24


#endif //_EXAPBITMAPRGB24_H_


// **** End of File ****
