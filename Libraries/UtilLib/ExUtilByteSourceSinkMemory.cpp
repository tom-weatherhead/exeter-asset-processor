/*
 *  ExUtilByteSourceSinkMemory.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Sun May 22 2005.
 *  Copyright (c) 2005 by Tom Weatherhead. All rights reserved.
 *
 */

#include "UtilLib\\ExUtilBitByteSourceSinkInterface.h"


// **** Class ExUtilByteSourceMemory ****


class ExUtilByteSourceMemory : public ExUtilByteSourceInterface
{
private:
	unsigned char * m_p;
	unsigned char * m_pNextLine;
	int m_bytesPerLine;

public:
	ExUtilByteSourceMemory( unsigned char * p, int bytesPerLine = 0 );

	virtual ~ExUtilByteSourceMemory( void );

	//virtual int ReadByte( void );	// Return int to allow return of EOF.
	virtual unsigned char ReadByte( void );
	//virtual void ReadBytes( unsigned char * buf, int numBytes );
	virtual void AdvanceToNextLine( void );
	virtual bool EndOfFile( void );
}; // class ExUtilPByteSourceMemory


ExUtilByteSourceMemory::ExUtilByteSourceMemory( unsigned char * p, int bytesPerLine )
	: m_p( p ),
		m_bytesPerLine( bytesPerLine )
{
	m_pNextLine = m_p + m_bytesPerLine;
}


ExUtilByteSourceMemory::~ExUtilByteSourceMemory( void )
{
}


unsigned char ExUtilByteSourceMemory::ReadByte( void )
{
	return( *m_p++ );
}


void ExUtilByteSourceMemory::AdvanceToNextLine( void )
{
	m_p = m_pNextLine;
	m_pNextLine += m_bytesPerLine;
}


bool ExUtilByteSourceMemory::EndOfFile( void )
{
	return( false );
}


// **** Class ExUtilByteSourceInterface ****


ExAPAutoPtr<ExUtilByteSourceInterface> ExUtilByteSourceInterface::CreateByteSourceMemory( unsigned char * p, int bytesPerLine )
{
	ExAPAutoPtr<ExUtilByteSourceInterface> pByteSource;

	try
	{
		pByteSource = new ExUtilByteSourceMemory( p, bytesPerLine );
	}
	catch( ExAPException & )
	{
		throw;
	}
	catch( ... )
	{
		ThrowExAPException( ExAPException::UnknownException );
	}

	if( pByteSource == 0 )
	{
		ThrowExAPException( ExAPException::ResourceAcquisitionFailed );
	}

	return( pByteSource );
}


// **** Class ExUtilByteSinkMemory ****


class ExUtilByteSinkMemory : public ExUtilByteSinkInterface
{
private:
	unsigned char * m_p;
	unsigned char * m_pNextLine;
	int m_bytesPerLine;

public:
	ExUtilByteSinkMemory( unsigned char * p, int bytesPerLine = 0 );

	virtual ~ExUtilByteSinkMemory( void );

	virtual void WriteByte( unsigned char ch );
	virtual void WriteBytes( const std::vector<unsigned char> & bytes );
	//virtual void WriteBytes( unsigned char * buf, int numBytes );
	virtual void AdvanceToNextLine( void );
}; // class ExUtilPByteSinkMemory


ExUtilByteSinkMemory::ExUtilByteSinkMemory( unsigned char * p, int bytesPerLine )
	: m_p( p ),
		m_bytesPerLine( bytesPerLine )
{
	m_pNextLine = m_p + m_bytesPerLine;
}


ExUtilByteSinkMemory::~ExUtilByteSinkMemory( void )
{
}


void ExUtilByteSinkMemory::WriteByte( unsigned char ch )
{
	*m_p++ = ch;
}


void ExUtilByteSinkMemory::WriteBytes( const std::vector<unsigned char> & bytes )
{
	std::vector<unsigned char>::const_iterator i;

	for( i = bytes.begin(); i != bytes.end(); ++i )
	{
		*m_p++ = *i;
	}
}


void ExUtilByteSinkMemory::AdvanceToNextLine( void )
{
	m_p = m_pNextLine;
	m_pNextLine += m_bytesPerLine;
}


// **** Class ExUtilByteSinkInterface ****


ExAPAutoPtr<ExUtilByteSinkInterface> ExUtilByteSinkInterface::CreateByteSinkMemory( unsigned char * p, int bytesPerLine )
{
	ExAPAutoPtr<ExUtilByteSinkInterface> pByteSink;

	try
	{
		pByteSink = new ExUtilByteSinkMemory( p, bytesPerLine );
	}
	catch( ExAPException & )
	{
		throw;
	}
	catch( ... )
	{
		ThrowExAPException( ExAPException::UnknownException );
	}

	if( pByteSink == 0 )
	{
		ThrowExAPException( ExAPException::ResourceAcquisitionFailed );
	}

	return( pByteSink );
}


// End of File
