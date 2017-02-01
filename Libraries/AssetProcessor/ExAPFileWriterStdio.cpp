/*
 *  ExAPFileWriterStdio.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Sat Jul 26 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#include "General\\ExAPException.h"
#include "ExAPFileWriterStdio.h"


ExAPFileWriterStdio::ExAPFileWriterStdio( const std::string & filename, bool littleEndian, bool binary ) throw( ExAPException )
	: m_fp( fopen( filename.c_str(), binary ? "wb" : "w" ) ),
		m_littleEndian( littleEndian ),
		m_byteCount( 0 ),
		m_haveExecutedSeek( false )
{

	if( m_fp == 0 )
	{
		// Explode.  Gracefully.
		ThrowExAPException( ExAPException::FileOpenFailed );
	}
} // ExAPFileWriterStdio::ExAPFileWriterStdio()


ExAPFileWriterStdio::~ExAPFileWriterStdio( void )
{

	if( m_fp != 0 )
	{
		fclose( m_fp );
		m_fp = 0;
	}
} // ExAPFileWriterStdio::~ExAPFileReaderStdio()


void ExAPFileWriterStdio::SetOffset( long offset )	// Throws an exception if it fails.
{
	
	if( fseek( m_fp, offset, SEEK_SET ) != 0 )
	{
		ThrowExAPException( ExAPException::FileSeekFailed );
	}

	m_haveExecutedSeek = true;
	m_byteCount = (unsigned int)offset;
} // ExAPFileWriterStdio::SetOffset()


unsigned int ExAPFileWriterStdio::GetOffset( void ) const
{
	return( m_byteCount );
} // ExAPFileWriterStdio::GetOffset()


void ExAPFileWriterStdio::Align( unsigned int alignment )
{
	
	if( alignment == 0 )
	{
		return;
	}

	while( m_byteCount % alignment != 0 )
	{
		WriteByte( 0 );
	}
} // ExAPFileWriterStdio::Align()


void ExAPFileWriterStdio::WriteByte( int c )
{
	const int status = fputc( c, m_fp );

	ExAssert( status != EOF )
	m_byteCount++;
} // ExAPFileWriterStdio::WriteByte()


void ExAPFileWriterStdio::WriteBytes( unsigned char * p, size_t n )
{
	const size_t bytesWritten = fwrite( p, 1, n, m_fp );

	ExAssert( bytesWritten == n )
	m_byteCount += (unsigned int)n;
} // ExAPFileWriterStdio::WriteBytes()


void ExAPFileWriterStdio::WriteWord( unsigned short w )
{
	unsigned char a[2];
	const unsigned char c0 = (unsigned char)( w & 0xFF );
	const unsigned char c1 = (unsigned char)( ( w >> 8 ) & 0xFF );

	if( m_littleEndian )
	{
		a[0] = c0;
		a[1] = c1;
	}
	else
	{
		a[0] = c1;
		a[1] = c0;
	}

	WriteBytes( a, sizeof( a ) );
} // ExAPFileWriterStdio::WriteWord()


void ExAPFileWriterStdio::WriteDoubleWord( unsigned int dw )
{
	unsigned char a[4];
	const unsigned char c0 = (unsigned char)( dw & 0xFF );
	const unsigned char c1 = (unsigned char)( ( dw >> 8 ) & 0xFF );
	const unsigned char c2 = (unsigned char)( ( dw >> 16 ) & 0xFF );
	const unsigned char c3 = (unsigned char)( ( dw >> 24 ) & 0xFF );

	if( m_littleEndian )
	{
		a[0] = c0;
		a[1] = c1;
		a[2] = c2;
		a[3] = c3;
	}
	else
	{
		a[0] = c3;
		a[1] = c2;
		a[2] = c1;
		a[3] = c0;
	}

	WriteBytes( a, sizeof( a ) );
} // ExAPFileWriterStdio::WriteDoubleWord()


// **** End of File ****
