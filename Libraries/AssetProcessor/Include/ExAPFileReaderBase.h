/*
 *  ExAPFileReaderBase.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Mon Jul 14 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#ifndef _EXAPFILEREADERBASE_H_
#define _EXAPFILEREADERBASE_H_

#include <string>


class ExAPFileReaderBase
{
protected:
	std::string m_filename;
	bool m_littleEndian;

public:
	explicit ExAPFileReaderBase( const std::string & filename, bool littleEndian = true );
	virtual ~ExAPFileReaderBase( void );

	inline const std::string & GetFilename( void ) const
	{
		return( m_filename );
	}

	void SetEndianness( bool littleEndian );

	virtual void SetOffset( long offset ) const = 0;	// Throws an exception if it fails.

	virtual bool EndOfFile( void ) const = 0;

	virtual int GetByteAsInt( void ) const = 0;
	virtual unsigned char GetByte( void ) const = 0;

	virtual void SkipBytes( int numBytes ) const;
	virtual int GetBytes( unsigned char * p, int numBytes ) const = 0;

	unsigned short GetWord( unsigned short word0, unsigned short word1 ) const;
	virtual unsigned short GetWord( void ) const = 0;
	unsigned short GetWord( unsigned char * p ) const;

	unsigned int GetDoubleWord( unsigned int dword0, unsigned int dword1, unsigned int dword2, unsigned int dword3 ) const;
	virtual unsigned int GetDoubleWord( void ) const = 0;
	unsigned int GetDoubleWord( unsigned char * p ) const;
}; // class ExAPFileReaderBase


#endif // #ifndef _EXAPFILEREADERBASE_H_


// **** End of File ****
