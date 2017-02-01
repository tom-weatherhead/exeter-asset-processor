/*
 *  ExAPBitmapNon8BitPaletted.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Wed Apr 20 2005.
 *  Copyright (c) 2005 by Tom Weatherhead. All rights reserved.
 *
 */
#ifndef _EXAPBITMAPNON8BITPALETTED_H_
#define _EXAPBITMAPNON8BITPALETTED_H_

#include "ExAPBitmapPaletted.h"
#include "ExAPFileReaderBase.h"


class ExAPReadNon8BitPalettedImageFileAsRGB24 : public ExAPReadPalettedImageFileAsRGB24
{
private:
	int m_bitsPerPixel;
	unsigned char m_initialMask;
	unsigned char m_currentMask;
	unsigned char m_mask;
	unsigned char m_bufferByte;
	int m_currentBitShift;

public:
	// Constructor should be told which pixel in which byte is to read first.
	ExAPReadNon8BitPalettedImageFileAsRGB24( ExAPAutoPtr<ExUtilFileReaderBaseInterface> pFile, const ExAPBitmapPaletted & image,
		int fileRowAlignmentInBytes, int srcTop = 0, int srcLeft = 0 );
	virtual ~ExAPReadNon8BitPalettedImageFileAsRGB24( void );

	//virtual void Reset();
	virtual void GetNextPixelFromFile( unsigned char & r, unsigned char & g, unsigned char & b );
}; // ExAPReadPalettedImageFileAsRGB24


class ExAPBitmapNon8BitPaletted : public ExAPBitmapPaletted
{
private:
	int m_row;
	int m_col;
	int m_rowAlignmentInBytes;

protected:
	// Every bitmap can be converted to and from RGBA64.
	// The rectangles below have already been cropped by the master Convert function.

	virtual void ConvertToRGBA64(
		unsigned short * dstRow,
		int dstSamplesPerRow,
		const ExAPRect & srcRect ) const;

	virtual void ConvertFromRGBA64(
		const unsigned short * srcRow,
		int srcSamplesPerRow,
		const ExAPRect & dstRect ) const;

public:
	// Constructors.
	ExAPBitmapNon8BitPaletted( ExAPBitmapPalette::PaletteType paletteType,
		int bitsPerPixel, int width, int height, int rowAlignmentInBytes = 0 ) throw( ExAPException );
	ExAPBitmapNon8BitPaletted( const ExAPBitmapPalette & palette,
		int bitsPerPixel, int width, int height, int rowAlignmentInBytes = 0 ) throw( ExAPException );

	// Destructor.
	virtual ~ExAPBitmapNon8BitPaletted( void );

	virtual ExAPAutoPtr<ExAPBitmap> CreateCompatibleBitmap( int width, int height ) const throw( ExAPException );
	virtual ExAPAutoPtr<ExAPBitmap> Duplicate( void ) const throw( ExAPException );
	virtual ExAPAutoPtr<ExAPReadPalettedImageFileAsRGB24> GetFileReader( ExAPAutoPtr<ExUtilFileReaderBaseInterface> pFile, int fileRowAlignmentInBytes, int srcTop, int srcLeft ) const;
}; // ExAPBitmapPaletted


#endif // _EXAPBITMAPNON8BITPALETTED_H_


// **** End of File ****
