/*
 *  ExAPBitmap8BitPaletted.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Sun Jun 22 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#ifndef _EXAPBITMAP8BITPALETTED_H_
#define _EXAPBITMAP8BITPALETTED_H_

//#include "ExAPBitmap8bpchNoAlpha.h"
#include "ExAPBitmapPaletted.h"


class ExAPRead8BitPalettedImageFileAsRGB24 : public ExAPReadPalettedImageFileAsRGB24
{
public:
	ExAPRead8BitPalettedImageFileAsRGB24( ExAPAutoPtr<ExUtilFileReaderBaseInterface> pFile, const ExAPBitmapPaletted & image, int fileRowAlignmentInBytes );
	virtual ~ExAPRead8BitPalettedImageFileAsRGB24( void );

	virtual void GetNextPixelFromFile( unsigned char & r, unsigned char & g, unsigned char & b );
}; // ExAPReadPalettedImageFileAsRGB24


class ExAPBitmap8BitPaletted : public ExAPBitmapPaletted
{
protected:

	virtual void ConvertToRGBA64(
		unsigned short * dstRow,
		int dstSamplesPerRow,
		const ExAPRect & srcRect ) const throw( ExAPException );

	virtual void ConvertFromRGBA64(
		const unsigned short * srcRow,
		int srcSamplesPerRow,
		const ExAPRect & dstRect ) const throw( ExAPException );

public:

	// Constructors.
	ExAPBitmap8BitPaletted( ExAPBitmapPalette::PaletteType paletteType, int width, int height ) throw( ExAPException );
	ExAPBitmap8BitPaletted( const ExAPBitmapPalette & palette, int width, int height ) throw( ExAPException );

	// Destructor.
	virtual ~ExAPBitmap8BitPaletted( void );

	virtual ExAPAutoPtr<ExAPBitmap> CreateCompatibleBitmap( int width, int height ) const throw( ExAPException );
	virtual ExAPAutoPtr<ExAPBitmap> Duplicate( void ) const throw( ExAPException );

	virtual ExAPAutoPtr<ExAPReadPalettedImageFileAsRGB24> GetFileReader( ExAPAutoPtr<ExUtilFileReaderBaseInterface> pFile, int fileRowAlignmentInBytes, int srcTop, int srcLeft ) const;

	// **** File Import and Export ****

	virtual void ExportToGIF( const std::string & dstFilename ) const;
}; // class ExAPBitmap8BitPaletted


#endif // #ifndef _EXAPBITMAP8BITPALETTED_H_


// **** End of File ****
