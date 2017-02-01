/*
 *  ExAPBitmap8BitPaletted.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Sun Jun 22 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

//lint -e1050 : Error -- expected a template argument list '<...>' for template 'std::iterator<<1>,<2>,<3>>'.
#include "ExAPBitmap8BitPaletted.h"
//lint +e1050


ExAPRead8BitPalettedImageFileAsRGB24::ExAPRead8BitPalettedImageFileAsRGB24( ExAPAutoPtr<ExUtilFileReaderBaseInterface> pFile, const ExAPBitmapPaletted & image, int fileRowAlignmentInBytes )
	: ExAPReadPalettedImageFileAsRGB24( pFile, image, fileRowAlignmentInBytes )
{
}


ExAPRead8BitPalettedImageFileAsRGB24::~ExAPRead8BitPalettedImageFileAsRGB24( void )
{
}


void ExAPRead8BitPalettedImageFileAsRGB24::GetNextPixelFromFile( unsigned char & r, unsigned char & g, unsigned char & b )
{
	const ExAPBitmapPalette & palette = m_image.getPalette();
	const int numPaletteEntries = palette.GetNumEntries();
	const int paletteIndex = m_pFile->GetByteAsInt();
	const ExAPBitmapRGBPaletteEntry colour = palette.GetEntry( paletteIndex );

	r = colour.GetRed();
	g = colour.GetGreen();
	b = colour.GetBlue();

	const int imageWidth = m_image.GetWidth();

	if( ++m_col >= imageWidth )
	{
		m_col = 0;
		++m_row;

		// Skip enough bytes to maintain alignment.

		if( imageWidth % m_fileRowAlignmentInBytes != 0 )
		{
			m_pFile->SkipBytes( m_fileRowAlignmentInBytes - imageWidth % m_fileRowAlignmentInBytes ) ;
		}
	}
}


// Constructors.

ExAPBitmap8BitPaletted::ExAPBitmap8BitPaletted( ExAPBitmapPalette::PaletteType paletteType, int width, int height ) throw( ExAPException )
	: ExAPBitmapPaletted( paletteType, 8, width, height )
{
}


ExAPBitmap8BitPaletted::ExAPBitmap8BitPaletted( const ExAPBitmapPalette & palette, int width, int height ) throw( ExAPException )
	: ExAPBitmapPaletted( palette, 8, width, height )
{
}


// Destructor.

ExAPBitmap8BitPaletted::~ExAPBitmap8BitPaletted( void )
{
}


ExAPAutoPtr<ExAPBitmap> ExAPBitmap8BitPaletted::CreateCompatibleBitmap( int width, int height ) const throw( ExAPException )
{
	ExAPAutoPtr<ExAPBitmap> newBitmap;

	try
	{
		newBitmap = new ExAPBitmap8BitPaletted( m_palette, width, height );
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

	if( newBitmap == 0 )
	{
		ThrowExAPException( ExAPException::ResourceAcquisitionFailed );
	}

	return( newBitmap );
} // ExAPBitmap8BitPaletted::CreateCompatibleBitmap()


ExAPAutoPtr<ExAPBitmap> ExAPBitmap8BitPaletted::Duplicate( void ) const throw( ExAPException )
{
	ExAPAutoPtr<ExAPBitmap> newImage = CreateCompatibleBitmap( m_width, m_height );
	ExAPBitmap * pRawNewImage = newImage;
	ExAPBitmap8BitPaletted * pCastNewImage = dynamic_cast<ExAPBitmap8BitPaletted *>( pRawNewImage );

	if( pCastNewImage == 0 )
	{
		ThrowExAPException( ExAPException::DynamicCastFailed );
	}

	memcpy( pCastNewImage->m_buffer, m_buffer, m_bytesPerLine * m_height );
	return( newImage );
}


void ExAPBitmap8BitPaletted::ConvertToRGBA64( unsigned short * dstRow, int dstSamplesPerRow,
	const ExAPRect & srcRect ) const throw( ExAPException )
{
	const unsigned char * srcRow = m_buffer + srcRect.getTop() * m_bytesPerLine + srcRect.getLeft();
	const int srcRectWidth = srcRect.getWidth();
	int srcRectHeight = srcRect.getHeight();

	while( srcRectHeight-- > 0 )
	{
		unsigned short * dstPixel = dstRow;
		const unsigned char * srcPixel = srcRow;

		dstRow += dstSamplesPerRow;
		srcRow += m_bytesPerLine;

		for( int i = 0; i < srcRectWidth; ++i )
		{
			const ExAPBitmapRGBPaletteEntry & entry = m_palette.GetEntry( static_cast<int>( *srcPixel ) );

			dstPixel[0] = (unsigned short)( 257 * entry.GetRed() );
			dstPixel[1] = (unsigned short)( 257 * entry.GetGreen() );
			dstPixel[2] = (unsigned short)( 257 * entry.GetBlue() );
			dstPixel[3] = 65535;
			dstPixel += 4;
			srcPixel++;
		}
	}
} // ExAPBitmap8BitPaletted::ConvertToRGBA64()


void ExAPBitmap8BitPaletted::ConvertFromRGBA64( const unsigned short * srcRow, int srcSamplesPerRow,
	const ExAPRect & dstRect ) const throw( ExAPException )
{
	unsigned char * dstRow = m_buffer + dstRect.getTop() * m_bytesPerLine + dstRect.getLeft();
	const int dstRectWidth = dstRect.getWidth();
	int dstRectHeight = dstRect.getHeight();

	for( int j = 0; j < dstRectHeight; ++j )
	{
		unsigned char * dstPixel = dstRow;
		const unsigned short * srcPixel = srcRow;

		dstRow += m_bytesPerLine;
		srcRow += srcSamplesPerRow;

		for( int i = 0; i < dstRectWidth; ++i )
		{
			*dstPixel++ = m_palette.m_pDitherer->DitherPixel(
				m_palette,
				(unsigned char)( srcPixel[0] / 256 ),
				(unsigned char)( srcPixel[1] / 256 ),
				(unsigned char)( srcPixel[2] / 256 ),
				dstRect.getLeft(), dstRect.getTop(), i , j );
			srcPixel += 4;
		}
	}
} // ExAPBitmap8BitPaletted::ConvertFromRGBA64()


ExAPAutoPtr<ExAPReadPalettedImageFileAsRGB24> ExAPBitmap8BitPaletted::GetFileReader( ExAPAutoPtr<ExUtilFileReaderBaseInterface> pFile, int fileRowAlignmentInBytes, int srcTop, int srcLeft ) const
{
	ExAPAutoPtr<ExAPReadPalettedImageFileAsRGB24> reader;

	try
	{
		reader = new ExAPRead8BitPalettedImageFileAsRGB24( pFile, *this, fileRowAlignmentInBytes );
	}
	catch( ExAPException & )
	{
		throw;
	}
	catch( ... )
	{
		// Do nothing.
	}
		
	if( reader == 0 )
	{
		ThrowExAPException( ExAPException::ResourceAcquisitionFailed );
	}

	return( reader );
}


// **** End of File ****
