/*
 *  ExAPBitmapPaletted.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Wed Apr 20 2005.
 *  Copyright (c) 2005 by Tom Weatherhead. All rights reserved.
 *
 */

//#include "ExAPBitmapPaletted.h"
#include "ExAPBitmap8BitPaletted.h"
#include "ExAPBitmapNon8BitPaletted.h"


#ifdef _MSC_VER			// TAW 2002/03/11 : ie. if MSVC
#ifndef MACINTOSH		// TAW 2002/02/23 : ie. if MSVC

// Suppress warning C4786: Identifier truncated to 255 chars in the debug info.
#pragma warning(disable : 4786)

#endif
#endif


ExAPReadPalettedImageFileAsRGB24::ExAPReadPalettedImageFileAsRGB24( ExAPAutoPtr<ExUtilFileReaderBaseInterface> pFile, const ExAPBitmapPaletted & image, int fileRowAlignmentInBytes )
	: m_row( 0 ),
	  m_col( 0 ),
	  m_pFile( pFile ),
	  m_image( image ),
	  m_fileRowAlignmentInBytes( fileRowAlignmentInBytes )
{
}


ExAPReadPalettedImageFileAsRGB24::~ExAPReadPalettedImageFileAsRGB24( void )
{
}


ExAPBitmapPaletted::ExAPBitmapPaletted( ExAPBitmapPalette::PaletteType paletteType, int bitsPerPixel, int width, int height, int fileRowAlignmentInBytes ) throw( ExAPException )
	: ExAPBitmap( ExAPColourModel::Paletted, bitsPerPixel, width, height ),
	  m_palette( paletteType ),
	  m_bitsPerPixel( bitsPerPixel ),
	  m_fileRowAlignmentInBytes( fileRowAlignmentInBytes )
{
	AllocateBuffer();
}


ExAPBitmapPaletted::ExAPBitmapPaletted( const ExAPBitmapPalette & palette, int bitsPerPixel, int width, int height, int fileRowAlignmentInBytes ) throw( ExAPException )
: ExAPBitmap( ExAPColourModel::Paletted, bitsPerPixel, width, height, fileRowAlignmentInBytes ),
	  m_palette( palette ),
	  m_bitsPerPixel( bitsPerPixel ),
	  m_fileRowAlignmentInBytes( fileRowAlignmentInBytes )
{

	if( m_palette.GetNumEntries() == 0 )
	{
		ThrowExAPException( ExAPException::IllegalParameterValue );
	}

	AllocateBuffer();
}


ExAPBitmapPaletted::~ExAPBitmapPaletted( void )
{
	delete [] m_buffer;
	m_buffer = 0;
}


void ExAPBitmapPaletted::AllocateBuffer( void )
{
#if 0
	m_bytesPerLine = ( m_bitsPerPixel * m_width + 7 ) / 8;

	if( m_fileRowAlignmentInBytes > 0 )
	{

		while( m_bytesPerLine % m_fileRowAlignmentInBytes != 0 )
		{
			++m_bytesPerLine;
		}
	}
#endif
	const unsigned int bufferSize = static_cast<unsigned int>( m_bytesPerLine * m_height );
	
	// TO_DO: Use an AutoArray.

	try
	{
		m_buffer = new unsigned char[bufferSize];
	}
	catch( ... )
	{
		// Do nothing.
	}
		
	if( m_buffer == 0 )
	{
		ThrowExAPException( ExAPException::ResourceAcquisitionFailed );
	}
		
	memset( m_buffer, 0, size_t( bufferSize ) );
}


#if 0
ExAPAutoPtr<ExAPBitmapPaletted> CreatePalettedBitmap( ExAPBitmapPalette::PaletteType paletteType, int bitsPerPixel, int width, int height ) throw( ExAPException )
{
	ExAPAutoPtr<ExAPBitmapPaletted> image;	// Initialized to zero.

	try
	{

		if( bitsPerPixel == 8 )
		{
			image = new ExAPBitmap8BitPaletted( paletteType, width, height );
		}
		else
		{
			image = new ExAPBitmapNon8BitPaletted( paletteType, bitsPerPixel, width, height );
		}
	}
	catch( ExAPException & )
	{
		// We caught a known exception; re-throw it.
		throw;
	}
	catch( ... )
	{
		// We caught an unknown exception.
		ThrowExAPException( ExAPException::UnknownException );
	}

	if( image == 0 )
	{
		ThrowExAPException( ExAPException::ResourceAcquisitionFailed );
	}

	return( image );
}
#endif


ExAPAutoPtr<ExAPBitmapPaletted> ExAPBitmapPaletted::Create( const ExAPBitmapPalette & palette, int width, int height, int bitsPerPixel, int fileRowAlignmentInBytes ) throw( ExAPException )
{
	const int numPaletteEntries = palette.GetNumEntries();
	//int bitsPerPixel = 0;

	if( numPaletteEntries < 1  ||  numPaletteEntries > 256 )
	{
		ThrowExAPException( ExAPException::IllegalParameterValue );
	}

	if( bitsPerPixel > 0 )
	{

		if( bitsPerPixel > 8  ||  ( bitsPerPixel & ( bitsPerPixel - 1 ) ) != 0  ||  numPaletteEntries > ( 1 << bitsPerPixel ) )
		{
			ThrowExAPException( ExAPException::IllegalParameterValue );
		}
	}
	else if( numPaletteEntries > 4 )
	{

		if( numPaletteEntries > 16 )
		{
			bitsPerPixel = 8;
		}
		else
		{
			bitsPerPixel = 4;
		}
	}
	else
	{

		if( numPaletteEntries > 2 )
		{
			bitsPerPixel = 2;
		}
		else
		{
			bitsPerPixel = 1;
		}
	}

	ExAPAutoPtr<ExAPBitmapPaletted> image;	// Initialized to zero.

	try
	{
		ExAPBitmapPaletted * p = 0;

		if( bitsPerPixel == 8 )
		{
			p = new ExAPBitmap8BitPaletted( palette, width, height );
		}
		else
		{
			p = new ExAPBitmapNon8BitPaletted( palette, bitsPerPixel, width, height, fileRowAlignmentInBytes );
		}

		image = p;
	}
	catch( ExAPException & )
	{
		// We caught a known exception; re-throw it.
		throw;
	}
	catch( ... )
	{
		// We caught an unknown exception.
		ThrowExAPException( ExAPException::UnknownException );
	}

	if( image == 0 )
	{
		ThrowExAPException( ExAPException::ResourceAcquisitionFailed );
	}

	return( image );
}


ExAPAutoPtr<ExAPBitmapPaletted> ExAPBitmapPaletted::Create( ExAPBitmapPalette::PaletteType paletteType, int width, int height, int bitsPerPixel, int fileRowAlignmentInBytes ) throw( ExAPException )
{
	return( Create( ExAPBitmapPalette( paletteType ), width, height, bitsPerPixel, fileRowAlignmentInBytes ) );
}


// **** End of File ****
