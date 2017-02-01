/*
 *  ExAPBitByteSourceSinkV1.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Wed Jun 25 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */


#ifndef _EXAPBITBYTESOURCESINKV1_H_
#define _EXAPBITBYTESOURCESINKV1_H_

#include <cstdio>
#include "ExAPBitByteSourceSink.h"


class ExAPBitSourceV1 : public ExAPBitSource
{
public:
	FILE * m_fp;
	bool m_closeFileWhenFinished;
	int m_buffer;
	int m_numBitsInBuffer;
	int m_lastNumBitsRead;
	int m_numChunksOfSameLengthRead;

	ExAPBitSourceV1( const char * filename );
	virtual ~ExAPBitSourceV1( void );

	virtual int ReadBits( int numBits );	// Max of 31 bits.
	//virtual void ReadBits( unsigned char * buf, int numBits ) = 0;
	virtual bool EndOfFile( void );
}; // class ExAPBitSourceV1


class ExAPBitSinkV1 : public ExAPBitSink
{
public:
	FILE * m_fp;
	bool m_closeFileWhenFinished;
	int m_buffer;
	int m_numBitsInBuffer;
	int m_lastNumBitsWritten;
	int m_numChunksOfSameLengthWritten;

	ExAPBitSinkV1( const char * filename );
	virtual ~ExAPBitSinkV1( void );

	virtual void WriteBits( int bits, int numBits );
	//virtual void WriteBits( unsigned char * buf, int numBits ) = 0;
	virtual void Flush( void );
}; // class ExAPBitSinkV1


class ExAPByteSourceV1 : public ExAPByteSource
{
public:
	FILE * m_fp;
	bool m_closeFileWhenFinished;
	int m_bytesRead;

	ExAPByteSourceV1( const char * filename );
	virtual ~ExAPByteSourceV1( void );

	//virtual int ReadByte( void ) = 0;	// Return int to allow return of EOF.
	virtual unsigned char ReadByte( void );
	//virtual void ReadBytes( unsigned char * buf, int numBytes ) = 0;
	virtual bool EndOfFile( void );
}; // class ExAPByteSourceV1


class ExAPByteSinkV1 : public ExAPByteSink
{
public:
	FILE * m_fp;
	bool m_closeFileWhenFinished;
	int m_bytesWritten;

	ExAPByteSinkV1( const char * filename );
	virtual ~ExAPByteSinkV1( void );

	virtual void WriteBytes( const std::vector<unsigned char> & bytes );
	//virtual void WriteBytes( unsigned char * buf, int numBytes ) = 0;
}; // class ExAPByteSinkV1


#endif // #ifndef _EXAPBITBYTESOURCESINKV1_H_


// **** End of File ****
