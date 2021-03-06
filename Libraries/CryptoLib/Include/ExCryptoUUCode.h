// ExCryptoUUCode.h - Part of Helix
// Copyright (c) 2002 by Tom Weatherhead.  All rights reserved.
// Started December 16, 2002


#pragma once

//#ifndef __UUCODE_H_
//#define __UUCODE_H_

//void UUEncode( char * pSrcFilename, FILE * pSrcFile, FILE * pDstFile );
//bool UUDecode( FILE * pSrcFile );

namespace ExCryptoUUCode
{
	void UUEncode( const std::string & SrcFilenameStdStr, ExAPAutoPtr<ExUtilFileReaderBaseInterface> pSrcFile, ExAPAutoPtr<ExUtilFileWriterBaseInterface> pDstFile );
	bool UUDecode( ExAPAutoPtr<ExUtilFileReaderBaseInterface> pSrcFile );
};


//#endif


// **** End of File ****
