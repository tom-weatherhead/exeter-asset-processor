/*
 *  ExAPBitmapGrey8.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Sun Jun 22 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#ifndef _EXAPBITMAPGREY8_H_
#define _EXAPBITMAPGREY8_H_

#include "ExAPBitmap8bpchNoAlpha.h"


class ExAPBitmapGrey8 : public ExAPBitmap8bpchNoAlpha
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
	ExAPBitmapGrey8( int width, int height ) throw( ExAPException );

	// Destructor.
	virtual ~ExAPBitmapGrey8( void );

	virtual ExAPAutoPtr<ExAPBitmap> CreateCompatibleBitmap( int width, int height ) const throw( ExAPException );
}; // class ExAPBitmapGrey8


#endif // #ifndef _EXAPBITMAPGREY8_H_


// **** End of File ****
