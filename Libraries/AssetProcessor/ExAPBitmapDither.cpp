/*
 *  ExAPBitmapDither.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Sat Apr 30 2005.
 *  Copyright (c) 2005 by Tom Weatherhead. All rights reserved.
 *
 */

//#include "General\ExAPException.h"
#include "ExAPBitmapDither.h"


ExAPBitmapDither::ExAPBitmapDither( DithererType type )
	: m_type( type )
{
}


ExAPBitmapDither::~ExAPBitmapDither( void )
{
}


bool ExAPBitmapDither::Chunkable( void ) const
{
	return( true );
}


// **** End of File ****
