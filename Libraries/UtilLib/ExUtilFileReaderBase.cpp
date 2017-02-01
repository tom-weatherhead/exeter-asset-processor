/*
 *  ExUtilFileReaderBase.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Mon Jul 14 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#include "ExUtilFileReaderBase.h"


// **** Class ExUtilFileReaderBaseInterface ****


ExUtilFileReaderBaseInterface::~ExUtilFileReaderBaseInterface( void )
{
}

	
// **** Class ExUtilFileReaderBase ****


ExUtilFileReaderBase::ExUtilFileReaderBase( const std::string & filename, bool littleEndian )
	: m_filename( filename ),
		m_littleEndian( littleEndian )
{
} // ExUtilFileReaderBase::ExUtilFileReaderBase()


ExUtilFileReaderBase::~ExUtilFileReaderBase( void )
{
} // ExAPFileReaderBase::~ExAPFileReaderBase()


const std::string & ExUtilFileReaderBase::GetFilename( void ) const
{
	return( m_filename );
}


void ExUtilFileReaderBase::SetEndianness( bool littleEndian )
{
	m_littleEndian = littleEndian;
} // ExUtilFileReaderBase::SetEndianness()


void ExUtilFileReaderBase::SkipBytes( int numBytes ) const
{
	
	while( numBytes-- > 0 )
	{
		(void)GetByteAsInt();
	}
} // ExUtilFileReaderBase::SkipBytes()


unsigned short ExUtilFileReaderBase::GetWord( unsigned short word0, unsigned short word1 ) const
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
} // ExUtilFileReaderBase::GetWord()


unsigned short ExUtilFileReaderBase::GetWord( unsigned char * p ) const
{
	return( GetWord( static_cast<unsigned short>( p[0] ), static_cast<unsigned short>( p[1] ) ) );
} // ExUtilFileReaderBase::GetWord()


unsigned int ExUtilFileReaderBase::GetDoubleWord( unsigned int dword0, unsigned int dword1, unsigned int dword2, unsigned int dword3 ) const
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
} // ExUtilFileReaderBase::GetDoubleWord()


unsigned int ExUtilFileReaderBase::GetDoubleWord( unsigned char * p ) const
{
	return( GetDoubleWord( static_cast<unsigned int>( p[0] ), static_cast<unsigned int>( p[1] ), static_cast<unsigned int>( p[2] ), static_cast<unsigned int>( p[3] ) ) );
} // ExUtilFileReaderBase::GetDoubleWord()


#ifdef EXUTIL_IMPLEMENT_GETFILEPOINTER
// Remove this as soon as the JPEG code no longer needs it.

FILE * ExUtilFileReaderBase::GetFilePointer( void ) const
{
	ThrowExAPException( ExAPException::UnimplementedFunction );
	return( 0 );
}
#endif


// **** End of File ****
