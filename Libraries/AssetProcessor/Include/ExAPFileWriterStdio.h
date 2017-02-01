/*
 *  ExAPFileWriterStdio.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Sat Jul 26 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */


#ifndef _EXAPFILEWRITERSTDIO_H_
#define _EXAPFILEWRITERSTDIO_H_		1

#include <cstdio>
#include <string>
#include "ExAPFileWriterBase.h"


class ExAPFileWriterStdio : public ExAPFileWriterBase
{
private:
	FILE * m_fp;
	const bool m_littleEndian;
	unsigned int m_byteCount;
	bool m_haveExecutedSeek;

public:
	explicit ExAPFileWriterStdio( const std::string & filename, bool littleEndian = true, bool binary = true ) throw( ExAPException );
	virtual ~ExAPFileWriterStdio( void );
	virtual void SetOffset( long offset );	// Throws an exception if it fails.
	virtual unsigned int GetOffset( void ) const;
	virtual void Align( unsigned int alignment );
	virtual void WriteByte( int c );
	virtual void WriteBytes( unsigned char * p, size_t n );
	virtual void WriteWord( unsigned short w );
	virtual void WriteDoubleWord( unsigned int dw );
}; // class ExAPFileWriterStdio


#endif // #ifndef _EXAPFILEWRITERSTDIO_H_


// **** End of File ****
