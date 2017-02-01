/*
 *  ExAPFileWriterBase.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Sat Jul 26 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */


#ifndef _EXAPFILEWRITERBASE_H_
#define _EXAPFILEWRITERBASE_H_		1

#include "General\\ExAPAutoPtr.h"


class ExAPFileWriterBase
{
public:
	static ExAPAutoPtr<ExAPFileWriterBase> Create( const std::string & filename, bool littleEndian = true, bool binary = true ) throw( ExAPException );

	virtual ~ExAPFileWriterBase( void );

	virtual void SetOffset( long offset ) = 0;	// Throws an exception if it fails.
	virtual unsigned int GetOffset( void ) const = 0;
	virtual void Align( unsigned int alignment ) = 0;
	virtual void WriteByte( int c ) = 0;
	virtual void WriteBytes( unsigned char * p, size_t n ) = 0;
	virtual void WriteWord( unsigned short w ) = 0;
	virtual void WriteDoubleWord( unsigned int dw ) = 0;
}; // class ExAPFileWriterBase


#endif // #ifndef _EXAPFILEWRITERBASE_H_


// **** End of File ****
