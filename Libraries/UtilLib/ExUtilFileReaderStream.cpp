/*
 *  ExUtilFileReaderStream.cpp
 *  Exeter Utility Library
 *
 *  Created by Tom Weatherhead on Mon May 23 2005.
 *  Copyright (c) 2003-2005 by Tom Weatherhead. All rights reserved.
 *
 */

#include <fstream>
#include "ExUtilFileReaderBase.h"


// **** Class ExUtilFileReaderStream ****


class ExUtilFileReaderStream : public ExUtilFileReaderBase
{
private:
	ExAPAutoPtr<std::basic_ifstream<unsigned char> > m_pStream;

public:
	explicit ExUtilFileReaderStream( const std::string & filename, bool littleEndian = true, bool binary = true ) throw( ExAPException );
	virtual ~ExUtilFileReaderStream( void );

	virtual void SetOffset( long offset ) const throw( ExAPException );	// Throws an exception if it fails.
	virtual bool EndOfFile( void ) const;
	virtual int GetByteAsInt( void ) const;
	virtual unsigned char GetByte( void ) const;
	virtual int GetBytes( unsigned char * p, int numBytes ) const;
	virtual void GetNullTerminatedString( unsigned char * p, int sizeOfBuffer ) const;
	virtual unsigned short GetWord( void ) const;
	virtual unsigned int GetDoubleWord( void ) const;
}; // class ExUtilFileReaderStream


ExUtilFileReaderStream::ExUtilFileReaderStream( const std::string & filename, bool littleEndian, bool binary ) throw( ExAPException )
	: ExUtilFileReaderBase( filename, littleEndian ),
		m_pStream( 0 )
{

	try
	{
		const std::ios_base::open_mode openMode = binary ? ( std::ios_base::in | std::ios_base::binary ) : std::ios_base::in;

		m_pStream = new std::basic_ifstream<unsigned char>( filename.c_str(), openMode );
	}
	catch( ... )
	{
		ThrowExAPException( ExAPException::UnknownException );
	}

	if( m_pStream == 0 )
	{
		ThrowExAPException( ExAPException::ResourceAcquisitionFailed );
	}

	if( !*m_pStream )
	{
		ThrowExAPException( ExAPException::FileOpenFailed );
	}
}


ExUtilFileReaderStream::~ExUtilFileReaderStream( void )
{
}


void ExUtilFileReaderStream::SetOffset( long offset ) const throw( ExAPException )	// Throws an exception if it fails.
{
	// TAW_TODO
	ThrowExAPException( ExAPException::UnimplementedFunction );
}


bool ExUtilFileReaderStream::EndOfFile( void ) const
{
	//return( !*m_pStream );
	return( m_pStream->eof() );
}


int ExUtilFileReaderStream::GetByteAsInt( void ) const
{
	const unsigned char c = GetByte();

#if 0
	if( EndOfFile() )
	{
		return( EOF );	// std::istream::EOF ?
	}
#endif

	return( static_cast<int>( c ) );
}


unsigned char ExUtilFileReaderStream::GetByte( void ) const
{
	unsigned char c;

	m_pStream->get( c );
	//return( static_cast<unsigned char>( c ) );
	return( c );
}


int ExUtilFileReaderStream::GetBytes( unsigned char * p, int numBytes ) const
{
	int i = 0;

	for( ; i < numBytes; ++i )
	{
		p[i] = GetByte();

		if( EndOfFile() )
		{
			break;
		}
	}

	return( i );
}


void ExUtilFileReaderStream::GetNullTerminatedString( unsigned char * p, int sizeOfBuffer ) const
{
	ThrowExAPException( ExAPException::UnimplementedFunction );
}


unsigned short ExUtilFileReaderStream::GetWord( void ) const
{
	const unsigned char byte0 = GetByte();
	const unsigned char byte1 = GetByte();

	return( ExUtilFileReaderBase::GetWord(
		static_cast<unsigned short>( byte0 ),
		static_cast<unsigned short>( byte1 ) ) );
}


unsigned int ExUtilFileReaderStream::GetDoubleWord( void ) const
{
	const unsigned char byte0 = GetByte();
	const unsigned char byte1 = GetByte();
	const unsigned char byte2 = GetByte();
	const unsigned char byte3 = GetByte();

	return( ExUtilFileReaderBase::GetDoubleWord(
		static_cast<unsigned int>( byte0 ),
		static_cast<unsigned int>( byte1 ),
		static_cast<unsigned int>( byte2 ),
		static_cast<unsigned int>( byte3 ) ) );
}


// **** Class ExUtilFileReaderBaseInterface ****


ExAPAutoPtr<ExUtilFileReaderBaseInterface> ExUtilFileReaderBaseInterface::StreamCreate( const std::string & filename, bool littleEndian, bool binary ) throw( ExAPException )
{
	ExAPAutoPtr<ExUtilFileReaderBaseInterface> p;

	try
	{
		p = new ExUtilFileReaderStream( filename, littleEndian, binary );
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


// End of File
