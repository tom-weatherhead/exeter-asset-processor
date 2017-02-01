/*
 *  ExAPFile.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Tue Jul 08 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#include "General\\ExAPException.h"
#include "ExAPFile.h"


ExAPFile::ExAPFile( const char * filename, const char * flags ) throw( ExAPException )
	: m_filename( filename ),		// What happens if filename == 0?
		m_fp( fopen( filename, flags ) )
{

	if( m_fp == 0 )
	{
		ThrowExAPException( ExAPException::FileOpenFailed );
	}
} // ExAPFile::ExAPFile()


ExAPFile::~ExAPFile( void )
{

	if( m_fp != 0 )
	{
		fclose( m_fp );
		m_fp = 0;
	}
} // ExAPFile::~ExAPFile()


// **** End of File ****
