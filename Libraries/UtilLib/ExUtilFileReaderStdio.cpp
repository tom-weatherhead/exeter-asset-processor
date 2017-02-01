/*
 *  ExUtilFileReaderStdio.cpp
 *  Exeter Utility Library
 *
 *  Created by Tom Weatherhead on Mon Jul 14 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#include <cstdio>
#include "ExUtilFileReaderBase.h"


// **** Class ExUtilFileReaderStdio ****


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
	virtual void GetNullTerminatedString( unsigned char * p, int sizeOfBuffer ) const;
	virtual unsigned short GetWord( void ) const;
	virtual unsigned int GetDoubleWord( void ) const;

#ifdef EXUTIL_IMPLEMENT_GETFILEPOINTER
	// Remove this as soon as the JPEG code no longer needs it.
	virtual FILE * GetFilePointer( void ) const;
#endif
}; // class ExUtilFileReaderStdio


ExUtilFileReaderStdio::ExUtilFileReaderStdio( const std::string & filename, bool littleEndian, bool binary ) throw( ExAPException )
	: ExUtilFileReaderBase( filename, littleEndian ),
		m_fp( fopen( filename.c_str(), binary ? "rb" : "r" ) )
{

	if( m_fp == 0 )
	{
		// Explode.  Gracefully.
		ThrowExAPException( ExAPException::FileOpenFailed );
	}
} // ExAPFileReaderStdio::ExAPFileReaderStdio()


ExUtilFileReaderStdio::~ExUtilFileReaderStdio( void )
{

	if( m_fp != 0 )
	{
		fclose( m_fp );
		m_fp = 0;
	}
} // ExUtilFileReaderStdio::~ExUtilFileReaderStdio()


void ExUtilFileReaderStdio::SetOffset( long offset ) const throw( ExAPException )
{
	
	if( fseek( m_fp, offset, SEEK_SET ) != 0 )
	{
		ThrowExAPException( ExAPException::FileSeekFailed );
	}
} // ExUtilFileReaderStdio::SetOffset()


bool ExUtilFileReaderStdio::EndOfFile( void ) const
{
	return( feof( m_fp ) != 0 );
} // ExUtilFileReaderStdio::EndOfFile()


int ExUtilFileReaderStdio::GetByteAsInt( void ) const
{
	return( fgetc( m_fp ) );
} // ExUtilFileReaderStdio::GetByteAsInt()


unsigned char ExUtilFileReaderStdio::GetByte( void ) const
{
	return( static_cast<unsigned char>( GetByteAsInt() ) );
} // ExUtilFileReaderStdio::GetByteAsInt()


int ExUtilFileReaderStdio::GetBytes( unsigned char * p, int numBytes ) const
{
	const size_t bytesRead = fread( p, 1, static_cast<size_t>( numBytes ), m_fp );

	return( static_cast<int>( bytesRead ) );
} // ExUtilFileReaderStdio::GetBytes()


void ExUtilFileReaderStdio::GetNullTerminatedString( unsigned char * p, int sizeOfBuffer ) const
{
	if( fgets( (char *)p, sizeOfBuffer, m_fp ) == 0 )
	{
		// We could check feof() and ferror() to determine what happened.
		ThrowExAPException( ExAPException::FileReadFailed );
	}

	// Truncate any appended newline:
	//p[sizeOfBuffer - 1] = '\0';
	const int stringLength = strlen( (char *)p );

	if( stringLength > 0 )
	{
		unsigned char * pLastChar = p + stringLength - 1;

		if( *pLastChar == '\n' )
		{
			*pLastChar = '\0';
		}
	}
} // ExUtilFileReaderStdio::GetNullTerminatedString()


unsigned short ExUtilFileReaderStdio::GetWord( void ) const
{
	const int byte0 = fgetc( m_fp );
	const int byte1 = fgetc( m_fp );

	return( ExUtilFileReaderBase::GetWord(
		static_cast<unsigned short>( byte0 ),
		static_cast<unsigned short>( byte1 ) ) );
} // ExUtilFileReaderStdio::GetWord()


unsigned int ExUtilFileReaderStdio::GetDoubleWord( void ) const
{
	const int byte0 = fgetc( m_fp );
	const int byte1 = fgetc( m_fp );
	const int byte2 = fgetc( m_fp );
	const int byte3 = fgetc( m_fp );

	return( ExUtilFileReaderBase::GetDoubleWord(
		static_cast<unsigned int>( byte0 ),
		static_cast<unsigned int>( byte1 ),
		static_cast<unsigned int>( byte2 ),
		static_cast<unsigned int>( byte3 ) ) );
} // ExUtilFileReaderStdio::GetDoubleWord()



#ifdef EXUTIL_IMPLEMENT_GETFILEPOINTER
// Remove this as soon as the JPEG code no longer needs it.

FILE * ExUtilFileReaderStdio::GetFilePointer( void ) const
{
	return( m_fp );
}
#endif


// **** Class ExUtilFileReaderBaseInterface ****


ExAPAutoPtr<ExUtilFileReaderBaseInterface> ExUtilFileReaderBaseInterface::StdioCreate( const std::string & filename, bool littleEndian, bool binary ) throw( ExAPException )
{
	ExAPAutoPtr<ExUtilFileReaderBaseInterface> p;

	try
	{
		p = new ExUtilFileReaderStdio( filename, littleEndian, binary );
	}
	catch( ExAPException & )
	{
		throw;
	}
	catch( ... )
	{
		ThrowExAPException( ExAPException::UnknownException );
	}

	if( p == 0 )
	{
		ThrowExAPException( ExAPException::ResourceAcquisitionFailed );
	}

	return( p );
}


// **** End of File ****
