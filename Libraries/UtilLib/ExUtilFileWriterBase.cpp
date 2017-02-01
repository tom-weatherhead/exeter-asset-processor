/*
 *  ExAPFileWriterBase.cpp
 *  Exeter Utility Library
 *
 *  Created by Tom Weatherhead on Tue May 24 2005.
 *  Copyright (c) 2003-2005 by Tom Weatherhead. All rights reserved.
 *
 */

#include "ExUtilFileWriterBase.h"


ExUtilFileWriterBase::ExUtilFileWriterBase( const std::string & filename, bool littleEndian )
	:	m_filename( filename ),
		m_littleEndian( littleEndian ),
		m_byteCount( 0 )
{
}


ExUtilFileWriterBase::~ExUtilFileWriterBase( void )
{
	// Do nothing.
} // ExUtilFileWriterBase::~ExUtilFileWriterBase()


unsigned int ExUtilFileWriterBase::GetOffset( void ) const
{
	return( m_byteCount );
} // ExUtilFileWriterStdio::GetOffset()


void ExUtilFileWriterBase::Align( unsigned int alignment )
{
	
	if( alignment == 0 )
	{
		return;
	}

#if 1
	const unsigned int offset = m_byteCount % alignment;

	if( offset > 0 )
	{
		static const unsigned char c = 0;

		WriteBytes( c, alignment - offset );
	}
#else
	while( m_byteCount % alignment != 0 )
	{
		WriteByte( 0 );
	}
#endif
} // ExUtilFileWriterStdio::Align()


void ExUtilFileWriterBase::WriteBytes( unsigned char c, unsigned int numCopies )
{
	m_byteCount += numCopies;

	while( numCopies-- > 0 )
	{
		WriteByte( c );
	}
}


void ExUtilFileWriterBase::WriteBytes( const unsigned char * p, size_t n )
{

	for( int i = 0; i < n; ++i )
	{
		WriteByte( p[i] );
	}
}


void ExUtilFileWriterBase::WriteNullTerminatedString( const unsigned char * p )
{
	WriteBytes( p, strlen( (const char *)p ) );
} // ExUtilFileWriterStdio::WriteNullTerminatedString()


void ExUtilFileWriterBase::WriteWord( unsigned short w )
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
}


void ExUtilFileWriterBase::WriteDoubleWord( unsigned int dw )
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
}


#if 1 //def EXUTIL_IMPLEMENT_GETFILEPOINTER
// Remove this as soon as the JPEG code no longer needs it.

FILE * ExUtilFileWriterBase::GetFilePointer( void ) const
{
	ThrowExAPException( ExAPException::UnimplementedFunction );
	return( 0 );
}
#endif


// **** End of File ****
