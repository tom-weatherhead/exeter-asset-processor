/*
 *  ExUtilBitSourceSink.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Thu May 19 2005.
 *  Copyright (c) 2005 by Tom Weatherhead. All rights reserved.
 *
 */

//#define TAW_TRACE		1

#include "UtilLib\\ExUtilBitByteSourceSinkInterface.h"


// **** Class ExUtilBitSource ****


class ExUtilBitSource : public ExUtilBitSourceInterface
{
private:
	ExUtilByteSourceInterface & m_byteSource;
	int m_buffer;
	int m_numBitsInBuffer;
#ifdef TAW_TRACE
	int m_lastNumBitsRead;
	int m_numChunksOfSameLengthRead;
#endif
	//std::queue<bool> m_bufferQueue;

public:
	ExUtilBitSource( ExUtilByteSourceInterface & byteSource, int initialBitOffset = 0, int initialWorkingInputByte = 0 );
	virtual ~ExUtilBitSource( void );

	virtual int ReadBits( int numBits );	// Returns the actual bits that were read.  Less safe than the following version.
	virtual int ReadBits( int numBits, int & bits );	// Returns number of bits read.  Max of 31 bits.
	//virtual void ReadBits( unsigned char * buf, int numBits ) = 0;
	//virtual ExAPAutoPtr<std::vector<bool> > ReadBits( int numBits ) = 0;
	virtual bool EndOfFile( void );
}; // class ExUtilBitSource


ExUtilBitSource::ExUtilBitSource( ExUtilByteSourceInterface & byteSource, int initialBitOffset, int initialWorkingInputByte )
	: m_byteSource( byteSource ),
		m_buffer( 0 ),
		m_numBitsInBuffer( 0 )
#ifdef TAW_TRACE
		, m_lastNumBitsRead( 0 ),
		m_numChunksOfSameLengthRead( 0 )
#endif
{

	if( initialBitOffset > 0 )
	{
		m_buffer = initialWorkingInputByte & ( 255 >> initialBitOffset );
		m_numBitsInBuffer = 8 - initialBitOffset;
	}
} // ExUtilBitSource::ExUtilBitSource()


ExUtilBitSource::~ExUtilBitSource( void )
{
} // ExUtilBitSource::~ExUtilBitSource()


int ExUtilBitSource::ReadBits( int numBits )	// Returns the actual bits that were read.  Less safe than the following version.
{
	int bits = 0;
	const int numBitsRead = ReadBits( numBits, bits );

	ExAssert( numBitsRead == numBits )
	return( bits );
}


int ExUtilBitSource::ReadBits( int numBits, int & bits )	// Returns number of bits read.  Max of 31 bits.
{
	// Accumulate bytes in m_buffer until the requested number of bits are available.
	// Should this return a std::vector<bool> or std::bitset ?

#ifdef TAW_TRACE
	printf( "BitSource: Reading %d bits.\n", numBits );

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
		//const int x = fgetc( m_fp );
		const int x = (int)m_byteSource.ReadByte();

		//if( x == EOF )
		if( m_byteSource.EndOfFile() )
		{
			// Return the number of bits actually read?
			bits = m_buffer;
			return( m_numBitsInBuffer );
		}

		m_buffer |= x << m_numBitsInBuffer;
		m_numBitsInBuffer += 8;		// 8 bits per byte.
	}

	bits = m_buffer & ( ( 1 << numBits ) - 1 );
	m_buffer >>= numBits;
	m_numBitsInBuffer -= numBits;
	return( numBits );
} // ExUtilBitSource::ReadBits()


bool ExUtilBitSource::EndOfFile( void )
{
	//return( feof( m_fp ) != 0 );
	return( m_byteSource.EndOfFile() );
} // ExUtilBitSource::EndOfFile()


// **** Class ExUtilBitSourceInterface ****


ExAPAutoPtr<ExUtilBitSourceInterface> ExUtilBitSourceInterface::CreateBitSource( ExUtilByteSourceInterface & byteSource )
{
	ExAPAutoPtr<ExUtilBitSourceInterface> pBitSource;

	try
	{
		pBitSource = new ExUtilBitSource( byteSource );
	}
	catch( ExAPException & )
	{
		throw;
	}
	catch( ... )
	{
		ThrowExAPException( ExAPException::UnknownException );
	}

	if( pBitSource == 0 )
	{
		ThrowExAPException( ExAPException::ResourceAcquisitionFailed );
	}

	return( pBitSource );
}


// **** Class ExUtilBitSink ****


class ExUtilBitSink : public ExUtilBitSinkInterface
{
private:
	ExUtilByteSinkInterface & m_byteSink;
	int m_buffer;
	int m_numBitsInBuffer;
#ifdef TAW_TRACE
	int m_lastNumBitsWritten;
	int m_numChunksOfSameLengthWritten;
#endif

	void WriteAllFullBytes( void );

public:
	ExUtilBitSink( ExUtilByteSinkInterface & byteSink, int initialBitOffset = 0, int initialWorkingOutputByte = 0 );
	virtual ~ExUtilBitSink( void );

	virtual void WriteBits( int bits, int numBits );
	virtual void Flush( void );
}; // class ExUtilBitSink


ExUtilBitSink::ExUtilBitSink( ExUtilByteSinkInterface & byteSink, int initialBitOffset, int initialWorkingOutputByte )
	: m_byteSink( byteSink ),
		m_buffer( 0 ),
		m_numBitsInBuffer( 0 )
#ifdef TAW_TRACE
		, m_lastNumBitsWritten( 0 ),
		m_numChunksOfSameLengthWritten( 0 )
#endif
{

	if( initialBitOffset > 0 )
	{
		m_buffer = initialWorkingOutputByte & ( 255 >> initialBitOffset );
		m_numBitsInBuffer = 8 - initialBitOffset;
	}
} // ExUtilBitSink::ExUtilBitSink()


ExUtilBitSink::~ExUtilBitSink( void )
{
} // ExUtilBitSink::~ExUtilBitSink()


void ExUtilBitSink::WriteAllFullBytes( void )
{

	while( m_numBitsInBuffer >= 8 )	// 8 bits per byte.
	{
		m_byteSink.WriteByte( ( m_buffer >> ( m_numBitsInBuffer - 8 ) ) & 0xFF );
		m_numBitsInBuffer -= 8;
	}
}


void ExUtilBitSink::WriteBits( int bits, int numBits )
{
#ifdef TAW_TRACE
	printf( "BitSink: Writing %d bits.\n", numBits );

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

#if 1
	m_buffer <<= numBits;
	m_buffer |= ( bits & ( ( 1 << numBits ) - 1 ) );
#else
	m_buffer |= bits << m_numBitsInBuffer;
#endif
	m_numBitsInBuffer += numBits;

	WriteAllFullBytes();
} // ExUtilBitSink::WriteBits()


void ExUtilBitSink::Flush( void )
{
	WriteAllFullBytes();	

	if( m_numBitsInBuffer > 0 )
	{
		// Write out a partial byte as a full byte.
		m_byteSink.WriteByte( m_buffer & ( ( 1 << m_numBitsInBuffer ) - 1 ) );
		m_numBitsInBuffer = 0;
	}
} // ExUtilBitSink::Flush()


// **** Class ExUtilBitSinkInterface ****


ExAPAutoPtr<ExUtilBitSinkInterface> ExUtilBitSinkInterface::CreateBitSink( ExUtilByteSinkInterface & byteSink )
{
	ExAPAutoPtr<ExUtilBitSinkInterface> pBitSink;

	try
	{
		pBitSink = new ExUtilBitSink( byteSink );
	}
	catch( ExAPException & )
	{
		throw;
	}
	catch( ... )
	{
		ThrowExAPException( ExAPException::UnknownException );
	}

	if( pBitSink == 0 )
	{
		ThrowExAPException( ExAPException::ResourceAcquisitionFailed );
	}

	return( pBitSink );
}


// **** End of File ****
