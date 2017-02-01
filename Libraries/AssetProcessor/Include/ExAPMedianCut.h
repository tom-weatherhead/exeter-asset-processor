/*
 *  ExAPMedianCut.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Thu Jul 17 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */


#ifndef _EXAPMEDIANCUT_H_
#define _EXAPMEDIANCUT_H_		1

#include "General\\ExAPAutoPtr.h"
#include "ExAPBitmap8BitPaletted.h"
#include "ExAPBitmapRGB24.h"


ExAPAutoPtr<ExAPBitmap8BitPaletted> CreatePalettedImageViaMedianCut(
	const ExAPBitmapRGB24 & srcImage,
	const ExAPRect & srcRectParam,
	int numColoursInPalette ) throw( ExAPException );


#endif // #ifndef _EXAPMEDIANCUT_H_


// **** End of File ****
