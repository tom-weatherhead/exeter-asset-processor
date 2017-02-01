/*
 *  ExAPBitmapCMYK32.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Sun Jun 22 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#ifndef _EXAPBITMAPCMYK32_H_
#define _EXAPBITMAPCMYK32_H_

#include "ExAPBitmap8bpchNoAlpha.h"


class ExAPBitmapCMYK32 : public ExAPBitmap8bpchNoAlpha
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
	ExAPBitmapCMYK32( int width, int height ) throw( ExAPException );

	// Destructor.
	virtual ~ExAPBitmapCMYK32( void );

	virtual ExAPAutoPtr<ExAPBitmap> CreateCompatibleBitmap( int width, int height ) const throw( ExAPException );
}; // class ExAPBitmapCMYK32


#endif // #ifndef _EXAPBITMAPCMYK32_H_


// **** End of File ****
