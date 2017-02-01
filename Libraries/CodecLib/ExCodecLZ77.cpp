/*
 *  ExCodecLZ77.cpp
 *  Exeter Codec Library
 *
 *  Created by Tom Weatherhead on Thu Jul 31 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

//#define TAW_TRACE		1
#define TAW_LZ77_TEST		1

#include <deque>

#include "General\\ExAPAutoPtr.h"
#include "General\\ExAPException.h"
#include "UtilLib\\ExUtilBitByteSourceSinkInterface.h"

#ifdef TAW_LZ77_TEST
#include "ExCodecTestBitHolder.h"
#endif


class ExCodecLZ77
{
private:

	class HistoryBuf
	{
private:
		const unsigned int m_windowSize;
		std::deque<unsigned char> m_buf;
		bool m_lastByteUnprocessed;

		bool RunsMatch( unsigned int i1, unsigned int i2, unsigned int n ) const;

public:
		HistoryBuf( unsigned int windowSize );

		void PushByte( unsigned char ch );
		unsigned char GetBack( void ) const;
		unsigned char LookupU( unsigned int u ) const;

		// Returns n, the size of the repeated string
		unsigned int FindLongestRepeat( ExAPAutoPtr<ExUtilByteSourceInterface> src, unsigned char & literalByte, unsigned int & u );
	}; // class HistoryBuf

public:
	static void CompressData( ExAPAutoPtr<ExUtilByteSourceInterface> src, ExAPAutoPtr<ExUtilBitSinkInterface> sink, unsigned int windowExponent );
	static void DecompressData( ExAPAutoPtr<ExUtilBitSourceInterface> src, ExAPAutoPtr<ExUtilByteSinkInterface> sink, unsigned int windowExponent );
}; // class ExCodecLZ77


ExCodecLZ77::HistoryBuf::HistoryBuf( unsigned int windowSize )
	: m_windowSize( windowSize ),
		m_lastByteUnprocessed( false )
{
} // ExCodecLZ77::HistoryBuf::HistoryBuf()


void ExCodecLZ77::HistoryBuf::PushByte( unsigned char ch )
{
	m_buf.push_back( ch );

#if 0
	ExAssert( m_buf.size() <= m_windowSize )
#else
	while( m_buf.size() > m_windowSize )
	{
		m_buf.pop_front();
	}
#endif
} // ExCodecLZ77::HistoryBuf::PushByte()


unsigned char ExCodecLZ77::HistoryBuf::GetBack( void ) const
{
	return( m_buf.back() );
} // ExCodecLZ77::HistoryBuf::GetBack()


unsigned char ExCodecLZ77::HistoryBuf::LookupU( unsigned int u ) const
{
	ExAssert( m_buf.size() == m_windowSize )
	ExAssert( u < m_windowSize )
	return( m_buf[m_windowSize - 1 - u] );
} // ExCodecLZ77::HistoryBuf::LookupU()


bool ExCodecLZ77::HistoryBuf::RunsMatch( unsigned int i1, unsigned int i2, unsigned int n ) const
{

	while( n-- > 0 )
	{

		if( m_buf[i1++] != m_buf[i2++] )
		{
			return( false );
		}
	}

	return( true );
} // ExCodecLZ77::HistoryBuf::RunsMatch()


// Returns n, the size of the repeated string
unsigned int ExCodecLZ77::HistoryBuf::FindLongestRepeat( ExAPAutoPtr<ExUtilByteSourceInterface> src, unsigned char & literalByte, unsigned int & u )
{
	const unsigned int newRunStart = m_windowSize;
	unsigned int n = 0;
	unsigned int oldRunStart = 0;

	// TO_DO: Should there be an upper limit on n?

	for( ; ; )
	{
		unsigned char firstByte = 0;

		if( m_lastByteUnprocessed )
		{
			firstByte = m_buf.back();
			m_lastByteUnprocessed = false;
#ifdef TAW_TRACE
			printf( "Reprocessing byte %02X.", int( firstByte ) );
#endif
		}
		else
		{
			firstByte = src->ReadByte();

			if( src->EndOfFile() )
			{
				break;
			}

			m_buf.push_back( firstByte );
#ifdef TAW_TRACE
			printf( "%02X  ", int( firstByte ) );
#endif
		}

		n++;

		bool oldRunOfLengthNFound = false;

		for( ; oldRunStart < newRunStart; ++oldRunStart )
		{

			if( RunsMatch( oldRunStart, newRunStart, n ) )
			{
				oldRunOfLengthNFound = true;
				// 0 <= u < m_windowSize.
				u = newRunStart - oldRunStart - 1;
				break;
			}
		}

		if( !oldRunOfLengthNFound )
		{

			if( n <= 2 )
			{
				literalByte = m_buf[ m_buf.size() - n ];
			}

			if( n > 1 )
			{
				// Return n == 1 if there are no matches for n >= 1.
				m_lastByteUnprocessed = true;
				n--;
#ifdef TAW_TRACE
				printf( "Unprocessed byte %02X.", int( m_buf.back() ) );
#endif
			}

			break;
		}
	}

	const unsigned int maxBufSize = m_windowSize + ( m_lastByteUnprocessed ? 1 : 0 );

	while( m_buf.size() > maxBufSize )
	{
		m_buf.pop_front();
	}

	return( n );
} // ExCodecLZ77::HistoryBuf::FindLongestRepeat()


void ExCodecLZ77::CompressData( ExAPAutoPtr<ExUtilByteSourceInterface> src, ExAPAutoPtr<ExUtilBitSinkInterface> sink, unsigned int windowExponent )
{
	const unsigned int windowSize = 1 << windowExponent;
	HistoryBuf historyBuf( windowSize );
#ifdef TAW_TRACE
	static const unsigned int inputByteCountLimit = 1060;
	unsigned int inputByteCount = 0;
#endif

	// 1) Copy the initial windowSize bytes.

	for( unsigned int i = 0; i < windowSize; ++i )
	{
		const unsigned char ch = src->ReadByte();

		if( src->EndOfFile() )
		{
			return;
		}

		historyBuf.PushByte( ch );
		sink->WriteBits( int( ch ), 8 );		// We assume 8 bits per byte.
	}

	// 2) Encode the rest.

#ifdef TAW_TRACE
	inputByteCount = windowSize;
#endif

	while( !src->EndOfFile() )
	{
		unsigned char literalByte = 0;
		unsigned int u = 0;
		const unsigned int n = historyBuf.FindLongestRepeat( src, literalByte, u );

		if( n == 0 )
		{
			continue;		// EOF.
		}

		// Encode n into a prefix-free code word, using unary/binary code;
		// ie. if n contains m significant bits, precede it with m-1 zero bits.
		unsigned int significantBits = 0;
		unsigned int n2 = n;

		while( n2 != 0 )
		{
			++significantBits;
			n2 >>= 1;
		}

#if 1
		sink->WriteBits( int( n ), 2 * significantBits - 1 );
#else
		sink->WriteBits( 0, significantBits - 1 );
		sink->WriteBits( int( n ), significantBits );
#endif

		if( n == 1 )
		{
			const int backAsInt = int( literalByte );

			sink->WriteBits( backAsInt, 8 );

#ifdef TAW_TRACE
			if( inputByteCount < inputByteCountLimit )
			{
				printf( "Compress: %u: Received literal byte %02X.\n", inputByteCount, backAsInt );
			}
#endif
		}
		else
		{
			// Write u.
			sink->WriteBits( int( u ), windowExponent );
		}

#ifdef TAW_TRACE
		inputByteCount += n;
#endif
	}

	sink->Flush();
} // ExCodecLZ77::CompressData()


void ExCodecLZ77::DecompressData( ExAPAutoPtr<ExUtilBitSourceInterface> src, ExAPAutoPtr<ExUtilByteSinkInterface> sink, unsigned int windowExponent )
{
	const unsigned int windowSize = 1 << windowExponent;
	HistoryBuf historyBuf( windowSize );
#ifdef TAW_TRACE
	static const unsigned int outputByteCountLimit = 1060;
	unsigned int outputByteCount = 0;
#endif

	// 1) Copy the initial windowSize bytes.

	for( unsigned int i = 0; i < windowSize; ++i )
	{
		const unsigned char ch = (unsigned char)( src->ReadBits( 8 ) );

		if( src->EndOfFile() )
		{
			return;
		}

		historyBuf.PushByte( ch );
		sink->WriteByte( ch );
	}

	// 2) Decode the rest.

#ifdef TAW_TRACE
	outputByteCount = windowSize;
#endif

	for( ; ; )
	{
		// Read bits from the source until we read a 1 bit.
		unsigned int numZeroBits = 0;

		for( ; ; )
		{
			const bool bit = src->ReadBits( 1 ) != 0;

			if( src->EndOfFile() )
			{
				return;
			}

			if( bit )
			{
				break;
			}

			++numZeroBits;
		}

		unsigned int n = 1 << numZeroBits;
		
		if( numZeroBits == 0 )
		{
			// n == 1.  Read a literal byte from the source stream.
			const unsigned char literalByte = (unsigned char)( src->ReadBits( 8 ) );

			if( src->EndOfFile() )
			{
				return;
			}

#ifdef TAW_TRACE
			if( outputByteCount < outputByteCountLimit )
			{
				printf( "Decompress: %u: Received literal byte %02X.\n", outputByteCount, int( literalByte ) );
				outputByteCount++;
			}
#endif
			historyBuf.PushByte( literalByte );
			sink->WriteByte( literalByte );
		}
		else
		{
			// n > 1.  Read u, then decode the run.
			n |= (unsigned int)src->ReadBits( int( numZeroBits ) );

			const unsigned int u = (unsigned int)( src->ReadBits( int( windowExponent ) ) );

			if( src->EndOfFile() )
			{
				return;
			}

#ifdef TAW_TRACE
			if( outputByteCount < outputByteCountLimit )
			{
				printf( "Decompress: %u: Received run of length %u; u == %u.\n", outputByteCount, n, u );
				outputByteCount += n;
			}
#endif

			while( n-- > 0 )
			{
				const unsigned char ch = historyBuf.LookupU( u );

#ifdef TAW_TRACE
				if( outputByteCount < outputByteCountLimit )
				{
					printf( "  %02X", int( ch ) );
				}
#endif
				historyBuf.PushByte( ch );
				sink->WriteByte( ch );
			}

#ifdef TAW_TRACE
			if( outputByteCount < outputByteCountLimit )
			{
				printf( "\n" );
			}
#endif
		}
	}
} // ExCodecLZ77::DecompressData()


#ifdef TAW_LZ77_TEST


// **** Class ExCodecLZ77TestByteSource ****


class ExCodecLZ77TestByteSource : public ExUtilByteSourceInterface
{
private:
	FILE * m_fp;

public:
	ExCodecLZ77TestByteSource( const std::string & srcFilename );
	virtual ~ExCodecLZ77TestByteSource( void );
	void Rewind( void );		// Return to start of data for a subsequent pass.
	virtual unsigned char ReadByte( void );			// Return data by value.
	virtual void AdvanceToNextLine( void );
	virtual bool EndOfFile( void );
}; // class ExCodecLZ77TestByteSource


ExCodecLZ77TestByteSource::ExCodecLZ77TestByteSource( const std::string & srcFilename )
	: m_fp( fopen( srcFilename.c_str(), "rb" ) )
{
	ExAssert( m_fp != 0 )
} // ExCodecLZ77TestByteSource::ExCodecLZ77TestByteSource()


ExCodecLZ77TestByteSource::~ExCodecLZ77TestByteSource( void )
{

	if( m_fp != 0 )
	{
		fclose( m_fp );
		m_fp = 0;
	}
} // ExCodecLZ77TestByteSource::~ExCodecLZ77TestByteSource()


#if 1
void ExCodecLZ77TestByteSource::Rewind( void )
{
	const int status = fseek( m_fp, 0L, SEEK_SET );

	ExAssert( status == 0 )
} // ExCodecLZ77TestByteSource::Rewind()
#endif


unsigned char ExCodecLZ77TestByteSource::ReadByte( void )
{
	return( (unsigned char)fgetc( m_fp ) );
} // ExCodecLZ77TestByteSource::ReadByte()


void ExCodecLZ77TestByteSource::AdvanceToNextLine( void )
{
} // ExCodecLZ77TestByteSource::AdvanceToNextLine()


bool ExCodecLZ77TestByteSource::EndOfFile( void )
{
	return( feof( m_fp ) != 0 );
} // ExCodecLZ77TestByteSource::EndOfFile()


// **** Class ExCodecLZ77TestByteSink ****


class ExCodecLZ77TestByteSink : public ExUtilByteSinkInterface
{
private:
	FILE * m_fp;

public:
	ExCodecLZ77TestByteSink( const std::string & dstFilename );
	virtual ~ExCodecLZ77TestByteSink( void );
	virtual void WriteByte( unsigned char ch );
	virtual void WriteBytes( const std::vector<unsigned char> & bytes );
	virtual void AdvanceToNextLine( void );
}; // class ExCodecLZ77TestByteSink


ExCodecLZ77TestByteSink::ExCodecLZ77TestByteSink( const std::string & dstFilename )
	: m_fp( fopen( dstFilename.c_str(), "wb" ) )
{
	ExAssert( m_fp != 0 )
} // ExCodecLZ77TestByteSink::ExCodecLZ77TestByteSink()


ExCodecLZ77TestByteSink::~ExCodecLZ77TestByteSink( void )
{

	if( m_fp != 0 )
	{
		fclose( m_fp );
		m_fp = 0;
	}
} // ExCodecLZ77TestByteSink::~ExCodecLZ77TestByteSink()


void ExCodecLZ77TestByteSink::WriteByte( unsigned char ch )
{
	const int status = fputc( int( ch ), m_fp );

	ExAssert( status != EOF )
} // ExCodecLZ77TestByteSink::WriteData()


void ExCodecLZ77TestByteSink::WriteBytes( const std::vector<unsigned char> & bytes )
{
	std::vector<unsigned char>::const_iterator it = bytes.begin();
	const std::vector<unsigned char>::const_iterator itEnd = bytes.end();

	while( it != itEnd )
	{
		WriteByte( *it++ );
	}
} // ExCodecLZ77TestByteSink::WriteBytes()


void ExCodecLZ77TestByteSink::AdvanceToNextLine( void )
{
} // ExCodecLZ77TestByteSink::AdvanceToNextLine()


// **** Non-member functions ****


void LZ77Test( const std::string & srcFilename )
{
	static const unsigned int windowExponent = 10;
	ExAPAutoPtr<ExCodecHuffmanTestBitHolder> bitHolder = new ExCodecHuffmanTestBitHolder;
	ExCodecHuffmanTestBitHolder * pBitHolder = bitHolder;

	{
		ExAPAutoPtr<ExUtilByteSourceInterface> byteSource = new ExCodecLZ77TestByteSource( srcFilename );
		ExAPAutoPtr<ExUtilBitSinkInterface> bitSink = bitHolder;

		bitSink.SetResourceOwnership( false );

		ExCodecLZ77::CompressData( byteSource, bitSink, windowExponent );
	}

	{
		const std::string dstFilename = "LZ77TestOut.bin";
		ExAPAutoPtr<ExUtilBitSourceInterface> bitSource = bitHolder;
		ExAPAutoPtr<ExUtilByteSinkInterface> byteSink = new ExCodecLZ77TestByteSink( dstFilename );

		bitSource.SetResourceOwnership( false );

		bitHolder->PrepareToRead();
		ExCodecLZ77::DecompressData( bitSource, byteSink, windowExponent );
	}
} // LZ77Test()


#endif


// **** End of File ****
