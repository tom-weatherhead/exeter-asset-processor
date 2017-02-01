/*
 *  ExUtilFileWriterStdio.cpp
 *  Exeter Utility Library
 *
 *  Created by Tom Weatherhead on Sat Jul 26 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#include <cstdio>
#include "ExUtilFileWriterBase.h"


// **** Class ExUtilFileWriterStdio ****


class ExUtilFileWriterStdio : public ExUtilFileWriterBase
{
private:
	FILE * m_fp;
	//bool m_haveExecutedSeek;

public:
	explicit ExUtilFileWriterStdio( const std::string & filename, bool littleEndian = true, bool binary = true ) throw( ExAPException );
	virtual ~ExUtilFileWriterStdio( void );

	virtual void SetOffset( long offset );	// Throws an exception if it fails.
	virtual void WriteByte( unsigned char c );
	virtual void WriteByte( int c );
	virtual void WriteEndLine( void );
	virtual void WriteBytes( const unsigned char * p, size_t n );
}; // class ExUtilFileWriterStdio


ExUtilFileWriterStdio::ExUtilFileWriterStdio( const std::string & filename, bool littleEndian, bool binary ) throw( ExAPException )
	: ExUtilFileWriterBase( filename, littleEndian ),
		m_fp( fopen( filename.c_str(), binary ? "wb" : "w" ) )
		//, m_haveExecutedSeek( false )
{

	if( m_fp == 0 )
	{
		// Explode.  Gracefully.
		ThrowExAPException( ExAPException::FileOpenFailed );
	}
} // ExUtilFileWriterStdio::ExUtilFileWriterStdio()


ExUtilFileWriterStdio::~ExUtilFileWriterStdio( void )
{

	if( m_fp != 0 )
	{
		fclose( m_fp );
		m_fp = 0;
	}
} // ExUtilFileWriterStdio::~ExUtilFileReaderStdio()


void ExUtilFileWriterStdio::SetOffset( long offset )	// Throws an exception if it fails.
{
	
	if( fseek( m_fp, offset, SEEK_SET ) != 0 )
	{
		ThrowExAPException( ExAPException::FileSeekFailed );
	}

	//m_haveExecutedSeek = true;
	m_byteCount = (unsigned int)offset;
} // ExUtilFileWriterStdio::SetOffset()


void ExUtilFileWriterStdio::WriteByte( unsigned char c )
{
	WriteByte( static_cast<int>( c ) );
} // ExUtilFileWriterStdio::WriteByte()


void ExUtilFileWriterStdio::WriteByte( int c )
{
	const int status = fputc( c, m_fp );

	ExAssert( status != EOF )
	m_byteCount++;
} // ExUtilFileWriterStdio::WriteByte()


void ExUtilFileWriterStdio::WriteEndLine( void )
{
	WriteByte( '\n' );
} // ExUtilFileWriterStdio::WriteEndLine()


void ExUtilFileWriterStdio::WriteBytes( const unsigned char * p, size_t n )
{
	const size_t bytesWritten = fwrite( p, 1, n, m_fp );

	ExAssert( bytesWritten == n )
	m_byteCount += static_cast<unsigned int>( n );
} // ExUtilFileWriterStdio::WriteBytes()


#ifdef EXUTIL_IMPLEMENT_GETFILEPOINTER
// Remove this as soon as the JPEG code no longer needs it.

FILE * ExUtilFileWriterStdio::GetFilePointer( void ) const
{
	return( m_fp );
}
#endif


// **** Class ExUtilFileWriterBaseInterface ****


ExAPAutoPtr<ExUtilFileWriterBaseInterface> ExUtilFileWriterBaseInterface::StdioCreate( const std::string & filename, bool littleEndian, bool binary ) throw( ExAPException )
{
	ExAPAutoPtr<ExUtilFileWriterBaseInterface> p;

	try
	{
		p = new ExUtilFileWriterStdio( filename, littleEndian, binary );
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
} // ExUtilFileWriterBaseInterface::StdioCreate()


ExUtilFileWriterBaseInterface::~ExUtilFileWriterBaseInterface( void )
{
	// Do nothing.
} // ExUtilFileWriterBaseInterface::~ExUtilFileWriterBaseInterface()


// **** End of File ****
