/*
 *  ExAPFileWriterBase.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Sat Jul 26 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

//#include "General\\ExAPException.h"
#include "ExAPFileWriterStdio.h"


ExAPAutoPtr<ExAPFileWriterBase> ExAPFileWriterBase::Create( const std::string & filename, bool littleEndian, bool binary ) throw( ExAPException )
{
	ExAPAutoPtr<ExAPFileWriterBase> p;

	try
	{
		p = new ExAPFileWriterStdio( filename, littleEndian, binary );
	}
	catch( ExAPException & )
	{
		throw;
	}
	catch( ... )
	{
		// Do nothing.
	}

	if( p == 0 )
	{
		ThrowExAPException( ExAPException::ResourceAcquisitionFailed );
	}

	return( p );
} // ExAPFileWriterBase::Create()


ExAPFileWriterBase::~ExAPFileWriterBase( void )
{
	// Do nothing.
} // ExAPFileWriterBase::~ExAPFileWriterBase()


// **** End of File ****
