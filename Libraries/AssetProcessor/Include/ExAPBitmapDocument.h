/*
 *  ExAPBitmapDocument.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Wed Apr 30 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#ifndef _EXAPBITMAPDOCUMENT_H_
#define _EXAPBITMAPDOCUMENT_H_

#include <string>
#include <vector>

#include "ExAPBitmap.h"


class ExAPBitmapDocument
{
public:
	typedef std::vector< ExAPAutoPtr<ExAPBitmap> > BitmapVectorType;
	BitmapVectorType m_v;

	// Constructors.
	ExAPBitmapDocument( const std::string & filePath ) throw( ExAPException );	// Create from a file.
	//ExAPBitmapDocument( int width, int height );	// Create an empty document.
	
	// Destructor.
	/* virtual */ ~ExAPBitmapDocument( void );
	
	// Operations.
	void TestDisplay( void ) const throw( ExAPException );

	ExAPAutoPtr<ExAPBitmap> GetComposite( void ) const throw( ExAPException );

	//Resample();

	// **** File Import and Export ****

	void ExportToTIFF(
		const std::string & dstFilename,
		bool useLZWCompression = true,
		bool littleEndian = true ) const;
}; // class ExAPBitmapDocument


#endif //_EXAPBITMAPDOCUMENT_H_


// **** End of File ****
