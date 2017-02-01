/*
 *  ExCodecBitByteSourceSink.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Wed Jun 25 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */


#ifndef _EXAPBITBYTESOURCESINK_H_
#define _EXAPBITBYTESOURCESINK_H_

#include <vector>


// Abstract base classes; interfaces only.


class ExAPBitSource
{
public:

	virtual ~ExAPBitSource( void )
	{
	}

	virtual int ReadBits( int numBits ) = 0;	// Max of 31 bits.
	//virtual void ReadBits( unsigned char * buf, int numBits ) = 0;
	virtual bool EndOfFile( void ) = 0;
}; // class ExAPBitSource


class ExAPBitSink
{
public:

	virtual ~ExAPBitSink( void )
	{
	}

	virtual void WriteBits( int bits, int numBits ) = 0;
	virtual void Flush( void ) = 0;
	//virtual void WriteBits( unsigned char * buf, int numBits ) = 0;
}; // class ExAPBitSink


class ExAPByteSource
{
public:

	virtual ~ExAPByteSource( void )
	{
	}

	//virtual int ReadByte( void ) = 0;	// Return int to allow return of EOF.
	virtual unsigned char ReadByte( void ) = 0;
	//virtual void ReadBytes( unsigned char * buf, int numBytes ) = 0;
	virtual bool EndOfFile( void ) = 0;
}; // class ExAPByteSource


class ExAPByteSink
{
public:

	virtual ~ExAPByteSink( void )
	{
	}

	virtual void WriteByte( unsigned char ch ) = 0;
	virtual void WriteBytes( const std::vector<unsigned char> & bytes ) = 0;
	//virtual void WriteBytes( unsigned char * buf, int numBytes ) = 0;
}; // class ExAPByteSink


#endif // #ifndef _EXAPBITBYTESOURCESINK_H_


// **** End of File ****
