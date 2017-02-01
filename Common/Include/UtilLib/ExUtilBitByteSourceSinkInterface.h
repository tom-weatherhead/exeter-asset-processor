/*
 *  ExUtilBitByteSourceSinkInterface.h
 *  Exeter Utility Library
 *
 *  Created by Tom Weatherhead on Thu May 19 2005.
 *  Copyright (c) 2005 by Tom Weatherhead. All rights reserved.
 *
 */


#ifndef _EXUTILBITBYTESOURCESINKINTERFACE_H_
#define _EXUTILBITBYTESOURCESINKINTERFACE_H_

#include <vector>
#include "General\\ExAPAutoPtr.h"
#include "UtilLib\\ExUtilFileReaderBaseInterface.h"
#include "UtilLib\\ExUtilFileWriterBaseInterface.h"


// Abstract base classes; interfaces only.


class ExUtilByteSourceInterface;
class ExUtilByteSinkInterface;


class ExUtilBitSourceInterface
{
public:
	// Must these sources and sinks be destroyed by the same DLL that created them?
	static ExAPAutoPtr<ExUtilBitSourceInterface> CreateBitSource( ExUtilByteSourceInterface & byteSource );

	virtual ~ExUtilBitSourceInterface( void )
	{
	}

	virtual int ReadBits( int numBits ) = 0;	// Returns the actual bits that were read.  Less safe than the following version.
	virtual int ReadBits( int numBits, int & bits )	= 0;	// Returns number of bits read.  Max of 31 bits.
	//virtual void ReadBits( unsigned char * buf, int numBits ) = 0;
	virtual bool EndOfFile( void ) = 0;
}; // class ExUtilBitSourceInterface


class ExUtilBitSinkInterface
{
public:
	static ExAPAutoPtr<ExUtilBitSinkInterface> CreateBitSink( ExUtilByteSinkInterface & byteSink );

	virtual ~ExUtilBitSinkInterface( void )
	{
	}

	virtual void WriteBits( int bits, int numBits ) = 0;
	//virtual void WriteBits( unsigned char * buf, int numBits ) = 0;
	virtual void Flush( void ) = 0;
}; // class ExUtilBitSinkInterface


class ExUtilByteSourceInterface
{
public:
	static ExAPAutoPtr<ExUtilByteSourceInterface> CreateByteSourceMemory( unsigned char * p, int bytesPerLine = 0 );
	static ExAPAutoPtr<ExUtilByteSourceInterface> CreateByteSourceStdio( const std::string & filename, int initialOffset = 0, int bytesPerLine = 0 );
	static ExAPAutoPtr<ExUtilByteSourceInterface> CreateByteSourceFile( ExUtilFileReaderBaseInterface & fileReader, int initialOffset = -1, int bytesPerLine = 0 );

	virtual ~ExUtilByteSourceInterface( void )
	{
	}

	//virtual int ReadByte( void ) = 0;	// Return int to allow return of EOF.
	virtual unsigned char ReadByte( void ) = 0;
	//virtual void ReadBytes( unsigned char * buf, int numBytes ) = 0;
	virtual void AdvanceToNextLine( void ) = 0;
	virtual bool EndOfFile( void ) = 0;
}; // class ExUtilPByteSourceInterface


class ExUtilByteSinkInterface
{
public:
	static ExAPAutoPtr<ExUtilByteSinkInterface> CreateByteSinkMemory( unsigned char * p, int bytesPerLine = 0 );
	static ExAPAutoPtr<ExUtilByteSinkInterface> CreateByteSinkStdio( const std::string & filename, int initialOffset = 0, int bytesPerLine = 0 );
	static ExAPAutoPtr<ExUtilByteSinkInterface> CreateByteSinkFile( ExUtilFileWriterBaseInterface & fileWriter, int initialOffset = -1, int bytesPerLine = 0 );

	virtual ~ExUtilByteSinkInterface( void )
	{
	}

	virtual void WriteByte( unsigned char ch ) = 0;
	virtual void WriteBytes( const std::vector<unsigned char> & bytes ) = 0;
	//virtual void WriteBytes( unsigned char * buf, int numBytes ) = 0;
	virtual void AdvanceToNextLine( void ) = 0;
}; // class ExUtilByteSinkInterface


#endif // #ifndef _EXUTILBITBYTESOURCESINKINTERFACE_H_


// **** End of File ****
