/*
 *  ExAPLZWCodecV1.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Wed Jun 25 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */


#ifndef _EXAPLZWCODECV1_H_
#define _EXAPLZWCODECV1_H_


#ifdef _MSC_VER			// TAW 2002/03/11 : ie. if MSVC
#ifndef MACINTOSH		// TAW 2002/02/23 : ie. if MSVC

// Suppress warning C4786: Identifier truncated to 255 chars in the debug info.
#pragma warning(disable : 4786)

#endif
#endif


#include <map>
//#include <vector>		// Already included.

#include "ExAPLZWCodec.h"


//#define CUSTOM_BYTE_VECTOR_TYPE		1
//#define CUSTOM_BYTE_VECTOR_USE_GETVECTOR		1


#ifndef CUSTOM_BYTE_VECTOR_TYPE
typedef std::vector<unsigned char> ByteVector;
#else
class ByteVector
{
public:
	typedef std::vector<unsigned char>::const_iterator const_iterator;

	std::vector<unsigned char> m_v;

	ByteVector( void );
	ByteVector( const std::vector<unsigned char> & v );
	ByteVector( int size, unsigned char val );
	bool operator<( const ByteVector & src ) const;

	inline bool operator!=( const ByteVector & src ) const
	{
		return( *this < src  ||  src < *this );
	}

	inline const_iterator begin( void ) const
	{
		return( m_v.begin() );
	}

	inline const_iterator end( void ) const
	{
		return( m_v.end() );
	}

	inline unsigned char & operator[]( int i )
	{
		return( m_v[i] );
	}

	inline const unsigned char & operator[]( int i ) const
	{
		return( m_v[i] );
	}

	inline int size( void ) const
	{
		return( m_v.size() );
	}

	inline void push_back( unsigned char c )
	{
		m_v.push_back( c );
	}

	inline void clear( void )
	{
		m_v.clear();
	}

#if 1
	inline operator const std::vector<unsigned char> &( void ) const
#else
	inline const std::vector<unsigned char> & GetVector( void ) const
#endif
	{
		return( m_v );
	}
}; // class ByteVector
#endif


class ExAPLZWCodecV1 : public ExAPLZWCodec
{
private:
	typedef std::map< ByteVector, int > CompressionDictionaryMapType;
	typedef CompressionDictionaryMapType::const_iterator CompressionDictionaryMapConstIteratorType;

	class CompressionDictionary
	{
	public:
		CompressionDictionaryMapType m_dictMap;
		int m_nextCode;
		int m_maxDictionarySize;	// eg. 4096
		bool m_variableCodeSize;
		int m_maxCodeSize;
		int m_currentCodeSize;
		int m_nextPowerOf2;

		explicit CompressionDictionary( int maxDictionarySize, bool variableCodeSize = false );
		void reset( void );
		int getCode( const ByteVector & str ) const;
		void addCode( const ByteVector & str );
		int GetCurrentCodeSize( void );

#ifdef TAW_TRACE
		const ByteVector & getString( int i ) const;
#endif
	}; // class CompressionDictionary

	class DecompressionDictionary
	{
	public:
		std::vector<ByteVector> m_dictVector;
		int m_maxDictionarySize;	// eg. 4096
		bool m_variableCodeSize;
		int m_maxCodeSize;
		int m_currentCodeSize;
		int m_nextPowerOf2;

		DecompressionDictionary( int maxDictionarySize, bool variableCodeSize = false );
		void reset( void );
		const ByteVector & getString( int code ) const;
		void addString( const ByteVector & str );
		bool CodeExists( int code ) const;
		int GetCurrentCodeSize( bool firstTime );
	}; // class DecompressionDictionary

public:
	virtual void CompressData( ExAPByteSource & src, ExAPBitSink & sink, int maxDictionarySize = 4096, bool variableCodeSize = false );
	virtual void DecompressData( ExAPBitSource & src, ExAPByteSink & sink, int maxDictionarySize = 4096, bool variableCodeSize = false );
}; // class ExAPLZWCodecV1


#endif // #ifndef _EXAPLZWCODECV1_H_


// **** End of File ****
