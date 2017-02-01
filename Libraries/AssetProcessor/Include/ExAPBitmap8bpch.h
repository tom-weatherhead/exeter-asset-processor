/*
 *  ExAPBitmap8bpch.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Sat May 03 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#ifndef _EXAPBITMAP8BPCH_H_
#define _EXAPBITMAP8BPCH_H_

//#include <cstdio>		// For ReadJPEG( FILE * fp ).

#include "General\\ExAPAutoPtr.h"
#include "UtilLib\\ExUtilFileReaderBaseInterface.h"
#include "ExAPBitmap.h"


class ExAPBitmap8bpch : public ExAPBitmap
{
private:

	// Copy constructor.
	ExAPBitmap8bpch( const ExAPBitmap8bpch & src );

	// Assignment operator.
	ExAPBitmap8bpch & operator=( const ExAPBitmap8bpch & src );

protected:
	unsigned char * m_buffer;

public:
	// Constructor.
	ExAPBitmap8bpch( ExAPColourModel::ModelType colourModelType, int width, int height ) throw( ExAPException );
	
	// Destructor.
	virtual ~ExAPBitmap8bpch();
	
	// Operations.

	inline unsigned char * getBuffer( void ) const
	{
		return( m_buffer );
	}
	
	inline int GetBytesPerPixel( void ) const
	{
		return( m_bitsPerPixel / 8 );
	}

	static void Blit(
		unsigned char * dst, int dstBytesPerLine,
		const unsigned char * src, int srcBytesPerLine,
		int bytesPerPixel, int width, int height, bool srcIsSinglePixel = false );

	static void Blit(
		unsigned char * dst, int dstBytesPerLine,
		int dstX, int dstY,
		const ExAPRect & dstBoundingRect,
		const unsigned char * src, int srcBytesPerLine,
		const ExAPRect & srcRectParam,
		const ExAPRect & srcBoundingRect,
		int bytesPerPixel );
		
	void SolidRectangle( const ExAPRect & dstRect, const unsigned char * src );

	void NonAntiAliasedCircle( int centerX, int centerY, int radius, unsigned char * srcPixelData );

	void ComplementImageData( void );

	//static ExAPBitmap8bpch * ReadJPEG( FILE * fp ) throw( ExAPException );
	static ExAPBitmap8bpch * ReadJPEG( ExAPAutoPtr<ExUtilFileReaderBaseInterface> pSrcFile ) throw( ExAPException );
	void WriteJPEG( const char * filename, int quality ) const throw( ExAPException );

	virtual ExAPAutoPtr<ExAPBitmap> Duplicate( void ) const throw( ExAPException );
}; // class ExAPBitmap8bpch


#endif //_EXAPBITMAP8BPCH_H_


// **** End of File ****
