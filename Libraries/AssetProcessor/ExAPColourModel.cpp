/*
 *  ExAPColourModel.cpp
 *  AssetProcessor
 *
 *  Created by Tom Weatherhead on Sat May 03 2003.
 *  Copyright (c) 2003 by Tom Weatherhead.  All rights reserved.
 *
 */

#include "General\\ExAPException.h"
#include "ExAPColourModel.h"


// Constructor.
	
ExAPColourModel::ExAPColourModel( ModelType type ) throw( ExAPException )
	: m_type( type ),
		m_bytesPerPixel( CalculateBytesPerPixel( type ) )
{
} // ExAPColourModel::ExAPColourModel()


int ExAPColourModel::CalculateBitsPerPixel( ModelType type ) throw( ExAPException )
{
	int bitsPerPixel = 0;

	switch( type )
	{
		case Grey8:
			bitsPerPixel = 8;
			break;

		case RGB24:
			bitsPerPixel = 24;
			break;
				
		case RGBA32:
		case CMYK32:
			bitsPerPixel = 32;
			break;

		case RGBA64:
			bitsPerPixel = 64;
			break;

		case Paletted:
			// Could be 1, 2, 4, or 8, so throw an exception.
			ThrowExAPException( ExAPException::IllegalParameterValue );

		default:
			// Throw an exception.
			ThrowExAPException( ExAPException::UnsupportedColourModel );
	}

	return( bitsPerPixel );
} // ExAPColourModel::CalculateBytesPerPixel()


int ExAPColourModel::CalculateBytesPerPixel( ModelType type ) throw( ExAPException )
{
	int bytesPerPixel = 0;

	switch( type )
	{
		case Grey8:
		case Paletted:  //  Only if 8 bits per pixel
			bytesPerPixel = 1;
			break;

		case RGB24:
			bytesPerPixel = 3;
			break;
				
		case RGBA32:
		case CMYK32:
			bytesPerPixel = 4;
			break;

		case RGBA64:
			bytesPerPixel = 8;
			break;

		default:
			// Throw an exception.
			ThrowExAPException( ExAPException::UnsupportedColourModel );
	}

	return( bytesPerPixel );
} // ExAPColourModel::CalculateBytesPerPixel()


// **** End of File ****
