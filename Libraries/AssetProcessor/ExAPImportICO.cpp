/*
 *  ExAPImportICO.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Wed Apr 20 2005.
 *  Copyright (c) 2005 by Tom Weatherhead. All rights reserved.
 *
 */

#ifdef _MSC_VER			// TAW 2002/03/11 : ie. if MSVC
#ifndef MACINTOSH		// TAW 2002/02/23 : ie. if MSVC

// Suppress warning C4786: Identifier truncated to 255 chars in the debug info.
#pragma warning(disable : 4786)

#endif
#endif


//lint -e1050 : Expected a template argument list '<...>' for template 'std::iterator<<1>,<2>,<3>>'
#include <vector>

#include "General\\ExAPAutoPtr.h"
#include "General\\ExAPException.h"
#include "UtilLib\\ExUtilFileReaderBaseInterface.h"
#include "ExAPBitmap1BitBlackWhite.h"
#include "ExAPBitmapPaletted.h"
#include "ExAPBitmapRGB24.h"
//#include "ExAPFileReaderStdio.h"

//lint +e1050


#define TAW_TRACE		1


class ICOEntry
{
public:
	int m_cursorWidth;
	int m_cursorHeight;
	int m_iconColourCount;
	//const int iconReserved;
	//const unsigned short m_iconPlanes;
	unsigned short m_iconBitCount;
	unsigned int m_iconSizeInBytes;
	unsigned int m_iconFileOffset;

	ICOEntry( int cursorWidth, int cursorHeight, int iconColourCount, unsigned short iconBitCount, unsigned int iconSizeInBytes, unsigned int iconFileOffset )
		: m_cursorWidth( cursorWidth ),
		  m_cursorHeight( cursorHeight ),
		  m_iconColourCount( iconColourCount ),
		  m_iconBitCount( iconBitCount ),
		  m_iconSizeInBytes( iconSizeInBytes ),
		  m_iconFileOffset( iconFileOffset )
	{
	}

	//ICOEntry( ExAPFileReaderBase & file ) {}

	ICOEntry & operator=( const ICOEntry & Src )
	{

		if ( *this != Src )
		{
			m_cursorWidth = Src.m_cursorWidth;
			m_cursorHeight = Src.m_cursorHeight;
			m_iconColourCount = Src.m_iconColourCount;
			m_iconBitCount = Src.m_iconBitCount;
			m_iconSizeInBytes = Src.m_iconSizeInBytes;
			m_iconFileOffset = Src.m_iconFileOffset;
		}

		return( *this );
	}

	bool operator==( const ICOEntry & Src )
	{
		return( m_cursorWidth == Src.m_cursorWidth  &&
			m_cursorHeight == Src.m_cursorHeight  &&
			m_iconColourCount == Src.m_iconColourCount  &&
			m_iconBitCount == Src.m_iconBitCount  &&
			m_iconSizeInBytes == Src.m_iconSizeInBytes  &&
			m_iconFileOffset == Src.m_iconFileOffset );
	}

	bool operator!=( const ICOEntry & Src )
	{
		return( !( *this == Src ) );
	}
}; // class ICOEntry


static void PopulateRGBImage( ExAPAutoPtr<ExAPBitmapRGB24> rgbImage, ExAPAutoPtr<ExAPReadPalettedImageFileAsRGB24> fileReader )
{
	const int imageWidth = rgbImage->GetWidth();
	const int imageHeight = rgbImage->GetHeight();
	int x;
	int y;
	unsigned char * rowPtr = rgbImage->getBuffer() + ( imageHeight - 1 ) * rgbImage->getBytesPerLine();
	unsigned char * pixelPtr;

	for( y = 0; y < imageHeight; ++y )
	{
		pixelPtr = rowPtr;
		rowPtr -= rgbImage->getBytesPerLine();

		for( x = 0; x < imageWidth; ++x )
		{
			fileReader->GetNextPixelFromFile( pixelPtr[0], pixelPtr[1], pixelPtr[2] );
			pixelPtr += 3;
		}
	}
}


void ExploreICOFile( const std::string & filename )
{
	//ExAPFileReaderStdio file( filename, true );	// Little-endian.
	ExAPAutoPtr<ExUtilFileReaderBaseInterface> pFile = ExUtilFileReaderBaseInterface::StdioCreate( filename, true );	// Little-endian.
	const unsigned short usReserved = pFile->GetWord();  // == 0
	const unsigned short usType = pFile->GetWord();  // == 1
	const unsigned short usCount = pFile->GetWord();  //  == Number of icons in this file

	if( pFile->EndOfFile() )
	{
		printf( "Error: End Of File encountered before header found.\n" );;
		return;
	}

	if( usReserved != 0 )
	{
		printf( "Error: The reserved word is not zero.\n" );
		return;
	}

	if( usType != 1 )
	{
		printf( "Error: The type word is not 1.\n" );
		return;
	}

	// Read the Entries.
	std::vector<ICOEntry> ICOEntryVector;
	unsigned short i;

	for( i = 0; i < usCount; ++i )
	{
		const int cursorWidth = pFile->GetByteAsInt();
		const int cursorHeight = pFile->GetByteAsInt();
		int iconColourCount = pFile->GetByteAsInt();

		if( iconColourCount == 0 )
		{
			iconColourCount = 256;
		}

		const int iconReserved = pFile->GetByteAsInt();  // == 0
		const unsigned short iconPlanes = pFile->GetWord();  // == 1
		const unsigned short iconBitCount = pFile->GetWord();
		const unsigned int iconSizeInBytes = pFile->GetDoubleWord();
		const unsigned int iconFileOffset = pFile->GetDoubleWord();

		const ICOEntry entry( cursorWidth, cursorHeight,iconColourCount,iconBitCount, iconSizeInBytes, iconFileOffset );
		ICOEntryVector.push_back( entry );
	}

	// Read the 40-byte InfoHeader
	const unsigned int iconInfoHeaderSize = pFile->GetDoubleWord();  // == 40
	const unsigned int iconWidth = pFile->GetDoubleWord();
	const unsigned int totalIconHeight = pFile->GetDoubleWord();  //  Added height of XOR-bitmap and AND-bitmap
	const unsigned int iconHeight = totalIconHeight / 2;
	const unsigned short iconPlanes = pFile->GetWord();  // == 1
	const unsigned short iconBitsPerPixel = pFile->GetWord();  // == 1, 4, or 8
	const unsigned int iconCompression = pFile->GetDoubleWord();  //  Type of compression == 0
	const unsigned int iconImageSize = pFile->GetDoubleWord();  //  Size of image in bytes == 0 (uncompressed)
	const unsigned int iconXpixelsPerM = pFile->GetDoubleWord();  //  unused == 0
	const unsigned int iconYpixelsPerM = pFile->GetDoubleWord();  //  unused == 0
	const unsigned int iconColoursUsed = pFile->GetDoubleWord();  //  unused == 0
	const unsigned int iconColoursImportant = pFile->GetDoubleWord();  //  unused == 0

	// Read the colour map (palette) for the XOR-bitmap(s)
	ExAPBitmapPalette palette( ExAPBitmapPalette::PalCustom );
	const int numberOfColours = 1 << iconBitsPerPixel;
	int j;

	for( j = 0; j < numberOfColours; ++j )
	{
		const unsigned char b = pFile->GetByte();
		const unsigned char g = pFile->GetByte();
		const unsigned char r = pFile->GetByte();
		const unsigned char reserved = pFile->GetByte();

		palette.AddEntry( r, g, b );
	}

	const int numPaletteEntries = palette.GetNumEntries();

	// Read the XOR bitmap(s) and the AND bitmap(s)...

	{
		ExAPAutoPtr<ExAPBitmapPaletted> xorImage = ExAPBitmapPaletted::Create( palette, iconWidth, iconHeight, iconBitsPerPixel );
		ExAPAutoPtr<ExAPBitmapRGB24> rgbImage = new ExAPBitmapRGB24( iconWidth, iconHeight );
		ExAPAutoPtr<ExAPReadPalettedImageFileAsRGB24> fileReader = xorImage->GetFileReader( pFile, 4, 0, 0 );

		PopulateRGBImage( rgbImage, fileReader );
		rgbImage->TestDisplay();
	}

	{
		//ExAPAutoPtr<ExAPBitmapPaletted> andImage = ExAPBitmapPaletted::Create( ExAPBitmapPalette::PalBlackWhite, iconWidth, iconHeight );
		ExAPAutoPtr<ExAPBitmap1BitBlackWhite> andImage = ExAPBitmap1BitBlackWhite::Create( iconWidth, iconHeight );
		ExAPAutoPtr<ExAPBitmapRGB24> rgbImage = new ExAPBitmapRGB24( iconWidth, iconHeight );
		ExAPAutoPtr<ExAPReadPalettedImageFileAsRGB24> fileReader = andImage->GetFileReader( pFile, 4, 0, 0 );

		PopulateRGBImage( rgbImage, fileReader );
		rgbImage->TestDisplay();
	}
} // ExploreICOFile()


// **** End of File ****
