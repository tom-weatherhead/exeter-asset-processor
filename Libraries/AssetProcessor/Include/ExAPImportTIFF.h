/*
 *  ExAPImportTIFF.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Tue Jul 1 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */


#ifndef _EXAPIMPORTTIFF_H_
#define _EXAPIMPORTTIFF_H_		1


#include "ExAPBitmap.h"
#include "ExAPReadTIFFTags.h"


class ExAPTIFFReader
{
public:
	ExAPAutoPtr<ExUtilFileReaderBaseInterface> m_file;
	unsigned int m_imageFileDirectoryOffset;
	std::vector<IFDFieldType> m_IFDFields;
	ExAPTIFFTagReaderMaster m_tagReaderMaster;

	explicit ExAPTIFFReader( ExAPAutoPtr<ExUtilFileReaderBaseInterface> file ) throw( ExAPException );

	bool NextIFDExists( void ) const;
	ExAPBitmap * ImportImage( void ) throw( ExAPException );
}; // class ExAPTIFFReader


#endif // #ifndef _EXAPIMPORTTIFF_H_


// **** End of File ****
