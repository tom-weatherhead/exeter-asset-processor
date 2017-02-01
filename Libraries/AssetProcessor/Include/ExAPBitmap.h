/*
 *  ExAPBitmap.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Wed Apr 30 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#ifndef _EXAPBITMAP_H_
#define _EXAPBITMAP_H_

#include "General\\ExAPAutoPtr.h"
#include "General\\ExAPException.h"
#include "ExAPColourModel.h"
#include "ExAPRect.h"


class ExAPBitmap
{
private:
	static int ComputeBytesPerLine( int bitsPerPixel, int width, int bytesPerLineAlignment );

protected:
	const ExAPColourModel m_colourModel;
	const int m_width;
	const int m_height;
	const int m_bitsPerPixel;
	const int m_bytesPerLineAlignment;
	const int m_bytesPerLine;

	static bool IsPowerOf2( int x );

	// Every bitmap can be converted to and from RGBA64.
	// The rectangles below have already been cropped by the master Convert function.

	virtual void ConvertToRGBA64(
		unsigned short * dstRow,
		int dstSamplesPerRow,
		const ExAPRect & srcRect ) const = 0;

	virtual void ConvertFromRGBA64(
		const unsigned short * srcRow,
		int srcSamplesPerRow,
		const ExAPRect & dstRect ) const = 0;

public:
	// Constructors.
	ExAPBitmap( ExAPColourModel::ModelType colourModelType, int bitsPerPixel, int width, int height, int bytesPerLineAlignment = 0 ) throw( ExAPException );

	// Destructor.
	virtual ~ExAPBitmap( void );
	
	// Operations.
	
	inline ExAPColourModel::ModelType GetColourModelType( void ) const
	{
		return( m_colourModel.GetModelType() );
	}

	inline int GetWidth( void ) const
	{
		return( m_width );
	}

	inline int GetHeight( void ) const
	{
		return( m_height );
	}

	inline int GetBitsPerPixel( void ) const
	{
		return( m_bitsPerPixel );
	}

	inline int getBytesPerLine( void ) const
	{
		return( m_bytesPerLine );
	}

	inline int getUnalignedBytesPerLine( void ) const
	{
		return( ( m_bitsPerPixel * m_width + 7 ) / 8 );
	}

	inline ExAPRect getBoundingRect( void ) const
	{
		return( ExAPRect( 0, 0, m_width, m_height ) );
	}

	// Convert the image to RGB24 and display.
	// This function is overridden in the RGB24 image class.
	virtual void TestDisplay( void ) const throw( ExAPException );

#if 0
	// A new dest image is created.  The return value is the dst image.
	ExAPBitmap * BilinearResample( const ExAPDimensions & dstDims, const ExAPRect & srcRect ) const;
	
	// Write to a previously existing image.
	void BilinearResample( ExAPBitmap & dst, const ExAPRect & dstRect, const ExAPRect & srcRect ) const;
#endif

	virtual ExAPAutoPtr<ExAPBitmap> CreateCompatibleBitmap( int width, int height ) const throw( ExAPException ) = 0;

	// Colour conversion:

	void ConvertColourModel( const ExAPPoint & dstPoint,
		const ExAPBitmap & srcBitmap, const ExAPPoint & srcPoint,
		const ExAPDimensions & dims ) const throw( ExAPException );

	virtual ExAPAutoPtr<ExAPBitmap> Duplicate( void ) const throw( ExAPException ) = 0;

	// **** File Import and Export ****

	// This function is overridden in the RGB24 and Paletted8 image classes.
	virtual void ExportToGIF( const std::string & dstFilename ) const;
}; // class ExAPBitmap


#endif //_EXAPBITMAP_H_


// **** End of File ****
