/*
 *  ExAPBitmap16bpch.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Mon Jul 14 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#ifndef _EXAPBITMAP16BPCH_H_
#define _EXAPBITMAP16BPCH_H_

#include "ExAPBitmap.h"


class ExAPBitmap16bpch : public ExAPBitmap
{
protected:
	typedef unsigned short SampleType;

	//SampleType * m_buffer;	// Use an AutoArray.
	int m_samplesPerPixel;
	int m_samplesPerLine;
	ExAPAutoArray<SampleType> m_buffer;

public:
	// Constructor.
	ExAPBitmap16bpch( ExAPColourModel::ModelType colourModelType, int width, int height ) throw( ExAPException );
	
	// Destructor.
	virtual ~ExAPBitmap16bpch();
	
	// Operations.

	inline SampleType * GetBuffer( void ) const
	{
		return( m_buffer );
	}

	inline int GetSamplesPerPixel( void ) const
	{
		return( m_samplesPerPixel );
	}

	inline int GetSamplesPerLine( void ) const
	{
		return( m_samplesPerLine );
	}

	static void Blit(
		SampleType * dst, int dstSamplesPerLine,
		const SampleType * src, int srcSamplesPerLine,
		int samplesPerPixel, int width, int height,
		bool srcIsSinglePixel = false );

#if 0	
	static void Blit(
		SampleType * dst, int dstBytesPerLine,
		int dstX, int dstY,
		const ExAPRect & dstBoundingRect,
		const SampleType * src, int srcBytesPerLine,
		const ExAPRect & srcRectParam,
		const ExAPRect & srcBoundingRect,
		int bytesPerPixel );

	void SolidRectangle( const ExAPRect & dstRect, const SampleType * src );

	void NonAntiAliasedCircle( int centerX, int centerY, int radius, SampleType * srcPixelData );

	//static ExAPBitmap16bpch * ReadJPEG( FILE * fp );
	//void WriteJPEG( const char * filename, int quality ) const;
#endif

	virtual ExAPAutoPtr<ExAPBitmap> Duplicate( void ) const throw( ExAPException );
}; // class ExAPBitmap16bpch


#endif //_EXAPBITMAP16BPCH_H_


// **** End of File ****
