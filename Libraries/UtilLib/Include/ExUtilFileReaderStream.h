/*
 *  ExUtilFileReaderStream.h
 *  Exeter Utility Library
 *
 *  Created by Tom Weatherhead on Mon May 23 2005.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#ifndef _EXUTILFILEREADERSTREAM_H_
#define _EXUTILFILEREADERSTREAM_H_

#include <fstream>

#include "ExUtilFileReaderBase.h"


class ExUtilFileReaderStream : public ExUtilFileReaderBase
{
private:
	ExAPAutoPtr<std::basic_ifstream<unsigned char> > m_pStream;

public:
	explicit ExUtilFileReaderStream( const std::string & filename, bool littleEndian = true, bool binary = true ) throw( ExAPException );
	virtual ~ExUtilFileReaderStream( void );

	virtual void SetOffset( long offset ) const throw( ExAPException );	// Throws an exception if it fails.
	virtual bool EndOfFile( void ) const;
	virtual int GetByteAsInt( void ) const;
	virtual unsigned char GetByte( void ) const;
	virtual int GetBytes( unsigned char * p, int numBytes ) const;
	virtual unsigned short GetWord( void ) const;
	virtual unsigned int GetDoubleWord( void ) const;

#if 0
	// Remove this as soon as the JPEG code no longer needs it.
	inline FILE * GetFilePointer( void ) const
	{
		return( m_fp );
	}
#endif
}; // class ExUtilFileReaderStream


#endif // #ifndef _EXUTILFILEREADERSTREAM_H_


// **** End of File ****
