/*
 *  ExCodecLZW.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Tue Jun 24 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */


#ifndef _EXCODECLZW_H_
#define _EXCODECLZW_H_

#include "General\\ExAPAutoPtr.h"
#include "UtilLib\\ExUtilBitByteSourceSinkInterface.h"

class ExAPLZWCodec
{
public:

	enum TypeType
	{
		TypeGIF,
		TypeTIFF
	};

	static ExAPAutoPtr<ExAPLZWCodec> CreateCodec( TypeType eLZWType, int bitsPerPixel ) throw( ExAPException );

	virtual void CompressData( ExAPAutoPtr<ExUtilByteSourceInterface> src, ExAPAutoPtr<ExUtilBitSinkInterface> sink /* , int maxDictionarySize = 4096, bool variableCodeSize = false */ ) = 0;
	virtual void DecompressData( ExAPAutoPtr<ExUtilBitSourceInterface> src, ExAPAutoPtr<ExUtilByteSinkInterface> sink /*, int maxDictionarySize = 4096, bool variableCodeSize = false */ ) = 0;
}; // class ExAPLZWCodec


#endif // #ifndef _EXCODECLZW_H_


// **** End of File ****
