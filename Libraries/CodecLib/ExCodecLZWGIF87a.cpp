/*
 *  ExCodecLZWGIF87a.cpp
 *  Exeter Codec Library
 *
 *  Created by Tom Weatherhead on Sat Jun 28 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

//#define TAW_TRACE		1

#if 1 //def TAW_TRACE
#include <cstdio>
#endif

//lint -e1050 : Expected a template argument list '<...>' for template 'std::iterator<<1>,<2>,<3>>'.

#include "General\\ExAPMinMax.h"
#include "ExCodecLZWGIF87a.h"


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


#if 0
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
#endif


// **** The compression dictionary class ****


ExAPLZWCodecGIF87a::CompressionDictionary::CompressionDictionary( int bitsPerPixel, int codeResizingOffset )
	: m_nextCode( 0 ),
		m_currentCodeSize( 0 ),
		m_nextPowerOf2( 0 ),
		m_bitsPerPixel( bitsPerPixel ),
		m_codeResizingOffset( codeResizingOffset )
{
	reset();
} // ExAPLZWCodecGIF87a::CompressionDictionary::CompressionDictionary()


bool ExAPLZWCodecGIF87a::CompressionDictionary::IsFull( void ) const
{
	return( m_nextCode >= 4095 );
} // ExAPLZWCodecGIF87a::CompressionDictionary::IsFull()


void ExAPLZWCodecGIF87a::CompressionDictionary::reset( void )
{
	m_currentCodeSize = max( m_bitsPerPixel, 2 ) + 1;
	m_nextCode = ( 1 << ( m_currentCodeSize - 1 ) ) + 2;
	m_nextPowerOf2 = 1 << m_currentCodeSize;

	// Code ( m_nextCode - 2 ) is the clear code.
	// Code ( m_nextCode - 1 ) is the end-of-information code.

	// "key" is constructed once, and never needs resizing.
	ByteVector key( 1, static_cast<unsigned char>( 0 ) );

	m_dictMap.clear();

	for( int i = 0; i < m_nextCode - 2; ++i )
	{
		key[0] = static_cast<unsigned char>( i );
		m_dictMap[key] = i;
	}
} // ExAPLZWCodecGIF87a::CompressionDictionary::reset()


int ExAPLZWCodecGIF87a::CompressionDictionary::getCode( const ByteVector & str ) const
{
	CompressionDictionaryMapConstIteratorType i = m_dictMap.find( str );

	if( i == m_dictMap.end() )
	{
		return( -1 );
	}

	return( i->second );
} // ExAPLZWCodecGIF87a::CompressionDictionary::getCode()


void ExAPLZWCodecGIF87a::CompressionDictionary::addCode( const ByteVector & str )
{

#if 0		// The compression function must emit a clear code and call reset().
	if( m_nextCode >= m_maxDictionarySize )
	{
#ifdef TAW_TRACE
		printf( "CompressionDictionary: Resetting dictionary.\n" );
#endif
		reset();
	}
#endif

#ifdef TAW_TRACE
	PrintCode( m_nextCode, str );
#endif

	m_dictMap[str] = m_nextCode++;
} // ExAPLZWCodecGIF87a::CompressionDictionary::addCode()


int ExAPLZWCodecGIF87a::CompressionDictionary::GetCurrentCodeSize( void )
{
#if 1
	if( m_currentCodeSize < 12  &&  m_nextCode + m_codeResizingOffset > m_nextPowerOf2 )
#else
	const int dictionarySize = int( m_dictMap.size() );
		
	if( dictionarySize + 1 >= m_nextPowerOf2 )
#endif
	{
		++m_currentCodeSize;
		m_nextPowerOf2 *= 2;
	}

	return( m_currentCodeSize );
} // ExAPLZWCodecGIF87a::CompressionDictionary::GetCurrentCodeSize()


#ifdef TAW_TRACE
const ByteVector & ExAPLZWCodecGIF87a::CompressionDictionary::getString( int i ) const
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


ExAPLZWCodecGIF87a::DecompressionDictionary::DecompressionDictionary( int bitsPerPixel, int codeResizingOffset )
	: m_currentCodeSize( 0 ),
		m_nextPowerOf2( 0 ),
		m_bitsPerPixel( bitsPerPixel ),
		m_codeResizingOffset( codeResizingOffset )
{
	reset();
} // ExAPLZWCodecGIF87a::DecompressionDictionary::DeompressionDictionary()


void ExAPLZWCodecGIF87a::DecompressionDictionary::reset( void )
{
	m_currentCodeSize = max( m_bitsPerPixel, 2 ) + 1;
	m_nextPowerOf2 = 1 << m_currentCodeSize;

	const int nextCode = ( 1 << ( m_currentCodeSize - 1 ) ) + 2;

	// Code ( nextCode - 2 ) is the clear code.
	// Code ( nextCode - 1 ) is the end-of-information code.

	// "key" is constructed once, and never needs resizing.
	ByteVector key( 1, static_cast<unsigned char>( 0 ) );

	m_dictVector.clear();

	for( int i = 0; i < nextCode; ++i )
	{
		key[0] = static_cast<unsigned char>( i );
		m_dictVector.push_back( key );
	}

#ifdef TAW_TRACE
	printf( "Decompression dictionary has been reset.\n" );
	printf( "  Clear code: %d.\n", nextCode - 2 );
	printf( "  End of Information code: %d.\n", nextCode - 1 );
#endif
} // ExAPLZWCodecGIF87a::DecompressionDictionary::reset()


bool ExAPLZWCodecGIF87a::DecompressionDictionary::IsFull( void ) const
{
	return( m_dictVector.size() >= 4096 );
} // ExAPLZWCodecGIF87a::DecompressionDictionary::IsFull()


bool ExAPLZWCodecGIF87a::DecompressionDictionary::CodeExists( int code ) const
{
	return( code >= 0  &&  code < int( m_dictVector.size() ) );
} // ExAPLZWCodecGIF87a::DecompressionDictionary::CodeExists()


const ByteVector & ExAPLZWCodecGIF87a::DecompressionDictionary::getString( int code ) const throw( ExAPException )
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

	return( m_dictVector[ std::vector<ByteVector>::size_type( code ) ] );
} // ExAPLZWCodecGIF87a::DecompressionDictionary::getString()


void ExAPLZWCodecGIF87a::DecompressionDictionary::addString( const ByteVector & str )
{

	if( IsFull() )
	{
		// Do nothing.
		return;
	}

#if 0	// The code size is lengthened by GetCurrentCodeSize().

	if( m_dictVector.size() >= m_nextPowerOf2 )
	{
		++m_currentCodeSize;
		m_nextPowerOf2 *= 2;
#ifdef TAW_TRACE
		printf( "AddString: Increasing code size to %d bits.\n", m_currentCodeSize );
#endif
	}
#endif

	m_dictVector.push_back( str );
} // ExAPLZWCodecGIF87a::DecompressionDictionary::addString()


int ExAPLZWCodecGIF87a::DecompressionDictionary::GetCurrentCodeSize( void )
{
	const int nextCode = int( m_dictVector.size() );

	if( m_currentCodeSize < 12  &&  nextCode + m_codeResizingOffset >= m_nextPowerOf2 )
	{
		// A necessary hack.
		// The decompressor reads two codes before adding the first string to the dictionary.
		++m_currentCodeSize;
		m_nextPowerOf2 *= 2;
#ifdef TAW_TRACE
		printf( "GetCurrentCodeSize: Increasing code size to %d bits.\n", m_currentCodeSize );
#endif
	}

	return( m_currentCodeSize );
} // ExAPLZWCodecGIF87a::DecompressionDictionary::GetCurrentCodeSize()


// **** The LZW compressor/decompressor class ****


ExAPLZWCodecGIF87a::ExAPLZWCodecGIF87a( int bitsPerPixel, int codeResizingOffset )
	: m_bitsPerPixel( bitsPerPixel ),
		m_codeResizingOffset( codeResizingOffset )
{
} // ExAPLZWCodecGIF87a::ExAPLZWCodecGIF87a()


void ExAPLZWCodecGIF87a::CompressData( ExAPAutoPtr<ExUtilByteSourceInterface> src, ExAPAutoPtr<ExUtilBitSinkInterface> sink /*, int bitsPerPixel */ )
{
	CompressionDictionary dictionary( m_bitsPerPixel, m_codeResizingOffset );
	const int clearCode = 1 << max( m_bitsPerPixel, 2 );
	const int endOfInformationCode = clearCode + 1;

	sink->WriteBits( clearCode, dictionary.GetCurrentCodeSize() );

	unsigned char c = src->ReadByte();
	ByteVector str;	// ( 1, c );

	if( src->EndOfFile() )
	{
		// There was no data in the source stream.
		return;
	}

	str.push_back( c );

	// Main loop.

	for( ; ; )
	{
		c = src->ReadByte();

		if( src->EndOfFile() )
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
			sink->WriteBits( code, dictionary.GetCurrentCodeSize() );

			// Add string+character to the dictionary.
			// This may reset the dictionary, so codes will be invalidated.
			dictionary.addCode( strPlusC );

			if( dictionary.IsFull() )
			{
				sink->WriteBits( clearCode, dictionary.GetCurrentCodeSize() );
				dictionary.reset();
			}

			// string = character.
			str.clear();
			str.push_back( c );
		}
	}

	// Output the code for the final string.
	// 12 bits per code.  We could make this variable-length.
	sink->WriteBits( dictionary.getCode( str ), dictionary.GetCurrentCodeSize() );
	sink->WriteBits( endOfInformationCode, dictionary.GetCurrentCodeSize() );
	sink->Flush();
} // ExAPLZWCodecGIF87a::CompressData()


void ExAPLZWCodecGIF87a::DecompressData( ExAPAutoPtr<ExUtilBitSourceInterface> src, ExAPAutoPtr<ExUtilByteSinkInterface> sink /*, int bitsPerPixel, int codeResizingOffset */ ) throw( ExAPException )
{
	DecompressionDictionary dictionary( m_bitsPerPixel, m_codeResizingOffset );
	const int clearCode = 1 << max( m_bitsPerPixel, 2 );
	const int endOfInformationCode = clearCode + 1;
	int oldCode = 0;
	unsigned char ch = 0;
	bool justReadClearCode = true;

	for( ; ; )
	{
		
		if( justReadClearCode )
		{

			do
			{
				oldCode = src->ReadBits( dictionary.GetCurrentCodeSize() );

#ifdef TAW_TRACE
				printf( "oldCode == %d\n", oldCode );
#endif

#ifdef TAW_TRACE
				if( oldCode == clearCode )
				{
					printf( "Decompress: Clear code received (X).\n" );
				}
#endif
			}
			while( oldCode == clearCode );

			if( oldCode == endOfInformationCode )
			{
#ifdef TAW_TRACE
				printf( "Decompress: End of Information code received (following a clear code).\n" );
				printf( "  Final dictionary size: %d.\n", dictionary.m_dictVector.size() );
#endif
				break;
			}

			const ByteVector oldCodeString = dictionary.getString( oldCode );

			ch = oldCodeString[0];
			sink->WriteBytes( oldCodeString );
			justReadClearCode = false;
		}

		const int newCode = src->ReadBits( dictionary.GetCurrentCodeSize() );

		if( src->EndOfFile()  ||  newCode < 0 )
		{
#ifdef TAW_TRACE
			printf( "Decompress: End of SourceBits.\n" );
#endif
			break;
		}
		else if( newCode == endOfInformationCode )
		{
#ifdef TAW_TRACE
			printf( "Decompress: End of Information code received.\n" );
			printf( "  Final dictionary size: %d.\n", dictionary.m_dictVector.size() );
#endif
			break;
		}
		else if( newCode == clearCode )
		{
#if 1 //def TAW_TRACE
			printf( "Decompress: Clear code received.\n" );
#endif
			dictionary.reset();
			justReadClearCode = true;
			continue;
		}

#ifdef TAW_TRACE
		printf( "Decompress: Read code %d.\n", newCode );
		printf( "Decompress: Looking up code %d in dictionary.\n", oldCode );
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

		sink->WriteBytes( str );

		ch = str[0];

		// Add oldCode + ch to the dictionary.
		oldCodeStr.push_back( ch );
		dictionary.addString( oldCodeStr );

		oldCode = newCode;
	}
} // ExAPLZWCodecGIF87a::DecompressData()


// **** End of File ****
