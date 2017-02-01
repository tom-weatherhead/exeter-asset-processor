/*
 *  ExUtilFileWriterStream.cpp
 *  Exeter Utility Library
 *
 *  Created by Tom Weatherhead on Tue May 24 2005.
 *  Copyright (c) 2003-2005 by Tom Weatherhead. All rights reserved.
 *
 */

#include <fstream>
#include "ExUtilFileWriterBase.h"


// **** Class ExUtilFileWriterStream ****


class ExUtilFileWriterStream : public ExUtilFileWriterBase
{
private:
	ExAPAutoPtr<std::basic_ofstream<unsigned char> > m_pStream;

public:
	explicit ExUtilFileWriterStream( const std::string & filename, bool littleEndian = true, bool binary = true ) throw( ExAPException );
	virtual ~ExUtilFileWriterStream( void );

	virtual void SetOffset( long offset );	// Throws an exception if it fails.
	virtual void WriteByte( unsigned char c );
	virtual void WriteByte( int c );
	virtual void WriteEndLine( void );
}; // class ExUtilFileWriterStream


ExUtilFileWriterStream::ExUtilFileWriterStream( const std::string & filename, bool littleEndian, bool binary ) throw( ExAPException )
	: ExUtilFileWriterBase( filename, littleEndian ),
		m_pStream()
{

	try
	{
		static const std::ios_base::open_mode binaryOpenMode = std::ios_base::in | std::ios_base::binary;
		static const std::ios_base::open_mode nonBinaryOpenMode = std::ios_base::in;
		const std::ios_base::open_mode openMode = binary ? binaryOpenMode : nonBinaryOpenMode;

		m_pStream = new std::basic_ofstream<unsigned char>( filename.c_str(), openMode );
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


ExUtilFileWriterStream::~ExUtilFileWriterStream( void )
{
}


void ExUtilFileWriterStream::SetOffset( long offset )	// Throws an exception if it fails.
{
	// TAW_TODO
	ThrowExAPException( ExAPException::UnimplementedFunction );
}


void ExUtilFileWriterStream::WriteByte( unsigned char c )
{
	m_pStream->put( c );
	m_byteCount++;
}


void ExUtilFileWriterStream::WriteByte( int c )
{
	WriteByte( static_cast<unsigned char>( c ) );
}


void ExUtilFileWriterStream::WriteEndLine( void )
{
	*m_pStream << std::endl;
}


// **** Class ExUtilFileWriterBaseInterface ****


ExAPAutoPtr<ExUtilFileWriterBaseInterface> ExUtilFileWriterBaseInterface::StreamCreate( const std::string & filename, bool littleEndian, bool binary ) throw( ExAPException )
{
	ExAPAutoPtr<ExUtilFileWriterBaseInterface> p;

	try
	{
		p = new ExUtilFileWriterStream( filename, littleEndian, binary );
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
} // ExUtilFileWriterBaseInterface::StreamCreate()


// End of File
