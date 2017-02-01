/*
 *  ExAPLZWCodecV1.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Wed Jun 25 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

//#define TAW_TRACE		1

#ifdef TAW_TRACE
#include <cstdio>
#endif

#include "ExAPLZWCodecV1.h"
#include "ExAPException.h"


#ifdef TAW_TRACE
static void PrintCode( int n, const ByteVector & str )
{
	printf( "Code %d is: ", n );

	ByteVector::const_iterator i;

	for( i = str.begin(); i != str.end(); ++i )
	{
		printf( "%02X ", *i );
	}

	printf( "\n" );
} // ExAPLZWCodecV1::PrintCode()
#endif


static int NumSignificantBits( int x )
{

	if( x < 0 )
	{
		return( 0 );
	}

	int rtn = 0;

	while( x != 0 )
	{
		x >>= 1;
		++rtn;
	}

	return( rtn );
} // NumSignificantBits()


#ifdef CUSTOM_BYTE_VECTOR_TYPE
// **** The ByteVector class ****


ByteVector::ByteVector( void )
: m_v()
{
}


ByteVector::ByteVector( const std::vector<unsigned char> & v )
: m_v( v )
{
}


ByteVector::ByteVector( int size, unsigned char val )
: m_v( size, val )
{
}


bool ByteVector::operator<( const ByteVector & src ) const
{
	bool rtn = false;
	std::vector<unsigned char>::const_iterator i = m_v.begin();
	std::vector<unsigned char>::const_iterator iSrc = src.m_v.begin();
	const std::vector<unsigned char>::const_iterator iEnd = m_v.end();
	const std::vector<unsigned char>::const_iterator iSrcEnd = src.m_v.end();

	for( ; ; )
	{
		
		if( iSrc == iSrcEnd )
		{
			// *this == iSrc  ||  *this > iSrc
			break;
		}
		else if( i == iEnd )
		{
			rtn = true;
			break;
		}

		const unsigned char ch = *i++;
		const unsigned char chSrc = *iSrc++;

		if( ch < chSrc )
		{
			rtn = true;
			break;
		}
		else if( ch > chSrc )
		{
			break;
		}
	}

	return( rtn );
} // ByteVector::operator<()
#endif


// **** The compression dictionary class ****


ExAPLZWCodecV1::CompressionDictionary::CompressionDictionary( int maxDictionarySize, bool variableCodeSize )
	: m_nextCode( 0 ),
		m_maxDictionarySize( maxDictionarySize ),
		m_variableCodeSize( variableCodeSize ),
		m_maxCodeSize( NumSignificantBits( maxDictionarySize - 1 ) ),
		m_currentCodeSize( 0 ),
		m_nextPowerOf2( 0 )
{
	reset();
} // ExAPLZWCodecV1::CompressionDictionary::CompressionDictionary()


void ExAPLZWCodecV1::CompressionDictionary::reset( void )
{
	m_dictMap.clear();

	// "key" is constructed once, and never needs resizing.
	ByteVector key( 1, static_cast<unsigned char>( 0 ) );

	for( int i = 0; i < 256; ++i )
	{
		key[0] = static_cast<unsigned char>( i );
		m_dictMap[key] = i;
	}

	m_nextCode = 256;

	if( m_variableCodeSize )
	{
		m_nextPowerOf2 = 256;
		m_currentCodeSize = 8;
	}
	else
	{
		m_nextPowerOf2 = m_maxDictionarySize;
		m_currentCodeSize = m_maxCodeSize;
	}
} // ExAPLZWCodecV1::CompressionDictionary::reset()


int ExAPLZWCodecV1::CompressionDictionary::getCode( const ByteVector & str ) const
{
	CompressionDictionaryMapConstIteratorType i = m_dictMap.find( str );

	if( i == m_dictMap.end() )
	{
		return( -1 );
	}

	return( i->second );
} // ExAPLZWCodecV1::CompressionDictionary::getCode()


void ExAPLZWCodecV1::CompressionDictionary::addCode( const ByteVector & str )
{

	if( m_nextCode >= m_maxDictionarySize )
	{
#ifdef TAW_TRACE
		printf( "CompressionDictionary: Resetting dictionary.\n" );
#endif
		reset();
	}

#ifdef TAW_TRACE
	//printf( "CompressionDictionary: Creating code %d.\n", m_nextCode );
	PrintCode( m_nextCode, str );

	if( str.size() > 2 )
	{
		printf( "This code is longer than 2 bytes.\n" );
	}

	if( m_nextCode == 269 )
	{
		printf( "Compress: Adding code %d to the dictionary.\n", m_nextCode );
	}
#endif

	m_dictMap[str] = m_nextCode++;
} // ExAPLZWCodecV1::CompressionDictionary::addCode()


int ExAPLZWCodecV1::CompressionDictionary::GetCurrentCodeSize( void )
{

	if( m_variableCodeSize )
	{
		const int dictionarySize = m_dictMap.size();
		
		if( dictionarySize > m_nextPowerOf2 )
		{
			++m_currentCodeSize;
			m_nextPowerOf2 *= 2;
		}
	}

	return( m_currentCodeSize );
} // ExAPLZWCodecV1::CompressionDictionary::GetCurrentCodeSize()


#ifdef TAW_TRACE
const ByteVector & ExAPLZWCodecV1::CompressionDictionary::getString( int i ) const
{
	static const ByteVector dflt;

	CompressionDictionaryMapConstIteratorType it;

	for( it = m_dictMap.begin(); it != m_dictMap.end(); ++it )
	{
		
		if( it->second == i )
		{
			return( it->first );
		}
	}

	return( dflt );
}
#endif


// **** The decompression dictionary class ****


ExAPLZWCodecV1::DecompressionDictionary::DecompressionDictionary( int maxDictionarySize, bool variableCodeSize )
	: m_maxDictionarySize( maxDictionarySize ),
		m_variableCodeSize( variableCodeSize ),
		m_maxCodeSize( NumSignificantBits( maxDictionarySize - 1 ) ),
		m_currentCodeSize( 0 ),
		m_nextPowerOf2( 0 )
{
	reset();
} // ExAPLZWCodecV1::DecompressionDictionary::DeompressionDictionary()


void ExAPLZWCodecV1::DecompressionDictionary::reset( void )
{
	m_dictVector.clear();

	// "key" is constructed once, and never needs resizing.
	ByteVector key( 1, static_cast<unsigned char>( 0 ) );

	for( int i = 0; i < 256; ++i )
	{
		key[0] = static_cast<unsigned char>( i );
		m_dictVector.push_back( key );
	}

	if( m_variableCodeSize )
	{
		m_nextPowerOf2 = 256;
		m_currentCodeSize = 8;
	}
	else
	{
		m_nextPowerOf2 = m_maxDictionarySize;
		m_currentCodeSize = m_maxCodeSize;
	}
} // ExAPLZWCodecV1::DecompressionDictionary::reset()


bool ExAPLZWCodecV1::DecompressionDictionary::CodeExists( int code ) const
{
	return( code >= 0  &&  code < m_dictVector.size() );
} // ExAPLZWCodecV1::DecompressionDictionary::CodeExists()


const ByteVector & ExAPLZWCodecV1::DecompressionDictionary::getString( int code ) const
{
	// TO_DO: If code is out of range, throw an exception.

	if( !CodeExists( code ) )
	{
#if 1 //def TAW_TRACE
		printf( "DecompressionDictionary::getString(): Exception.\n" );
		printf( "  Requested code: %d.\n", code );
		printf( "  Dictionary size: %d.\n", m_dictVector.size() );
		printf( "  Current code size: %d.\n", m_currentCodeSize );
#endif
		ThrowExAPException( ExAPException::LZWError );
	}

	return( m_dictVector[code] );
} // ExAPLZWCodecV1::DecompressionDictionary::getString()


void ExAPLZWCodecV1::DecompressionDictionary::addString( const ByteVector & str )
{
	const int nextCode = m_dictVector.size();

	if( nextCode >= m_maxDictionarySize )
	{
#ifdef TAW_TRACE
		printf( "CompressionDictionary: Resetting dictionary.\n" );
#endif
		reset();
	}
#if 0
	else if( nextCode >= m_nextPowerOf2 )
	{
		++m_currentCodeSize;
		m_nextPowerOf2 *= 2;
	}
#endif

	m_dictVector.push_back( str );
} // ExAPLZWCodecV1::DecompressionDictionary::addString()


int ExAPLZWCodecV1::DecompressionDictionary::GetCurrentCodeSize( bool firstTime )
{
	//const int rtn = m_currentCodeSize;

	if( !m_variableCodeSize )
	{
		// Do nothing.
	}
	else if( firstTime )
	{
		m_currentCodeSize = 8;
	}
	else if( m_dictVector.size() >= m_nextPowerOf2 )
	{
		// A necessary hack.
		// The decompressor reads two codes before adding the first string to the dictionary.
		++m_currentCodeSize;
		m_nextPowerOf2 *= 2;

		// Another nasty kludge.

		if( m_currentCodeSize > m_maxCodeSize )
		{
			m_currentCodeSize = 9;
		}
	}

	return( m_currentCodeSize );
} // ExAPLZWCodecV1::DecompressionDictionary::GetCurrentCodeSize()


// **** The LZW compressor/decompressor class ****


void ExAPLZWCodecV1::CompressData( ExAPByteSource & src, ExAPBitSink & sink, int maxDictionarySize, bool variableCodeSize )
{
	CompressionDictionary dictionary( maxDictionarySize, variableCodeSize );
	unsigned char c = src.ReadByte();
	ByteVector str;	// ( 1, c );

	if( src.EndOfFile() )
	{
		// There was no data in the source stream.
		return;
	}

	str.push_back( c );

	// Main loop.

	for( ; ; )
	{
		c = src.ReadByte();

		if( src.EndOfFile() )
		{
			break;
		}

		ByteVector strPlusC( str );

		strPlusC.push_back( c );

		// Is string+character in the dictionary?

		if( dictionary.getCode( strPlusC ) >= 0 )
		{
			// Yes, it is.
			str = strPlusC;
		}
		else
		{
			// No, it isn't.
			// Output the code for string.
			// 12 bits per code.  We could make this variable-length.
			const int code = dictionary.getCode( str );
#ifdef TAW_TRACE
			const ByteVector & testStr = dictionary.getString( code );

			printf( "Compress: Writing code %d to the bit stream.\n", code );
			PrintCode( code, str );

			if( str.size() <= 0  ||  testStr != str )
			{
				printf( "Compress: Inconsistency found.\n" );
			}
#endif
			sink.WriteBits( code, dictionary.GetCurrentCodeSize() );

			// Add string+character to the dictionary.
			// This may reset the dictionary, so codes will be invalidated.
			dictionary.addCode( strPlusC );

			// string = character.
			str.clear();
			str.push_back( c );
		}
	}

	// Output the code for the final string.
	// 12 bits per code.  We could make this variable-length.
	sink.WriteBits( dictionary.getCode( str ), dictionary.GetCurrentCodeSize() );
} // ExAPLZWCodecV1::CompressData()


void ExAPLZWCodecV1::DecompressData( ExAPBitSource & src, ExAPByteSink & sink, int maxDictionarySize, bool variableCodeSize )
{
	DecompressionDictionary dictionary( maxDictionarySize, variableCodeSize );
	int oldCode = src.ReadBits( dictionary.GetCurrentCodeSize( true ) );
	ByteVector oldCodeString = dictionary.getString( oldCode );
	unsigned char ch = oldCodeString[0];

#ifdef CUSTOM_BYTE_VECTOR_USE_GETVECTOR
	sink.WriteBytes( oldCodeString.GetVector() );
#else
	sink.WriteBytes( oldCodeString );
#endif

	for( ; ; )
	{
		const int newCode = src.ReadBits( dictionary.GetCurrentCodeSize( false ) );

		if( src.EndOfFile() )
		{
			break;
		}

#ifdef TAW_TRACE
		printf( "Decompress: Read code %d.\n", newCode );
#endif

		ByteVector oldCodeStr = dictionary.getString( oldCode );
		ByteVector str;

		if( !dictionary.CodeExists( newCode ) )
		{
#ifdef TAW_TRACE
			printf( "Decompress: The new code does not exist.\n" );
#endif
			str = oldCodeStr;
			str.push_back( ch );
		}
		else
		{
#ifdef TAW_TRACE
			printf( "Decompress: The new code exists.\n" );
#endif
			str = dictionary.getString( newCode );
#ifdef TAW_TRACE
			PrintCode( newCode, str );
#endif
		}

#ifdef CUSTOM_BYTE_VECTOR_USE_GETVECTOR
		sink.WriteBytes( str.GetVector() );
#else
		sink.WriteBytes( str );
#endif

		ch = str[0];

		// Add oldCode + ch to the dictionary.
		oldCodeStr.push_back( ch );
		dictionary.addString( oldCodeStr );

		oldCode = newCode;
	}
} // ExAPLZWCodecV1::DecompressData()


// **** End of File ****
