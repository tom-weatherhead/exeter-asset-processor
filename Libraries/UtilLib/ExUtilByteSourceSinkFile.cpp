/*
 *  ExUtilByteSourceSinkStdio.cpp
 *  Exeter Utility Library
 *
 *  Created by Tom Weatherhead on Mon May 23 2005.
 *  Copyright (c) 2005 by Tom Weatherhead. All rights reserved.
 *
 */

#include "UtilLib\\ExUtilBitByteSourceSinkInterface.h"
#include "UtilLib\\ExUtilFileReaderBaseInterface.h"
#include "UtilLib\\ExUtilFileWriterBaseInterface.h"


// **** Class ExUtilByteSourceStdio ****


class ExUtilByteSourceFile : public ExUtilByteSourceInterface
{
private:
	ExUtilFileReaderBaseInterface & m_fileReader;
	int m_bytesPerLine;
	int m_bytesRemainingInLine;

public:
	ExUtilByteSourceFile( ExUtilFileReaderBaseInterface & fileReader, int initialOffset, int bytesPerLine );
	virtual ~ExUtilByteSourceFile( void );

	virtual unsigned char ReadByte( void );
	virtual void AdvanceToNextLine( void );
	virtual bool EndOfFile( void );
};


ExUtilByteSourceFile::ExUtilByteSourceFile( ExUtilFileReaderBaseInterface & fileReader, int initialOffset, int bytesPerLine )
	: m_fileReader( fileReader ),
		m_bytesPerLine( bytesPerLine ),
		m_bytesRemainingInLine( bytesPerLine )
{

	if( initialOffset >= 0 )
	{
		m_fileReader.SetOffset( static_cast<long>( initialOffset ) );
	}
}


ExUtilByteSourceFile::~ExUtilByteSourceFile( void )
{
}


unsigned char ExUtilByteSourceFile::ReadByte( void )
{
	--m_bytesRemainingInLine;
	return( m_fileReader.GetByte() );
}


void ExUtilByteSourceFile::AdvanceToNextLine( void )
{

	if( m_bytesRemainingInLine > 0 )
	{
		m_fileReader.SkipBytes( m_bytesRemainingInLine );
	}

	m_bytesRemainingInLine = m_bytesPerLine;
}

	
bool ExUtilByteSourceFile::EndOfFile( void )
{
	return( m_fileReader.EndOfFile() );
}


// **** Class ExUtilByteSourceInterface ****


ExAPAutoPtr<ExUtilByteSourceInterface> ExUtilByteSourceInterface::CreateByteSourceFile( ExUtilFileReaderBaseInterface & fileReader, int initialOffset, int bytesPerLine )
{
	ExAPAutoPtr<ExUtilByteSourceInterface> pByteSource;

	try
	{
		pByteSource = new ExUtilByteSourceFile( fileReader, initialOffset, bytesPerLine );
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
} // ExUtilByteSourceInterface::CreateByteSourceFile()


// **** Class ExUtilByteSinkFile ****


class ExUtilByteSinkFile : public ExUtilByteSinkInterface
{
private:
	ExUtilFileWriterBaseInterface & m_fileWriter;
	int m_bytesPerLine;
	int m_bytesRemainingInLine;

public:
	ExUtilByteSinkFile( ExUtilFileWriterBaseInterface & fileWriter, int initialOffset, int bytesPerLine );

	virtual ~ExUtilByteSinkFile( void );

	virtual void WriteByte( unsigned char ch );
	virtual void WriteBytes( const std::vector<unsigned char> & bytes );
	virtual void AdvanceToNextLine( void );
};


ExUtilByteSinkFile::ExUtilByteSinkFile( ExUtilFileWriterBaseInterface & fileWriter, int initialOffset, int bytesPerLine )
	: m_fileWriter( fileWriter ),
		m_bytesPerLine( bytesPerLine ),
		m_bytesRemainingInLine( bytesPerLine )
{

	if( initialOffset >= 0 )
	{
		m_fileWriter.SetOffset( static_cast<long>( initialOffset ) );
	}
} // ExUtilByteSinkFile::ExUtilByteSinkFile()


ExUtilByteSinkFile::~ExUtilByteSinkFile( void )
{
} // ExUtilByteSinkFile::~ExUtilByteSinkFile()


void ExUtilByteSinkFile::WriteByte( unsigned char ch )
{
	//fputc( static_cast<int>( ch ), m_fp );
	m_fileWriter.WriteByte( static_cast<int>( ch ) );
	--m_bytesRemainingInLine;
} // ExUtilByteSinkFile::WriteByte()


void ExUtilByteSinkFile::WriteBytes( const std::vector<unsigned char> & bytes )
{
	std::vector<unsigned char>::const_iterator i;

	for( i = bytes.begin(); i != bytes.end(); ++i )
	{
		//fputc( static_cast<int>( *i ), m_fp );
		m_fileWriter.WriteByte( static_cast<int>( *i ) );
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
} // ExUtilByteSinkFile::WriteBytes()


void ExUtilByteSinkFile::AdvanceToNextLine( void )
{

	while( m_bytesRemainingInLine-- > 0 )
	{
		m_fileWriter.WriteByte( 0 );
	}

	m_bytesRemainingInLine = m_bytesPerLine;
} // ExUtilByteSinkFile::AdvanceToNextLine()


// **** Class ExUtilByteSinkInterface ****


ExAPAutoPtr<ExUtilByteSinkInterface> ExUtilByteSinkInterface::CreateByteSinkFile( ExUtilFileWriterBaseInterface & fileWriter, int initialOffset, int bytesPerLine )
{
	ExAPAutoPtr<ExUtilByteSinkInterface> pByteSink;

	try
	{
		pByteSink = new ExUtilByteSinkFile( fileWriter, initialOffset, bytesPerLine );
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
} // ExUtilByteSourceInterface::CreateByteSourceFile()


// End of File
