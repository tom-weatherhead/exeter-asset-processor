/*
 *  ExAPFileReaderStdio.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Mon Jul 14 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#include "General\\ExAPException.h"
#include "ExAPFileReaderStdio.h"


ExAPFileReaderStdio::ExAPFileReaderStdio( const std::string & filename, bool littleEndian, bool binary ) throw( ExAPException )
	: ExAPFileReaderBase( filename, littleEndian ),
		m_fp( fopen( filename.c_str(), binary ? "rb" : "r" ) )
{

	if( m_fp == 0 )
	{
		// Explode.  Gracefully.
		ThrowExAPException( ExAPException::FileOpenFailed );
	}
} // ExAPFileReaderStdio::ExAPFileReaderStdio()


ExAPFileReaderStdio::~ExAPFileReaderStdio( void )
{

	if( m_fp != 0 )
	{
		fclose( m_fp );
		m_fp = 0;
	}
} // ExAPFileReaderStdio::~ExAPFileReaderStdio()


void ExAPFileReaderStdio::SetOffset( long offset ) const throw( ExAPException )
{
	
	if( fseek( m_fp, offset, SEEK_SET ) != 0 )
	{
		ThrowExAPException( ExAPException::FileSeekFailed );
	}
} // ExAPFileReaderStdio::SetOffset()


bool ExAPFileReaderStdio::EndOfFile( void ) const
{
	return( feof( m_fp ) != 0 );
} // ExAPFileReaderStdio::EndOfFile()


int ExAPFileReaderStdio::GetByteAsInt( void ) const
{
	return( fgetc( m_fp ) );
} // ExAPFileReaderStdio::GetByteAsInt()


unsigned char ExAPFileReaderStdio::GetByte( void ) const
{
	return( static_cast<unsigned char>( GetByteAsInt() ) );
} // ExAPFileReaderStdio::GetByteAsInt()


int ExAPFileReaderStdio::GetBytes( unsigned char * p, int numBytes ) const
{
	const size_t bytesRead = fread( p, 1, static_cast<size_t>( numBytes ), m_fp );

	return( static_cast<int>( bytesRead ) );
} // ExAPFileReaderStdio::GetBytes()


unsigned short ExAPFileReaderStdio::GetWord( void ) const
{
	const int byte0 = fgetc( m_fp );
	const int byte1 = fgetc( m_fp );

	return( ExAPFileReaderBase::GetWord(
		static_cast<unsigned short>( byte0 ),
		static_cast<unsigned short>( byte1 ) ) );
} // ExAPFileReaderStdio::GetWord()


unsigned int ExAPFileReaderStdio::GetDoubleWord( void ) const
{
	const int byte0 = fgetc( m_fp );
	const int byte1 = fgetc( m_fp );
	const int byte2 = fgetc( m_fp );
	const int byte3 = fgetc( m_fp );

	return( ExAPFileReaderBase::GetDoubleWord(
		static_cast<unsigned int>( byte0 ),
		static_cast<unsigned int>( byte1 ),
		static_cast<unsigned int>( byte2 ),
		static_cast<unsigned int>( byte3 ) ) );
} // ExAPFileReaderStdio::GetDoubleWord()


// **** End of File ****
