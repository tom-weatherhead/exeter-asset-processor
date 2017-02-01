/*
 *  ExAPFileReaderStdio.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Mon Jul 14 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#ifndef _EXAPFILEREADERSTDIO_H_
#define _EXAPFILEREADERSTDIO_H_

#include <cstdio>

#include "General\\ExAPException.h"
#include "ExAPFileReaderBase.h"


class ExAPFileReaderStdio : public ExAPFileReaderBase
{
private:
	FILE * m_fp;

public:
	explicit ExAPFileReaderStdio( const std::string & filename, bool littleEndian = true, bool binary = true ) throw( ExAPException );
	virtual ~ExAPFileReaderStdio( void );
	virtual void SetOffset( long offset ) const throw( ExAPException );	// Throws an exception if it fails.
	virtual bool EndOfFile( void ) const;
	virtual int GetByteAsInt( void ) const;
	virtual unsigned char GetByte( void ) const;
	virtual int GetBytes( unsigned char * p, int numBytes ) const;
	virtual unsigned short GetWord( void ) const;
	virtual unsigned int GetDoubleWord( void ) const;

#if 1
	// Remove this as soon as the JPEG code no longer needs it.
	inline FILE * GetFilePointer( void ) const
	{
		return( m_fp );
	}
#endif
}; // class ExAPFileReaderStdio


#endif // #ifndef _EXAPFILEREADERSTDIO_H_


// **** End of File ****
