/*
 *  ExAPBitByteSourceSinkV1.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Wed Jun 25 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

//#define TAW_TRACE		1

#include "ExAPBitByteSourceSinkV1.h"
#include "ExAPException.h"


// **** Class ExAPBitSourceV1 ****


ExAPBitSourceV1::ExAPBitSourceV1( const char * filename )
	: m_fp( 0 ),
		m_closeFileWhenFinished( false ),
		m_buffer( 0 ),
		m_numBitsInBuffer( 0 ),
		m_lastNumBitsRead( 0 ),
		m_numChunksOfSameLengthRead( 0 )
{
	m_fp = fopen( filename, "rb" );

	if( m_fp == 0 )
	{
		ThrowExAPException( ExAPException::FileOpenFailed );
	}

	m_closeFileWhenFinished = true;
} // ExAPBitSourceV1::ExAPBitSourceV1()


ExAPBitSourceV1::~ExAPBitSourceV1( void )
{
	
	if( m_fp != 0  &&  m_closeFileWhenFinished )
	{
		fclose( m_fp );
	}

	m_fp = 0;
} // ExAPBitSourceV1::~ExAPBitSourceV1()


int ExAPBitSourceV1::ReadBits( int numBits )	// Max of 31 bits.
{
	// Accumulate bytes in m_buffer until the requested number of bits are available.

#ifdef TAW_TRACE
	printf( "BitSource: Reading %d bits.\n", numBits );
#endif

#ifdef TAW_TRACE
	if( m_lastNumBitsRead == numBits )
	{
		++m_numChunksOfSameLengthRead;
	}
	else
	{

		if( m_lastNumBitsRead > 0 )
		{
			printf( "BitSource: Read %d chunks of %d bits.\n", m_numChunksOfSameLengthRead, m_lastNumBitsRead );
		}

		m_lastNumBitsRead = numBits;
		m_numChunksOfSameLengthRead = 1;
	}
#endif

	while( m_numBitsInBuffer < numBits )
	{
		const int x = fgetc( m_fp );

		if( x == EOF )
		{
			return( -1 );
		}

		m_buffer |= x << m_numBitsInBuffer;
		m_numBitsInBuffer += 8;		// 8 bits per byte.
	}

	const int rtn = m_buffer & ( ( 1 << numBits ) - 1 );

	m_buffer >>= numBits;
	m_numBitsInBuffer -= numBits;
	return( rtn );
} // ExAPBitSourceV1::ReadBits()


bool ExAPBitSourceV1::EndOfFile( void )
{
	return( feof( m_fp ) != 0 );
} // ExAPBitSourceV1::EndOfFile()


// **** Class ExAPBitSinkV1 ****


ExAPBitSinkV1::ExAPBitSinkV1( const char * filename )
	: m_fp( 0 ),
		m_closeFileWhenFinished( false ),
		m_buffer( 0 ),
		m_numBitsInBuffer( 0 ),
		m_lastNumBitsWritten( 0 ),
		m_numChunksOfSameLengthWritten( 0 )
{
	m_fp = fopen( filename, "wb" );

	if( m_fp == 0 )
	{
		ThrowExAPException( ExAPException::FileOpenFailed );
	}

	m_closeFileWhenFinished = true;
} // ExAPBitSinkV1::ExAPBitSinkV1()


ExAPBitSinkV1::~ExAPBitSinkV1( void )
{
	
	if( m_fp != 0 )
	{
		Flush();

		if( m_closeFileWhenFinished )
		{
			fclose( m_fp );
		}

		m_fp = 0;
	}
} // ExAPBitSinkV1::~ExAPBitSinkV1()


void ExAPBitSinkV1::WriteBits( int bits, int numBits )
{
#ifdef TAW_TRACE
	printf( "BitSink: Writing %d bits.\n", numBits );
#endif

#ifdef TAW_TRACE
	if( m_lastNumBitsWritten == numBits )
	{
		++m_numChunksOfSameLengthWritten;
	}
	else
	{

		if( m_lastNumBitsWritten > 0 )
		{
			printf( "BitSink: Wrote %d chunks of %d bits.\n", m_numChunksOfSameLengthWritten, m_lastNumBitsWritten );
		}

		m_lastNumBitsWritten = numBits;
		m_numChunksOfSameLengthWritten = 1;
	}
#endif

	m_buffer |= bits << m_numBitsInBuffer;
	m_numBitsInBuffer += numBits;

	while( m_numBitsInBuffer >= 8 )	// 8 bits per byte.
	{
		fputc( m_buffer & 0xFF, m_fp );
		m_buffer >>= 8;
		m_numBitsInBuffer -= 8;
	}
} // ExAPBitSinkV1::WriteBits()


void ExAPBitSinkV1::Flush( void )
{
	
	if( m_fp != 0 )
	{

		if( m_numBitsInBuffer > 0 )
		{
			// Write out a partial byte as a full byte.
			fputc( m_buffer & 0xFF, m_fp );
			m_numBitsInBuffer = 0;
		}

		fflush( m_fp );
	}
} // ExAPBitSinkV1::Flush()


// **** Class ExAPByteSourceV1 ****


ExAPByteSourceV1::ExAPByteSourceV1( const char * filename )
	: m_fp( 0 ),
		m_closeFileWhenFinished( false ),
		m_bytesRead( 0 )
{
	m_fp = fopen( filename, "rb" );

	if( m_fp == 0 )
	{
		ThrowExAPException( ExAPException::FileOpenFailed );
	}

	m_closeFileWhenFinished = true;
} // ExAPByteSourceV1::ExAPByteSourceV1()


ExAPByteSourceV1::~ExAPByteSourceV1( void )
{
	
	if( m_fp != 0  &&  m_closeFileWhenFinished )
	{
		fclose( m_fp );
	}

	m_fp = 0;
} // ExAPByteSourceV1::~ExAPByteSourceV1()


unsigned char ExAPByteSourceV1::ReadByte( void )
{
	// This function is unable to signal EOF via a return value.
	// Throw an exception upon EOF?
	const unsigned char ch = static_cast<unsigned char>( fgetc( m_fp ) );

	m_bytesRead++;
#if 0 //def TAW_TRACE
	printf( "ByteSource: Byte read: 0x%02X.\n", ch );

	if( m_bytesRead == 4209 )
	{
		printf( "ByteSource: Byte %d is 0x%02X.\n", m_bytesRead - 1, ch );
	}
#endif

	return( ch );
} // ExAPByteSourceV1::ReadByte()


bool ExAPByteSourceV1::EndOfFile( void )
{
	return( feof( m_fp ) != 0 );
} // ExAPByteSourceV1::EndOfFile()


// **** Class ExAPByteSinkV1 ****


ExAPByteSinkV1::ExAPByteSinkV1( const char * filename )
	: m_fp( 0 ),
		m_closeFileWhenFinished( false ),
		m_bytesWritten( 0 )
{
	m_fp = fopen( filename, "wb" );

	if( m_fp == 0 )
	{
		ThrowExAPException( ExAPException::FileOpenFailed );
	}

	m_closeFileWhenFinished = true;
} // ExAPByteSinkV1::ExAPByteSinkV1()


ExAPByteSinkV1::~ExAPByteSinkV1( void )
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
} // ExAPByteSinkV1::~ExAPByteSinkV1()


void ExAPByteSinkV1::WriteBytes( const std::vector<unsigned char> & bytes )
{
	std::vector<unsigned char>::const_iterator i;

	for( i = bytes.begin(); i != bytes.end(); ++i )
	{
		fputc( static_cast<int>( *i ), m_fp );

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
} // ExAPByteSinkV1::WriteBytes()


// **** End of File ****
