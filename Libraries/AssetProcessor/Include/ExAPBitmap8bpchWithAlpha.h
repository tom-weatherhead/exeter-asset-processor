/*
 *  ExAPBitmap8bpchWithAlpha.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Mon May 05 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#ifndef _EXAPBITMAP8BPCHWITHALPHA_H_
#define _EXAPBITMAP8BPCHWITHALPHA_H_

//#include <Carbon/Carbon.h>
#include "ExAPBitmap8bpch.h"


class ExAPBitmap8bpchWithAlpha : public ExAPBitmap8bpch
{
public:
	// Constructor.
	ExAPBitmap8bpchWithAlpha( ExAPColourModel::ModelType colourModelType, int width, int height ) throw( ExAPException );
	
	// Destructor.
	virtual ~ExAPBitmap8bpchWithAlpha( void );
}; // class ExAPBitmap8bpchWithAlpha


#endif //_EXAPBITMAP8BPCHWITHALPHA_H_


// **** End of File ****
