/*
 *  ExCodecTestBitHolder.h
 *  Exeter Codec Library
 *
 *  Created by Tom Weatherhead on Thu Jul 31 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#ifndef _EXCODECTESTBITHOLDER_H_
#define _EXCODECTESTBITHOLDER_H_		1

#include <vector>

//#include "CodecLib\\ExCodecBitByteSourceSink.h"
#include "UtilLib\\ExUtilBitByteSourceSinkInterface.h"


class ExCodecHuffmanTestBitHolder : public ExUtilBitSourceInterface, public ExUtilBitSinkInterface
{
private:
	std::vector<unsigned char> m_vec;
	std::vector<unsigned char>::const_iterator m_readHead;
	unsigned int m_buffer;
	int m_numBitsInBuffer;
	bool m_EOF;

public:

	ExCodecHuffmanTestBitHolder( void );

	// Bit Source functions.
	void PrepareToRead( void );
	virtual int ReadBits( int numBits );	// Max of 31 bits.
	virtual int ReadBits( int numBits, int & bits );	// Returns number of bits read.  Max of 31 bits.
	virtual bool EndOfFile( void );

	// Bit Sink functions.
	virtual void WriteBits( int bits, int numBits );
	virtual void Flush( void );
}; // class ExCodecHuffmanTestBitSink


#endif // #ifndef _EXCODECTESTBITHOLDER_H_


// **** End of File ****
