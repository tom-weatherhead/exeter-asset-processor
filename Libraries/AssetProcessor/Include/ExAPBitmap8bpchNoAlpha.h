/*
 *  ExAPBitmap8bpchNoAlpha.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Mon May 05 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#ifndef _EXAPBITMAP8BPCHNOALPHA_H_
#define _EXAPBITMAP8BPCHNOALPHA_H_

#include "ExAPBitmap8bpch.h"


class ExAPBitmap8bpchNoAlpha : public ExAPBitmap8bpch
{
public:
	// Constructor.
	ExAPBitmap8bpchNoAlpha( ExAPColourModel::ModelType colourModelType, int width, int height ) throw( ExAPException );
	
	// Destructor.
	virtual ~ExAPBitmap8bpchNoAlpha( void );
}; // class ExAPBitmap8bpchNoAlpha


#endif //_EXAPBITMAP8BPCHNOALPHA_H_


// **** End of File ****
