/*
 *  ExAPBitmap1BitBlackWhite.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Wed May 18 2005.
 *  Copyright (c) 2005 by Tom Weatherhead. All rights reserved.
 *
 */

#ifndef _EXAPBITMAP1BITBLACKWHITE_H_
#define _EXAPBITMAP1BITBLACKWHITE_H_

#include "UtilLib\\ExUtilBitByteSourceSinkInterface.h"
#include "ExAPBitmapNon8BitPaletted.h"


#if 0 //def _MSC_VER			// TAW 2002/03/11 : ie. if MSVC
#ifndef MACINTOSH		// TAW 2002/02/23 : ie. if MSVC

// Suppress warning C4786: Identifier truncated to 255 chars in the debug info.
#pragma warning(disable : 4786)

#endif
#endif


class ExAPBitmap1BitBWProcessorInterface
{
public:
	virtual ~ExAPBitmap1BitBWProcessorInterface( void )
	{
	}

	virtual void ReadBit( bool b ) = 0;
	virtual bool ModifyBit( bool b ) = 0;
};


class ExAPBitmap1BitBlackWhite : public ExAPBitmapNon8BitPaletted
{
public:
	// Constructors.
	ExAPBitmap1BitBlackWhite( int width, int height, int fileRowAlignmentInBytes = 0 ) throw( ExAPException );

	// Destructor.
	virtual ~ExAPBitmap1BitBlackWhite( void );

	void Invert( void );
	//int PopulationCount( void ) const;

	static ExAPAutoPtr<ExAPBitmap1BitBlackWhite> Create( int width, int height, int fileRowAlignmentInBytes = 0 ) throw( ExAPException );

	void ProcessImage(
		ExUtilByteSourceInterface * pByteSource,
		ExUtilByteSinkInterface * pByteSink,
		ExAPBitmap1BitBWProcessorInterface & bitProcessor,
		const ExAPRect & rect );
}; // class ExAPBitmap1BitBlackWhite


#endif  //  #ifndef _EXAPBITMAP1BITBLACKWHITE_H_


// End of File
