/*
 *  ExCodecLZW.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Thu Jul 17 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

//lint -e1050 : Expected a template argument list '<...>' for template 'std::iterator<<1>,<2>,<3>>'.
#include "ExCodecLZWGIF87a.h"


ExAPAutoPtr<ExAPLZWCodec> ExAPLZWCodec::CreateCodec( TypeType eLZWType, int bitsPerPixel ) throw( ExAPException )
{
	//lint -e429 : Custodial pointer 'codecRawPtr' has not been freed or returned.
	ExAPLZWCodec * codecRawPtr = 0;

	try
	{

		switch( eLZWType )
		{
			case TypeGIF:
				// 8 bits per pixel.
				// codeResizingOffset == 0.
				codecRawPtr = new ExAPLZWCodecGIF87a( bitsPerPixel, 0 );
				break;

			case TypeTIFF:
				// 8 bits per pixel.
				// codeResizingOffset == 1.
				codecRawPtr = new ExAPLZWCodecGIF87a( bitsPerPixel, 1 );
				break;

			default:
				ThrowExAPException( ExAPException::IllegalParameterValue );
		}
	}
	catch( ExAPException & )
	{
		throw;
	}
	catch( ... )
	{
		// Do nothing.
	}

	if( codecRawPtr == 0 )
	{
		ThrowExAPException( ExAPException::ResourceAcquisitionFailed );
	}

#if 0
	return( ExAPAutoPtr<ExAPLZWCodec>( codecRawPtr ) );
#else
	ExAPAutoPtr<ExAPLZWCodec> codec = codecRawPtr;

	return( codec );
#endif
	//lint +e429
} // ExAPLZWCodec::CreateCodec()


// **** End of File ****
