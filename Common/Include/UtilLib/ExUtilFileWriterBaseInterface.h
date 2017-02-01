/*
 *  ExUtilFileWriterBaseInterface.h
 *  Exeter Utility Library
 *
 *  Created by Tom Weatherhead on Sat Jul 26 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */


#ifndef _EXUTILFILEWRITERBASEINTERFACE_H_
#define _EXUTILFILEWRITERBASEINTERFACE_H_		1

#include "General\\ExAPAutoPtr.h"


class ExUtilFileWriterBaseInterface
{
public:
	static ExAPAutoPtr<ExUtilFileWriterBaseInterface> StdioCreate( const std::string & filename, bool littleEndian = true, bool binary = true ) throw( ExAPException );
	static ExAPAutoPtr<ExUtilFileWriterBaseInterface> StreamCreate( const std::string & filename, bool littleEndian = true, bool binary = true ) throw( ExAPException );

	virtual ~ExUtilFileWriterBaseInterface( void );

	virtual void SetOffset( long offset ) = 0;	// Throws an exception if it fails.
	virtual unsigned int GetOffset( void ) const = 0;
	virtual void Align( unsigned int alignment ) = 0;
	virtual void WriteByte( unsigned char c ) = 0;
	virtual void WriteByte( int c ) = 0;
	virtual void WriteEndLine( void ) = 0;
	virtual void WriteBytes( unsigned char c, unsigned int numCopies ) = 0;
	virtual void WriteBytes( const unsigned char * p, size_t n ) = 0;
	virtual void WriteNullTerminatedString( const unsigned char * p ) = 0;
	virtual void WriteWord( unsigned short w ) = 0;
	virtual void WriteDoubleWord( unsigned int dw ) = 0;

#if 1 //def EXUTIL_IMPLEMENT_GETFILEPOINTER
	// Remove this as soon as the JPEG code no longer needs it.
	virtual FILE * GetFilePointer( void ) const = 0;
#endif
}; // class ExUtilFileWriterBaseInterface


#endif // #ifndef _EXUTILFILEWRITERBASEINTERFACE_H_


// **** End of File ****
