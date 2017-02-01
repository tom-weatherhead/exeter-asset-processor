/*
 *  ExAPBitmapNon8BitPaletted.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Wed Apr 20 2005.
 *  Copyright (c) 2005 by Tom Weatherhead. All rights reserved.
 *
 */

//#include "ExAPBitmapDitherBayer.h"
//#include "ExAPBitmapDitherErrorDiffusion.h"
#include "ExAPBitmapNon8BitPaletted.h"
//#include "ExAPFileReaderBase.h"


#ifdef _MSC_VER			// TAW 2002/03/11 : ie. if MSVC
#ifndef MACINTOSH		// TAW 2002/02/23 : ie. if MSVC

// Suppress warning C4786: Identifier truncated to 255 chars in the debug info.
#pragma warning(disable : 4786)

#endif
#endif


// **** Class ExAPReadNon8BitPalettedImageFileAsRGB24 ****


ExAPReadNon8BitPalettedImageFileAsRGB24::ExAPReadNon8BitPalettedImageFileAsRGB24( ExAPAutoPtr<ExUtilFileReaderBaseInterface> pFile, const ExAPBitmapPaletted & image,
																				  int fileRowAlignmentInBytes, int srcTop, int srcLeft )
	: ExAPReadPalettedImageFileAsRGB24( pFile, image, fileRowAlignmentInBytes ),
	  m_bitsPerPixel( m_image.getBitsPerPixel() ),
	  m_mask( 255 >> ( 8 - m_bitsPerPixel ) ),	// Or ( 1 << m_bitsPerPixel ) - 1
	  //m_bufferByte( 0 ),
	  m_currentBitShift( 8 - m_bitsPerPixel - ( ( srcLeft * m_bitsPerPixel ) % 8 ) )
{
	const int numBytesToSkip = srcTop * image.getBytesPerLine() + ( srcLeft * m_bitsPerPixel ) % 8;

	pFile->SkipBytes( numBytesToSkip );
	// Force the first byte to be read.
	m_bufferByte = pFile->GetByte();
}


ExAPReadNon8BitPalettedImageFileAsRGB24::~ExAPReadNon8BitPalettedImageFileAsRGB24( void )
{
}


void ExAPReadNon8BitPalettedImageFileAsRGB24::GetNextPixelFromFile( unsigned char & r, unsigned char & g, unsigned char & b )
{
	const int imageWidth = m_image.GetWidth();

	if( m_currentBitShift < 0 )
	{
		m_currentBitShift = 8 - m_bitsPerPixel;
		m_bufferByte = m_pFile->GetByte();
	}

	const int paletteIndex = ( m_bufferByte >> m_currentBitShift ) & m_mask;
	const ExAPBitmapRGBPaletteEntry colour = m_image.getPalette().GetEntry( paletteIndex );
	r = colour.GetRed();
	g = colour.GetGreen();
	b = colour.GetBlue();
	m_currentBitShift -= m_bitsPerPixel;

	if( ++m_col >= imageWidth )
	{
		m_col = 0;
		++m_row;

		// Skip enough bytes to maintain alignment.
		const int bytesPerRow = ( imageWidth * m_bitsPerPixel + 7 ) / 8;

		if( !m_pFile->EndOfFile()  &&  ( bytesPerRow % m_fileRowAlignmentInBytes != 0 ) )
		{
			m_pFile->SkipBytes( m_fileRowAlignmentInBytes - bytesPerRow % m_fileRowAlignmentInBytes ) ;
		}
	}
}


// **** Class ExAPBitmapNon8BitPaletted ****


ExAPBitmapNon8BitPaletted::ExAPBitmapNon8BitPaletted( ExAPBitmapPalette::PaletteType paletteType,
	int bitsPerPixel, int width, int height, int rowAlignmentInBytes ) throw( ExAPException )
	: ExAPBitmapPaletted( paletteType, bitsPerPixel, width, height ),
	  m_rowAlignmentInBytes( rowAlignmentInBytes )
{

	if( m_bitsPerPixel < 1  ||  m_bitsPerPixel > 8  ||
		( m_bitsPerPixel & ( m_bitsPerPixel - 1 ) ) != 0 )  // Power of 2 test
	{
		ThrowExAPException( ExAPException::IllegalParameterValue );
	}
}


ExAPBitmapNon8BitPaletted::ExAPBitmapNon8BitPaletted( const ExAPBitmapPalette & palette,
	int bitsPerPixel, int width, int height, int rowAlignmentInBytes )
	: ExAPBitmapPaletted( palette, bitsPerPixel, width, height ),
	  m_rowAlignmentInBytes( rowAlignmentInBytes )
{

	if( m_bitsPerPixel < 1  ||  m_bitsPerPixel > 8  ||
		( m_bitsPerPixel & ( m_bitsPerPixel - 1 ) ) != 0 )  // Power of 2 test
	{
		ThrowExAPException( ExAPException::IllegalParameterValue );
	}
}


ExAPBitmapNon8BitPaletted::~ExAPBitmapNon8BitPaletted( void )
{
}


ExAPAutoPtr<ExAPBitmap> ExAPBitmapNon8BitPaletted::CreateCompatibleBitmap( int width, int height ) const throw( ExAPException )
{
	ExAPAutoPtr<ExAPBitmap> newBitmap;

	try
	{
		newBitmap = new ExAPBitmapNon8BitPaletted( m_palette, m_bitsPerPixel, width, height, m_rowAlignmentInBytes );		//newBitmap = new ExAPBitmapNon8BitPaletted( getPaletteType(), m_bitsPerPixel, width, height, m_rowAlignmentInBytes );
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
}


void ExAPBitmapNon8BitPaletted::ConvertToRGBA64(
		unsigned short * dstRow,
		int dstSamplesPerRow,
		const ExAPRect & srcRect ) const
{
	const int pixelMask = 255 >> ( 8 - m_bitsPerPixel );	// Or ( 1 << m_bitsPerPixel ) - 1
	const int srcBytesPerLine = getBytesPerLine();
	const int srcPixelsPerByte = 8 / m_bitsPerPixel;
	unsigned char * pSrcRow = getBuffer() + srcRect.getTop() * srcBytesPerLine + srcRect.getLeft() / srcPixelsPerByte;

	for( int j = 0; j < srcRect.getHeight(); ++j )
	{
		int currentBitShift = 8 - m_bitsPerPixel - ( ( srcRect.getLeft() * getBitsPerPixel() ) % 8 );
		unsigned char * pSrcByte = pSrcRow;
		unsigned short * dstPixel = dstRow;

		pSrcRow += srcBytesPerLine;
		dstRow += dstSamplesPerRow;

		for( int i = 0; i < srcRect.getWidth(); ++i )
		{

			if( currentBitShift < 0 )
			{
				++pSrcByte;
				currentBitShift = 8 - m_bitsPerPixel;
			}

			const int paletteIndex = ( *pSrcByte >> currentBitShift ) & pixelMask;

			currentBitShift -= m_bitsPerPixel;

			const ExAPBitmapRGBPaletteEntry & colour = m_palette.GetEntry( paletteIndex );

			dstPixel[0] = 257 * colour.GetRed();
			dstPixel[1] = 257 * colour.GetGreen();
			dstPixel[2] = 257 * colour.GetBlue();
			dstPixel[3] = 65535;
			dstPixel += 4;
		}
	}
}


void ExAPBitmapNon8BitPaletted::ConvertFromRGBA64(
		const unsigned short * srcRow,
		int srcSamplesPerRow,
		const ExAPRect & dstRect ) const
{
	const int dstPixelsPerByte = 8 / m_bitsPerPixel;
	const int q = ( dstRect.getLeft() % dstPixelsPerByte ) * m_bitsPerPixel;
	unsigned char * dstRow = m_buffer + dstRect.getTop() * m_bytesPerLine + dstRect.getLeft() / dstPixelsPerByte;
	const int dstRectWidth = dstRect.getWidth();
	int dstRectHeight = dstRect.getHeight();
	unsigned char initialMask = 255 << ( 8 - m_bitsPerPixel );

	//m_palette.GenerateClosestColourVector();

	for( int j = 0; j < dstRectHeight; ++j )
	{
		unsigned char * dstPixel = dstRow;
		const unsigned short * srcPixel = srcRow;
		unsigned char mask = initialMask >> q;
		int shift = 8 - m_bitsPerPixel - q;

		dstRow += m_bytesPerLine;
		srcRow += srcSamplesPerRow;

		for( int i = 0; i < dstRectWidth; ++i )
		{
			const unsigned char paletteIndex = m_palette.m_pDitherer->DitherPixel(
				m_palette,
				(unsigned char)( srcPixel[0] / 256 ),
				(unsigned char)( srcPixel[1] / 256 ),
				(unsigned char)( srcPixel[2] / 256 ),
				dstRect.getLeft(), dstRect.getTop(), i , j );
			srcPixel += 4;

			const unsigned char inverseMask = ~mask;

			*dstPixel = ( /* (unsigned char) */ paletteIndex << shift ) | ( *dstPixel & inverseMask );
			shift -= m_bitsPerPixel;
			mask >>= m_bitsPerPixel;

			if( shift < 0 )
			{
				++dstPixel;
				shift = 8 - m_bitsPerPixel;
				mask = initialMask;
			}
		}
	}
}


ExAPAutoPtr<ExAPBitmap> ExAPBitmapNon8BitPaletted::Duplicate( void ) const throw( ExAPException )
{
	ExAPAutoPtr<ExAPBitmap> newImage = CreateCompatibleBitmap( m_width, m_height );
	ExAPBitmap * pRawNewImage = newImage;
	ExAPBitmapNon8BitPaletted * pCastNewImage = dynamic_cast<ExAPBitmapNon8BitPaletted *>( pRawNewImage );

	if( pCastNewImage == 0 )
	{
		ThrowExAPException( ExAPException::DynamicCastFailed );
	}

	memcpy( pCastNewImage->m_buffer, m_buffer, m_bytesPerLine * m_height );
	return( newImage );
}


ExAPAutoPtr<ExAPReadPalettedImageFileAsRGB24> ExAPBitmapNon8BitPaletted::GetFileReader( ExAPAutoPtr<ExUtilFileReaderBaseInterface> pFile, int fileRowAlignmentInBytes, int srcTop, int srcLeft ) const
{
	ExAPAutoPtr<ExAPReadPalettedImageFileAsRGB24> reader;

	try
	{
		reader = new ExAPReadNon8BitPalettedImageFileAsRGB24( pFile, *this, fileRowAlignmentInBytes, srcTop, srcLeft );
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
