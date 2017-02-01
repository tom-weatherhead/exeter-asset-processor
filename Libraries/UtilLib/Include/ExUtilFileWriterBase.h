/*
 *  ExUtilFileWriterBase.h
 *  Exeter Utility Library
 *
 *  Created by Tom Weatherhead on Tue May 24 2005.
 *  Copyright (c) 2003-2005 by Tom Weatherhead. All rights reserved.
 *
 */


#ifndef _EXUTILFILEWRITERBASE_H_
#define _EXUTILFILEWRITERBASE_H_		1

#include "UtilLib\\ExUtilFileWriterBaseInterface.h"


class ExUtilFileWriterBase : public ExUtilFileWriterBaseInterface
{
protected:
	std::string m_filename;
	bool m_littleEndian;
	unsigned int m_byteCount;

public:
	explicit ExUtilFileWriterBase( const std::string & filename, bool littleEndian = true );
	virtual ~ExUtilFileWriterBase( void );

	virtual unsigned int GetOffset( void ) const;
	virtual void Align( unsigned int alignment );
	virtual void WriteBytes( unsigned char c, unsigned int numCopies );
	virtual void WriteBytes( const unsigned char * p, size_t n );
	virtual void WriteNullTerminatedString( const unsigned char * p );
	virtual void WriteWord( unsigned short w );
	virtual void WriteDoubleWord( unsigned int dw );

#if 1 //def EXUTIL_IMPLEMENT_GETFILEPOINTER
	// Remove this as soon as the JPEG code no longer needs it.
	virtual FILE * GetFilePointer( void ) const;
#endif
}; // class ExUtilFileWriterBaseInterface


#endif // #ifndef _EXUTILFILEWRITERBASE_H_


// **** End of File ****
