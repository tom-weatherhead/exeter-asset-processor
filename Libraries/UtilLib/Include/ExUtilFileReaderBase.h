/*
 *  ExUtilFileReaderBase.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Mon Jul 14 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#ifndef _EXUTILFILEREADERBASE_H_
#define _EXUTILFILEREADERBASE_H_

#include "UtilLib\\ExUtilFileReaderBaseInterface.h"


class ExUtilFileReaderBase : public ExUtilFileReaderBaseInterface
{
protected:
	std::string m_filename;
	bool m_littleEndian;

public:
	explicit ExUtilFileReaderBase( const std::string & filename, bool littleEndian = true );
	virtual ~ExUtilFileReaderBase( void );

	virtual const std::string & GetFilename( void ) const;

	virtual void SetEndianness( bool littleEndian );

	virtual void SkipBytes( int numBytes ) const;

	virtual unsigned short GetWord( unsigned short word0, unsigned short word1 ) const;
	virtual unsigned short GetWord( unsigned char * p ) const;

	virtual unsigned int GetDoubleWord( unsigned int dword0, unsigned int dword1, unsigned int dword2, unsigned int dword3 ) const;
	virtual unsigned int GetDoubleWord( unsigned char * p ) const;

#ifdef EXUTIL_IMPLEMENT_GETFILEPOINTER
	// Remove this as soon as the JPEG code no longer needs it.
	virtual FILE * GetFilePointer( void ) const;
#endif
}; // class ExUtilFileReaderBase


#endif // #ifndef _EXUTILFILEREADERBASE_H_


// **** End of File ****
