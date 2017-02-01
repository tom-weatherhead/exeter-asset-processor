/*
 *  ExCodecTestBitHolder.cpp
 *  Exeter Codec Library
 *
 *  Created by Tom Weatherhead on Thu Jul 31 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#include "General\\ExAPException.h"
#include "ExCodecTestBitHolder.h"


ExCodecHuffmanTestBitHolder::ExCodecHuffmanTestBitHolder( void )
	: m_readHead( m_vec.begin() ),
		m_buffer( 0 ),
		m_numBitsInBuffer( 0 ),
		m_EOF( false )
{
} // ExCodecHuffmanTestBitHolder::ExCodecHuffmanTestBitHolder()


void ExCodecHuffmanTestBitHolder::PrepareToRead( void )
{
	m_readHead = m_vec.begin();
	m_buffer = 0;
	m_numBitsInBuffer = 0;
} // ExCodecHuffmanTestBitHolder::PrepareToRead()


int ExCodecHuffmanTestBitHolder::ReadBits( int numBits )	// Returns the actual bits that were read.  Less safe than the following version.
{
	int bits = 0;
	const int numBitsRead = ReadBits( numBits, bits );

	ExAssert( numBitsRead == numBits )
	return( bits );
}


int ExCodecHuffmanTestBitHolder::ReadBits( int numBits, int & bits )
{
	
	if( m_EOF )
	{
		return( 0 );
	}

	while( m_numBitsInBuffer < numBits )
	{

		if( m_readHead == m_vec.end() )
		{
			m_EOF = true;
			return( 0 );
		}

		const unsigned char ch = *m_readHead++;

		m_buffer <<= 8;
		m_buffer |= (unsigned int)ch;
		m_numBitsInBuffer += 8;
	}

	bits = int( m_buffer >> ( m_numBitsInBuffer - numBits ) );

	m_numBitsInBuffer -= numBits;
	m_buffer &= ( ( 1 << m_numBitsInBuffer ) - 1 );
	return( numBits );
} // ExCodecHuffmanTestBitHolder::ReadBits()


bool ExCodecHuffmanTestBitHolder::EndOfFile( void )
{
	return( m_EOF );
} // ExCodecHuffmanTestBitHolder::EndOfFile()


void ExCodecHuffmanTestBitHolder::WriteBits( int bits, int numBits )
{
	ExAssert( m_numBitsInBuffer + numBits <= 32 )

	m_buffer <<= numBits;
	m_buffer |= bits;
	m_numBitsInBuffer += numBits;

	while( m_numBitsInBuffer >= 8 )
	{
#if 1
		const unsigned char ch = (unsigned char)( ( m_buffer >> ( m_numBitsInBuffer - 8 ) ) & 0xFF );

		m_vec.push_back( ch );
#else
		const int ch = int( ( m_buffer >> ( m_numBitsInBuffer - 8 ) ) & 0xFF );
		const int status = fputc( ch, m_fp );

		ExAssert( status != EOF )
#endif
		m_numBitsInBuffer -= 8;
	}
} // ExCodecHuffmanTestBitHolder::WriteBits()


void ExCodecHuffmanTestBitHolder::Flush( void )
{
	
	if( m_numBitsInBuffer > 0 )
	{
#if 1
		const unsigned char ch = (unsigned char)( ( m_buffer << ( 8 - m_numBitsInBuffer ) ) & 0xFF );

		m_vec.push_back( ch );
#else
		const int ch = int( ( m_buffer << ( 8 - m_numBitsInBuffer ) ) & 0xFF );
		const int status = fputc( ch, m_fp );

		ExAssert( status != EOF )
#endif
		m_numBitsInBuffer = 0;		// So that a subsequent Flush() would do nothing.
	}
} // ExCodecHuffmanTestBitHolder::Flush()


// **** End of File ****
