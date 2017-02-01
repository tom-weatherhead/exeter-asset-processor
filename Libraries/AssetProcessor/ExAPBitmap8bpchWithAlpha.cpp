/*
 *  ExAPBitmap8bpchWithAlpha.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Mon May 05 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#include <memory.h>
#include "ExAPBitmap8bpchWithAlpha.h"


// Constructor.

ExAPBitmap8bpchWithAlpha::ExAPBitmap8bpchWithAlpha( ExAPColourModel::ModelType colourModelType, int width, int height ) throw( ExAPException )
	: ExAPBitmap8bpch( colourModelType, width, height )
{
	memset( m_buffer, 0, size_t( m_bytesPerLine * m_height ) );
}

	
// Destructor.

ExAPBitmap8bpchWithAlpha::~ExAPBitmap8bpchWithAlpha( void )
{
}


// **** End of File ****
