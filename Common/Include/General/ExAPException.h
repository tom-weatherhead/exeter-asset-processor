/*
 *  ExAPException.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Tue May 06 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#ifndef _EXAPEXCEPTION_H_
#define _EXAPEXCEPTION_H_

#include <string>


class ExAPException
{
public:

	enum Type
	{
		UnknownException,
		ArrayIndexOutOfBounds,
		AssertOrSignal,
		DocumentCreationError,
		DynamicCastFailed,
		EmptyPalette,
		FileOpenFailed,
		FileSeekFailed,
		IllegalFunctionCall,
		IllegalParameterValue,
		LZWError,
		ResourceAcquisitionFailed,
		SystemCallFailed,
		UnsupportedColourModel,
		PaletteIsFull,
		CantAddToNonCustomPalette,
		UnimplementedFunction,
		FileReadFailed
	};
	
private:

	Type m_type;
	const char * m_file;
	int m_line;
	const std::string m_msg;

public:

	// Constructor.
	ExAPException( Type type, const char * file, int line, const std::string & msg )
		: m_type( type ), m_file( file ), m_line( line ), m_msg( msg )
	{
	}

	inline const char * getFile( void ) const
	{
		return( m_file );
	}

	inline int getLine( void ) const
	{
		return( m_line );
	}

	inline const std::string & getMsg( void ) const
	{
		return( m_msg );
	}
}; // class ExAPException


#define ThrowExAPException(e)						throw(ExAPException(e,__FILE__,__LINE__,"No comment"))
#define ThrowExAPExceptionWithMsg(e,m)	throw(ExAPException(e,__FILE__,__LINE__,m))


// Signals and asserts.

#if defined(_DEBUG)  &&  defined(_MSC_VER)  &&  !defined(MACINTOSH)
static void SystemSignal( void )
{
	// This only works on the Intel x86 architectures.
	__asm int 3
}
#else
#define SystemSignal() ((void)0)
#endif

#define ExSignal()		{ SystemSignal(); ThrowExAPException(ExAPException::AssertOrSignal); }

#define ExAssert(x)		if( !(x) ) ExSignal()

#endif


// **** End of File ****
