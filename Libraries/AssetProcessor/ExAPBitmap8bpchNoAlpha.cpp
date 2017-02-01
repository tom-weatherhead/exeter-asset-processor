/*
 *  ExAPBitmap8bpchNoAlpha.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Mon May 05 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#include "ExAPBitmap8bpchNoAlpha.h"


// Constructor.

ExAPBitmap8bpchNoAlpha::ExAPBitmap8bpchNoAlpha( ExAPColourModel::ModelType colourModelType, int width, int height ) throw( ExAPException )
	: ExAPBitmap8bpch( colourModelType, width, height )
{
}

	
// Destructor.

ExAPBitmap8bpchNoAlpha::~ExAPBitmap8bpchNoAlpha( void )
{
}


// **** End of File ****
