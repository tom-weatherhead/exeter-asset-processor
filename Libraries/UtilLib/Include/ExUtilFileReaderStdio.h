/*
 *  ExUtilFileReaderStdio.h
 *  Exeter Utility Library
 *
 *  Created by Tom Weatherhead on Mon Jul 14 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#ifndef _EXUTILFILEREADERSTDIO_H_
#define _EXUTILFILEREADERSTDIO_H_

#include <cstdio>

//#include "General\\ExAPException.h"
#include "ExUtilFileReaderBase.h"


class ExUtilFileReaderStdio : public ExUtilFileReaderBase
{
private:
	FILE * m_fp;

public:
	explicit ExUtilFileReaderStdio( const std::string & filename, bool littleEndian = true, bool binary = true ) throw( ExAPException );
	virtual ~ExUtilFileReaderStdio( void );

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
}; // class ExUtilFileReaderStdio


#endif // #ifndef _EXUTILFILEREADERSTDIO_H_


// **** End of File ****
