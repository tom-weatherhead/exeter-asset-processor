/*
 *  ExCodecLZWGIF87a.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Sat Jun 28 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */


#ifndef _EXCODECLZWGIF87A_H_
#define _EXCODECLZWGIF87A_H_


#ifdef _MSC_VER			// TAW 2002/03/11 : ie. if MSVC
#ifndef MACINTOSH		// TAW 2002/02/23 : ie. if MSVC

// Suppress warning C4786: Identifier truncated to 255 chars in the debug info.
#pragma warning(disable : 4786)

#endif
#endif


#include <map>
//#include <vector>		// Already included.

#include "CodecLib\\ExCodecLZW.h"
#include "UtilLib\\ExUtilBitByteSourceSinkInterface.h"


typedef std::vector<unsigned char> ByteVector;


class ExAPLZWCodecGIF87a : public ExAPLZWCodec
{
private:
	typedef std::map< ByteVector, int > CompressionDictionaryMapType;
	typedef CompressionDictionaryMapType::const_iterator CompressionDictionaryMapConstIteratorType;

	class CompressionDictionary
	{
	public:
		CompressionDictionaryMapType m_dictMap;
		int m_nextCode;
		//int m_maxDictionarySize;	// Always 4096.
		//bool m_variableCodeSize;	// Always true.
		//int m_maxCodeSize;				// Always 12 bits.
		int m_currentCodeSize;
		int m_nextPowerOf2;		// Not needed if n & ( n - 1 ) == 0 test used?
		int m_bitsPerPixel;
		int m_codeResizingOffset;

		explicit CompressionDictionary( int bitsPerPixel, int codeResizingOffset = 0 );
		bool IsFull( void ) const;
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
		//int m_maxDictionarySize;	// Always 4096.
		//bool m_variableCodeSize;	// Always true.
		//int m_maxCodeSize;				// Always 12 bits.
		int m_currentCodeSize;
		int m_nextPowerOf2;		// Not needed if n & ( n - 1 ) == 0 test used?
		int m_bitsPerPixel;
		int m_codeResizingOffset;

		DecompressionDictionary( int bitsPerPixel, int codeResizingOffset = 0 );
		void reset( void );
		bool IsFull( void ) const;
		const ByteVector & getString( int code ) const throw( ExAPException );
		void addString( const ByteVector & str );
		bool CodeExists( int code ) const;
		int GetCurrentCodeSize( void );
	}; // class DecompressionDictionary

	const int m_bitsPerPixel;
	const int m_codeResizingOffset;

public:
	ExAPLZWCodecGIF87a( int bitsPerPixel, int codeResizingOffset );

	virtual void CompressData( ExAPAutoPtr<ExUtilByteSourceInterface> src, ExAPAutoPtr<ExUtilBitSinkInterface> sink /*, int bitsPerPixel */ );
	virtual void DecompressData( ExAPAutoPtr<ExUtilBitSourceInterface> src, ExAPAutoPtr<ExUtilByteSinkInterface> sink /*, int bitsPerPixel, int codeResizingOffset = 0 */ ) throw( ExAPException );
}; // class ExAPLZWCodecGIF87a


#endif // #ifndef _EXCODECLZWGIF87A_H_


// **** End of File ****
