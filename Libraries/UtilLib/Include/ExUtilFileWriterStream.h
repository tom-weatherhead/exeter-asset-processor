/*
 *  ExUtilFileWriterStream.h
 *  Exeter Utility Library
 *
 *  Created by Tom Weatherhead on Tue May 24 2005.
 *  Copyright (c) 2003-2005 by Tom Weatherhead. All rights reserved.
 *
 */

#ifndef _EXUTILFILEWRITERSTREAM_H_
#define _EXUTILFILEWRITERSTREAM_H_

#include <fstream>

#include "ExUtilFileWriterBaseInterface.h"


class ExUtilFileWriterStream : public ExUtilFileWriterBaseInterface
{
private:
	ExAPAutoPtr<std::basic_ofstream<unsigned char> > m_pStream;

public:
	explicit ExUtilFileWriterStream( const std::string & filename, bool littleEndian = true, bool binary = true ) throw( ExAPException );
	virtual ~ExUtilFileWriterStream( void );

	virtual void SetOffset( long offset );	// Throws an exception if it fails.
	virtual unsigned int GetOffset( void ) const;
	virtual void Align( unsigned int alignment );
	virtual void WriteByte( int c );
	virtual void WriteBytes( unsigned char * p, size_t n );
	virtual void WriteWord( unsigned short w );
	virtual void WriteDoubleWord( unsigned int dw );

#if 0
	// Remove this as soon as the JPEG code no longer needs it.
	inline FILE * GetFilePointer( void ) const
	{
		return( m_fp );
	}
#endif
}; // class ExUtilFileReaderStream


#endif // #ifndef _EXUTILFILEWRITERSTREAM_H_


// **** End of File ****
