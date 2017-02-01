/*
 *  ExAPBitmapPaletted.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Wed Apr 20 2005.
 *  Copyright (c) 2005 by Tom Weatherhead. All rights reserved.
 *
 */

#ifndef _EXAPBITMAPPALETTED_H_
#define _EXAPBITMAPPALETTED_H_

#include "UtilLib\\ExUtilFileReaderBaseInterface.h"
#include "ExAPBitmap.h"
#include "ExAPBitmapPalette.h"


#if 0 //def _MSC_VER			// TAW 2002/03/11 : ie. if MSVC
#ifndef MACINTOSH		// TAW 2002/02/23 : ie. if MSVC

// Suppress warning C4786: Identifier truncated to 255 chars in the debug info.
#pragma warning(disable : 4786)

#endif
#endif


class ExAPBitmapPaletted;


class ExAPReadPalettedImageFileAsRGB24
{
protected:
	int m_row;
	int m_col;
	ExAPAutoPtr<ExUtilFileReaderBaseInterface> m_pFile;
	const ExAPBitmapPaletted & m_image;
	int m_fileRowAlignmentInBytes;

public:
	ExAPReadPalettedImageFileAsRGB24( ExAPAutoPtr<ExUtilFileReaderBaseInterface> pFile, const ExAPBitmapPaletted & image, int fileRowAlignmentInBytes );
	virtual ~ExAPReadPalettedImageFileAsRGB24( void );

	//virtual void Reset() = 0;
	virtual void GetNextPixelFromFile( unsigned char & r, unsigned char & g, unsigned char & b ) = 0;
}; // ExAPReadPalettedImageFileAsRGB24


class ExAPBitmapPaletted : public ExAPBitmap
{
private:
	void AllocateBuffer( void );

protected:
	// TAW 2003/06/22: Make this a smart pointer to a reference-counted object?
	ExAPBitmapPalette m_palette;
	unsigned char * m_buffer;
	int m_bitsPerPixel;
	int m_fileRowAlignmentInBytes;

public:
	// Constructors.
	ExAPBitmapPaletted( ExAPBitmapPalette::PaletteType paletteType, int bitsPerPixel, int width, int height, int fileRowAlignmentInBytes = 0 ) throw( ExAPException );
	ExAPBitmapPaletted( const ExAPBitmapPalette & palette, int bitsPerPixel, int width, int height, int fileRowAlignmentInBytes = 0 ) throw( ExAPException );

	// Destructor.
	virtual ~ExAPBitmapPaletted( void );

	virtual ExAPAutoPtr<ExAPBitmap> CreateCompatibleBitmap( int width, int height ) const throw( ExAPException ) = 0;

	inline unsigned char * getBuffer( void ) const
	{
		return( m_buffer );
	}

	inline ExAPBitmapPalette & getPalette( void )
	{
		return( m_palette );
	}

	inline const ExAPBitmapPalette & getPalette( void ) const
	{
		return( m_palette );
	}

	inline ExAPBitmapPalette::PaletteType getPaletteType( void ) const
	{
		return( m_palette.GetPaletteType() );
	}

	inline int getBitsPerPixel( void ) const
	{
		return( m_bitsPerPixel );
	}

	virtual ExAPAutoPtr<ExAPReadPalettedImageFileAsRGB24> GetFileReader( ExAPAutoPtr<ExUtilFileReaderBaseInterface> pFile, int fileRowAlignmentInBytes, int srcTop = 0, int srcLeft = 0 ) const = 0;

	static ExAPAutoPtr<ExAPBitmapPaletted> Create( const ExAPBitmapPalette & palette, int width, int height, int bitsPerPixel = 0, int fileRowAlignmentInBytes = 0 ) throw( ExAPException );
	static ExAPAutoPtr<ExAPBitmapPaletted> Create( ExAPBitmapPalette::PaletteType paletteType, int width, int height, int bitsPerPixel = 0, int fileRowAlignmentInBytes = 0 ) throw( ExAPException );
}; // ExAPBitmapPaletted


#endif // _EXAPBITMAPPALETTED_H_


// **** End of File ****
