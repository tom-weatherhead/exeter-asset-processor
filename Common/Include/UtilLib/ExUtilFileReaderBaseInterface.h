/*
 *  ExUtilFileReaderBaseInterface.h
 *  Exeter Utility Library
 *
 *  Created by Tom Weatherhead on Mon Jul 14 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#ifndef _EXUTILFILEREADERBASEINTERFACE_H_
#define _EXUTILFILEREADERBASEINTERFACE_H_

#define EXUTIL_IMPLEMENT_GETFILEPOINTER		1

#include <string>
#include "General\\ExAPAutoPtr.h"


class ExUtilFileReaderBaseInterface
{
public:
	static ExAPAutoPtr<ExUtilFileReaderBaseInterface> StdioCreate( const std::string & filename, bool littleEndian = true, bool binary = true ) throw( ExAPException );
	static ExAPAutoPtr<ExUtilFileReaderBaseInterface> StreamCreate( const std::string & filename, bool littleEndian = true, bool binary = true ) throw( ExAPException );

	virtual ~ExUtilFileReaderBaseInterface( void );

	virtual const std::string & GetFilename( void ) const = 0;

	virtual void SetEndianness( bool littleEndian ) = 0;

	virtual void SetOffset( long offset ) const = 0;	// Throws an exception if it fails.

	virtual bool EndOfFile( void ) const = 0;

	virtual int GetByteAsInt( void ) const = 0;
	virtual unsigned char GetByte( void ) const = 0;

	virtual void SkipBytes( int numBytes ) const = 0;
	virtual int GetBytes( unsigned char * p, int numBytes ) const = 0;

	// Reads until sizeOfBuffer - 1 bytes have been read, or a '\n' / endl has been encountered; then terminates the string with a null byte.
	// The '\n', if encountered, is *not* written to the string.
	virtual void GetNullTerminatedString( unsigned char * p, int sizeOfBuffer ) const = 0;

	//virtual void GetNullTerminatedString( std::string & str, int maxNumCharsToAppend ) = 0;

	virtual unsigned short GetWord( unsigned short word0, unsigned short word1 ) const = 0;
	virtual unsigned short GetWord( void ) const = 0;
	virtual unsigned short GetWord( unsigned char * p ) const = 0;

	virtual unsigned int GetDoubleWord( unsigned int dword0, unsigned int dword1, unsigned int dword2, unsigned int dword3 ) const = 0;
	virtual unsigned int GetDoubleWord( void ) const = 0;
	virtual unsigned int GetDoubleWord( unsigned char * p ) const = 0;

#ifdef EXUTIL_IMPLEMENT_GETFILEPOINTER
	// Remove this as soon as the JPEG code no longer needs it.
	virtual FILE * GetFilePointer( void ) const = 0;
#endif
}; // class ExUtilFileReaderBaseInterface


#endif // #ifndef _EXUTILFILEREADERBASEINTERFACE_H_


// **** End of File ****
