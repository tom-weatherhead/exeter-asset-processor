/*
 *  ExUtilBitSourceSink.h
 *  Exeter Utility Library
 *
 *  Created by Tom Weatherhead on Thu May 19 2005.
 *  Copyright (c) 2005 by Tom Weatherhead. All rights reserved.
 *
 */


#ifndef _EXUTILBITSOURCESINK_H_
#define _EXUTILBITSOURCESINK_H_

// Handle little-endian (bit 0 = 1) as well as the more common big endian (bit 0 = 128) ?

#include "UtilLib\\ExUtilBitByteSourceSinkInterface.h"


//class ExUtilBitQueue


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

	virtual int ReadBits( int numBits, int & bits );	// Returns number of bits read.  Max of 31 bits.
	//virtual void ReadBits( unsigned char * buf, int numBits ) = 0;
	//virtual ExAPAutoPtr<std::vector<bool> > ReadBits( int numBits ) = 0;
	virtual bool EndOfFile( void );
}; // class ExUtilBitSource


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
	//virtual void WriteBits( unsigned char * buf, int numBits ) = 0;
	virtual void Flush( void );
}; // class ExUtilBitSink


#endif  //  #ifndef _EXUTILBITSOURCESINK_H_


// **** End of File ****
