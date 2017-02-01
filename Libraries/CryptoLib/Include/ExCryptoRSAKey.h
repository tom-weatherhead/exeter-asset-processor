// RSAKey.h - Part of Helix
// Copyright (c) 2002 by Tom Weatherhead.  All rights reserved.
// Started December 17, 2002

#pragma once

//#ifndef _RSAKEY_H_
//#define _RSAKEY_H_

#include "ExCryptoCommon.h"
//#include "UtilLib\\ExUtilVersionInterface.h"


class CHelixRSAKey
{
private:
	ExAPAutoPtr<ExUtilVersionInterface> m_version;
	bool m_bIsPrivate;
	ExCryptoBigNumFixedLength m_exponent;
	ExCryptoBigNumFixedLength m_n;

public:
	CHelixRSAKey( bool bIsPrivate, ExAPAutoPtr<ExUtilVersionInterface> v, ExCryptoBigNumFixedLength & e, ExCryptoBigNumFixedLength & n );
	CHelixRSAKey( FILE * pSrcFile );
	void WriteToFile( FILE * pDstFile ) const;
	static void HashNumbers( ExCryptoBigNumFixedLength & d, ExCryptoBigNumFixedLength & n );
	
	inline const ExCryptoBigNumFixedLength & GetExponent( void ) const
	{
		return( m_exponent );
	}

	inline const ExCryptoBigNumFixedLength & GetN( void ) const
	{
		return( m_n );
	}
}; // class CHelixRSAKey


//#endif // _RSAKEY_H_


// **** End of File ****
