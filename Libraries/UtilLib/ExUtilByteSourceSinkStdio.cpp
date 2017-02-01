/*
 *  ExUtilByteSourceSinkStdio.cpp
 *  Exeter Utility Library
 *
 *  Created by Tom Weatherhead on Mon May 23 2005.
 *  Copyright (c) 2005 by Tom Weatherhead. All rights reserved.
 *
 */

#include "UtilLib\\ExUtilBitByteSourceSinkInterface.h"


// **** Class ExUtilByteSourceStdio ****


class ExUtilByteSourceStdio : public ExUtilByteSourceInterface
{
private:
	FILE * m_fp;
	bool m_closeFileWhenFinished;
	//int m_bytesRead;
	int m_bytesPerLine;
	int m_bytesRemainingInLine;

public:
	explicit ExUtilByteSourceStdio( const std::string & filename, int initialOffset = 0, int bytesPerLine = 0 );
	//ExUtilByteSourceStdio( ExUtilFileReaderStdio & file, int initialOffset = 0, int bytesPerLine = 0 );

	virtual ~ExUtilByteSourceStdio( void );

	virtual unsigned char ReadByte( void );
	virtual void AdvanceToNextLine( void );
	virtual bool EndOfFile( void );
}; // class ExUtilByteSourceStdio


ExUtilByteSourceStdio::ExUtilByteSourceStdio( const std::string & filename, int initialOffset, int bytesPerLine )
	: m_fp( fopen( filename.c_str(), "rb" ) ),
		m_closeFileWhenFinished( false ),
		//m_bytesRead( 0 ),
		m_bytesPerLine( bytesPerLine ),
		m_bytesRemainingInLine( bytesPerLine )
{
	//m_fp = fopen( filename, "rb" );

	if( m_fp == 0 )
	{
		ThrowExAPException( ExAPException::FileOpenFailed );
	}

	m_closeFileWhenFinished = true;

	if( initialOffset > 0  &&  fseek( m_fp, initialOffset, SEEK_SET ) != 0 )
	{
		ThrowExAPException( ExAPException::FileSeekFailed );
	}
} // ExUtilByteSourceStdio::ExUtilByteSourceStdio()


ExUtilByteSourceStdio::~ExUtilByteSourceStdio( void )
{
	
	if( m_fp != 0  &&  m_closeFileWhenFinished )
	{
		fclose( m_fp );
	}

	m_fp = 0;
} // ExUtilByteSourceStdio::~ExUtilByteSourceStdio()


unsigned char ExUtilByteSourceStdio::ReadByte( void )
{
	// This function is unable to signal EOF via a return value.
	// Throw an exception upon EOF?
	const unsigned char ch = static_cast<unsigned char>( fgetc( m_fp ) );

	//m_bytesRead++;
#if 0 //def TAW_TRACE
	printf( "ByteSource: Byte read: 0x%02X.\n", ch );

	if( m_bytesRead == 4209 )
	{
		printf( "ByteSource: Byte %d is 0x%02X.\n", m_bytesRead - 1, ch );
	}
#endif

	--m_bytesRemainingInLine;
	return( ch );
} // ExUtilByteSourceStdio::ReadByte()


void ExUtilByteSourceStdio::AdvanceToNextLine( void )
{

	if( m_bytesRemainingInLine != 0  &&  fseek( m_fp, m_bytesRemainingInLine, SEEK_CUR ) != 0 )
	{
		ThrowExAPException( ExAPException::FileSeekFailed );
	}

	m_bytesRemainingInLine = m_bytesPerLine;
} // ExUtilByteSourceStdio::AdvanceToNextLine()


bool ExUtilByteSourceStdio::EndOfFile( void )
{
	return( feof( m_fp ) != 0 );
} // ExUtilByteSourceStdio::EndOfFile()


// **** Class ExUtilByteSourceInterface ****


ExAPAutoPtr<ExUtilByteSourceInterface> ExUtilByteSourceInterface::CreateByteSourceStdio( const std::string & filename, int initialOffset, int bytesPerLine )
{
	ExAPAutoPtr<ExUtilByteSourceInterface> pByteSource;

	try
	{
		pByteSource = new ExUtilByteSourceStdio( filename, initialOffset, bytesPerLine );
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
} // ExUtilByteSourceInterface::CreateByteSourceStdio()


// **** Class ExUtilByteSinkStdio ****


class ExUtilByteSinkStdio : public ExUtilByteSinkInterface
{
private:
	FILE * m_fp;
	bool m_closeFileWhenFinished;
	//int m_bytesWritten;
	int m_bytesPerLine;
	int m_bytesRemainingInLine;

public:
	ExUtilByteSinkStdio( const std::string & filename, int initialOffset, int bytesPerLine );
	//ExUtilByteSourceSink( ExUtilFileReaderStdio & file, int initialOffset = 0, int bytesPerLine = 0 );

	virtual ~ExUtilByteSinkStdio( void );

	virtual void WriteByte( unsigned char ch );
	virtual void WriteBytes( const std::vector<unsigned char> & bytes );
	virtual void AdvanceToNextLine( void );
};


ExUtilByteSinkStdio::ExUtilByteSinkStdio( const std::string & filename, int initialOffset, int bytesPerLine )
	: m_fp( fopen( filename.c_str(), "wb" ) ),
		m_closeFileWhenFinished( false ),
		//m_bytesWritten( 0 )
		m_bytesPerLine( bytesPerLine ),
		m_bytesRemainingInLine( bytesPerLine )
{
	//m_fp = fopen( filename, "wb" );

	if( m_fp == 0 )
	{
		ThrowExAPException( ExAPException::FileOpenFailed );
	}

	m_closeFileWhenFinished = true;

	if( initialOffset > 0  &&  fseek( m_fp, initialOffset, SEEK_SET ) != 0 )
	{
		ThrowExAPException( ExAPException::FileSeekFailed );
	}
} // ExUtilByteSinkStdio::ExUtilByteSinkStdio()


ExUtilByteSinkStdio::~ExUtilByteSinkStdio( void )
{
	
	if( m_fp != 0 )
	{
		fflush( m_fp );

		if( m_closeFileWhenFinished )
		{
			fclose( m_fp );
		}

		m_fp = 0;
	}
} // ExUtilByteSinkStdio::~ExUtilByteSinkStdio()


void ExUtilByteSinkStdio::WriteByte( unsigned char ch )
{
	fputc( static_cast<int>( ch ), m_fp );
	--m_bytesRemainingInLine;
} // ExUtilByteSinkStdio::WriteByte()


void ExUtilByteSinkStdio::WriteBytes( const std::vector<unsigned char> & bytes )
{
	std::vector<unsigned char>::const_iterator i;

	for( i = bytes.begin(); i != bytes.end(); ++i )
	{
		fputc( static_cast<int>( *i ), m_fp );
		--m_bytesRemainingInLine;

#if 0 //def TAW_TRACE
		const unsigned char ch = *i;

		printf( "ByteSink: Byte written: 0x%02X.\n", ch );

		if( m_bytesWritten == 4208 )
		{
			printf( "ByteSink: Byte %d is 0x%02X.\n", m_bytesWritten, ch );
		}

		m_bytesWritten++;
#endif
	}
} // ExUtilByteSinkStdio::WriteBytes()


void ExUtilByteSinkStdio::AdvanceToNextLine( void )
{

	if( m_bytesRemainingInLine != 0  &&  fseek( m_fp, m_bytesRemainingInLine, SEEK_CUR ) != 0 )
	{
		ThrowExAPException( ExAPException::FileSeekFailed );
	}

	m_bytesRemainingInLine = m_bytesPerLine;
} // ExUtilByteSinkStdio::AdvanceToNextLine()


// **** Class ExUtilByteSinkInterface ****


ExAPAutoPtr<ExUtilByteSinkInterface> ExUtilByteSinkInterface::CreateByteSinkStdio( const std::string & filename, int initialOffset, int bytesPerLine )
{
	ExAPAutoPtr<ExUtilByteSinkInterface> pByteSink;

	try
	{
		pByteSink = new ExUtilByteSinkStdio( filename, initialOffset, bytesPerLine );
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
} // ExUtilByteSinkInterface::CreateByteSinkStdio()


// End of File
