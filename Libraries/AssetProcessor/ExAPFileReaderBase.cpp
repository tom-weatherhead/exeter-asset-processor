/*
 *  ExAPFileReaderBase.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Mon Jul 14 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#include "ExAPFileReaderBase.h"


ExAPFileReaderBase::ExAPFileReaderBase( const std::string & filename, bool littleEndian )
	: m_filename( filename ),
		m_littleEndian( littleEndian )
{
} // ExAPFileReaderBase::ExAPFileReaderBase()


ExAPFileReaderBase::~ExAPFileReaderBase( void )
{
} // ExAPFileReaderBase::~ExAPFileReaderBase()


void ExAPFileReaderBase::SetEndianness( bool littleEndian )
{
	m_littleEndian = littleEndian;
} // ExAPFileReaderBase::SetEndianness()


void ExAPFileReaderBase::SkipBytes( int numBytes ) const
{
	
	while( numBytes-- > 0 )
	{
		(void)GetByteAsInt();
	}
} // ExAPFileReaderBase::SkipBytes()


unsigned short ExAPFileReaderBase::GetWord( unsigned short word0, unsigned short word1 ) const
{
	unsigned short rtn = 0;

	if( m_littleEndian )
	{
		rtn = (unsigned short)( ( word1 << 8 ) | word0 );
	}
	else
	{
		rtn = (unsigned short)( ( word0 << 8 ) | word1 );
	}

	return( rtn );
} // ExAPFileReaderBase::GetWord()


unsigned short ExAPFileReaderBase::GetWord( unsigned char * p ) const
{
	return( GetWord( static_cast<unsigned short>( p[0] ), static_cast<unsigned short>( p[1] ) ) );
} // ExAPFileReaderBase::GetWord()


unsigned int ExAPFileReaderBase::GetDoubleWord( unsigned int dword0, unsigned int dword1, unsigned int dword2, unsigned int dword3 ) const
{
	unsigned int rtn = 0;

	if( m_littleEndian )
	{
		rtn = ( dword3 << 24 ) | ( dword2 << 16 ) | ( dword1 << 8 ) | dword0;
	}
	else
	{
		rtn = ( dword0 << 24 ) | ( dword1 << 16 ) | ( dword2 << 8 ) | dword3;
	}

	return( rtn );
} // ExAPFileReaderBase::GetDoubleWord()


unsigned int ExAPFileReaderBase::GetDoubleWord( unsigned char * p ) const
{
	return( GetDoubleWord( static_cast<unsigned int>( p[0] ), static_cast<unsigned int>( p[1] ), static_cast<unsigned int>( p[2] ), static_cast<unsigned int>( p[3] ) ) );
} // ExAPFileReaderBase::GetDoubleWord()


// **** End of File ****
