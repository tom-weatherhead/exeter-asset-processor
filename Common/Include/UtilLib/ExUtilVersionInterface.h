// ExUtilVersionInterface.h - Derived from Helix
// Copyright (c) 2002-2005 by Tom Weatherhead.  All rights reserved.
// Started December 17, 2002

#pragma once

#include "General\\ExAPAutoPtr.h"
#include "UtilLib\\ExUtilFileReaderBaseInterface.h"
#include "UtilLib\\ExUtilFileWriterBaseInterface.h"


// **** Class ExUtilVersionInterface ****


class ExUtilVersionInterface
{
public:
	virtual ~ExUtilVersionInterface( void );

	static ExAPAutoPtr<ExUtilVersionInterface> CreateVersionObject( unsigned int v1 = 0, unsigned int v2 = 0, unsigned int v3 = 0 );
	//ExUtilVersion( FILE * pSrcFile );

	virtual void ReadFromFile( ExAPAutoPtr<ExUtilFileReaderBaseInterface> pSrcFile ) = 0;
	virtual void WriteToFile( ExAPAutoPtr<ExUtilFileWriterBaseInterface> pDstFile ) const = 0;
	//virtual void Print( void ) const = 0;

	virtual bool operator<( ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const = 0;
	virtual bool operator>( ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const = 0;
	virtual bool operator!=( ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const = 0;
	virtual bool operator==( ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const = 0;
	virtual bool operator<=( ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const = 0;
	virtual bool operator>=( ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const = 0;

	virtual unsigned int GetPrimaryVersion( void ) const = 0;
	virtual unsigned int GetSecondaryVersion( void ) const = 0;
	virtual unsigned int GetTertiaryVersion( void ) const = 0;
}; // class ExUtilVersionInterface


// **** End of File ****
