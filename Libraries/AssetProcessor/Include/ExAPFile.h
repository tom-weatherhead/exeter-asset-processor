/*
 *  ExAPFile.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Tue Jul 08 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#ifndef _EXAPFILE_H_
#define _EXAPFILE_H_

#include <cstdio>
#include <string>

//#include "ExAPException.h"


class ExAPFile
{
private:
	const std::string m_filename;
	FILE * m_fp;

public:
	ExAPFile( const char * filename, const char * flags ) throw( ExAPException );
	~ExAPFile( void );

	inline const std::string & GetFilename( void ) const
	{
		return( m_filename );
	}

	inline FILE * GetFilePointer( void ) const
	{
		return( m_fp );
	}
}; // class ExAPFile


#endif // #ifndef _EXAPFILE_H_


// **** End of File ****
