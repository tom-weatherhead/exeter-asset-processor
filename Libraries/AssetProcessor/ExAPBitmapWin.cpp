/*
 *  ExAPBitmapWin.cpp
 *  Exeter AssetProcessor
 *	This file contains bitmap-related calls to the Windows API.
 *
 *  Created by Tom Weatherhead on Fri Jun 20 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include "ExAPBitmapRGB24.h"
#include "General\\ExAPException.h"


class ExAPWinDC
{
private:
	HDC m_hDC;

public:

	ExAPWinDC( void )
		: m_hDC( ::GetDC( NULL ) )
	{

		if( m_hDC == NULL )
		{
			ThrowExAPException( ExAPException::ResourceAcquisitionFailed );
		}
	}

	~ExAPWinDC( void )
	{
		ReleaseDC( NULL, m_hDC );
		m_hDC = NULL;
	}

	inline HDC GetDC( void ) const
	{
		return( m_hDC );
	}
}; // class ExAPWinDC


void ExAPBitmapRGB24::TestDisplay( void ) const
{
	// Create a DIB header for the image buffer, create a hDC, and display.
	const ExAPWinDC dc;
	ExAPAutoArray<unsigned char> displayBuffer( m_height * m_bytesPerLine );

	// Create a copy of the image with the red and blue channels swapped.
	unsigned char * srcRow = m_buffer;
	unsigned char * dstRow = displayBuffer;

	for( int y = 0; y < m_height; ++y )
	{
		unsigned char * srcPixel = srcRow;
		unsigned char * dstPixel = dstRow;

		srcRow += m_bytesPerLine;
		dstRow += m_bytesPerLine;

		for( int x = 0; x < m_width; ++x )
		{
			dstPixel[0] = srcPixel[2];
			dstPixel[1] = srcPixel[1];
			dstPixel[2] = srcPixel[0];
			srcPixel += 3;
			dstPixel += 3;
		}
	}

	BITMAPINFO Bmi;
	BITMAPINFOHEADER * pbmiHeader = &Bmi.bmiHeader;

	pbmiHeader->biSize 					= sizeof( BITMAPINFOHEADER );
	pbmiHeader->biXPelsPerMeter = 0;
	pbmiHeader->biYPelsPerMeter = 0;  
	pbmiHeader->biPlanes 				= 1;
	pbmiHeader->biWidth					= m_width;
	pbmiHeader->biHeight				= -m_height;	// Top-down DIB.
	pbmiHeader->biSizeImage			= m_height * m_bytesPerLine;
	pbmiHeader->biCompression 	= BI_RGB;
	pbmiHeader->biBitCount			= 24;
	pbmiHeader->biClrUsed 			= 0;
	pbmiHeader->biClrImportant	= 0;

	const int numScanLinesSet = SetDIBitsToDevice(
		dc.GetDC(),
		0, 0, //pBlock->nX, pBlock->nY,
		m_width, m_height,
		0, 0,
		0, m_height,
		displayBuffer,
		&Bmi,
		DIB_RGB_COLORS	);

	if( numScanLinesSet == 0 )
	{
		// An error occurred during the call to SetDIBitsToDevice().
		ThrowExAPException( ExAPException::SystemCallFailed );
	}
} // ExAPBitmapRGB24::TestDisplay()


// **** End of File ****
