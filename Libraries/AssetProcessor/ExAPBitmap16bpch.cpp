/*
 *  ExAPBitmap16bpch.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Sat May 03 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#include <memory>	// For memset()
#include <cmath>

#include "ExAPBitmap16bpch.h"


// Constructor.
	
ExAPBitmap16bpch::ExAPBitmap16bpch( ExAPColourModel::ModelType colourModelType, int width, int height ) throw( ExAPException )
	: ExAPBitmap( colourModelType, ExAPColourModel::CalculateBitsPerPixel( colourModelType ), width, height ),
		m_samplesPerPixel( m_bitsPerPixel / ( 8 * int( sizeof( SampleType ) ) ) ),
		m_samplesPerLine( m_bytesPerLine / int( sizeof( SampleType ) ) ),
		m_buffer( m_samplesPerLine * m_height )
{
	//memset( m_buffer, 0, bufferSize );
} // ExAPBitmap16bpch::ExAPBitmap16bpch()


// Destructor.

ExAPBitmap16bpch::~ExAPBitmap16bpch()
{
} // ExAPBitmap16bpch::~ExAPBitmap16bpch()


void ExAPBitmap16bpch::Blit(
	SampleType * dst, int dstSamplesPerLine,
	const SampleType * src, int srcSamplesPerLine,
	int samplesPerPixel, int width, int height, bool srcIsSinglePixel )
{
	const int samplesToCopyPerRow = samplesPerPixel * width;
	const int bytesToCopyPerRow = int( sizeof( SampleType ) ) * samplesToCopyPerRow;
	
	if( srcIsSinglePixel )
	{
	
		while( height-- > 0 )
		{
			SampleType * dstPixel = dst;
			int x = width;
			
			dst += dstSamplesPerLine;
			
			while( x-- > 0 )
			{
			
				for( int i = 0; i < samplesPerPixel; ++i )
				{
					dstPixel[i] = src[i];
				}
				
				dstPixel += samplesPerPixel;
			}
		}
	}
	else if( samplesToCopyPerRow != dstSamplesPerLine  ||  samplesToCopyPerRow != srcSamplesPerLine )
	{
	
		while( height-- > 0 )
		{
			memcpy( dst, src, size_t( bytesToCopyPerRow ) );
			dst += dstSamplesPerLine;
			src += srcSamplesPerLine;
		}
	}
	else
	{
		// Do it all in one operation.
		memcpy( dst, src, size_t( bytesToCopyPerRow * height ) );
	}
} // ExAPBitmap16bpch::Blit()


#if 0
void ExAPBitmap16bpch::Blit(
	SampleType * dst, int dstSamplesPerLine,
	int dstX, int dstY,
	const ExAPRect & dstBoundingRect,
	const SampleType * src, int srcSamplesPerLine,
	const ExAPRect & srcRectParam,
	const ExAPRect & srcBoundingRect,
	int samplesPerPixel )
{
	const int srcToDstX = dstX - srcRectParam.getLeft();
	const int srcToDstY = dstY - srcRectParam.getTop();
	ExAPRect srcRect( srcRectParam );
	
	srcRect.intersect( srcBoundingRect );				// rect &= rect
	srcRect.offset( srcToDstX, srcToDstY );				// rect += vector
	srcRect.intersect( dstBoundingRect );				// rect &= rect
	
	const ExAPRect dstRect( srcRect );
	
	srcRect.offset( -srcToDstX, -srcToDstY );			// rect -= vector
	
	if( srcRect.isEmpty() )
	{
		return;
	}

	SampleType * dstRow = dst + dstRect.getTop() * dstSamplesPerLine + dstRect.getLeft() * samplesPerPixel;
	const SampleType * srcRow = src + srcRect.getTop() * srcSamplesPerLine + srcRect.getLeft() * samplesPerPixel;

	ExAPBitmap16bpch::Blit(
		dstRow, dstSamplesPerLine,
		srcRow, srcSamplesPerLine,
		samplesPerPixel, dstRect.getWidth(), dstRect.getHeight() );
} // ExAPBitmap16bpch::Blit()


void ExAPBitmap16bpch::SolidRectangle( const ExAPRect & dstRectParam, const unsigned char * src )
{
	ExAPRect dstRect( dstRectParam );
	
	dstRect.intersect( getBoundingRect() );
	
	if( dstRect.isEmpty() )
	{
		return;
	}

	unsigned char * dstRow = m_buffer + dstRect.getTop() * m_bytesPerLine + dstRect.getLeft() * m_bytesPerPixel;

	ExAPBitmap16bpch::Blit(
		dstRow, m_bytesPerLine,
		src, 0,
		m_bytesPerPixel, dstRect.getWidth(), dstRect.getHeight(), true );
} // ExAPBitmap16bpch::SolidRectangle()


void ExAPBitmap16bpch::NonAntiAliasedCircle( int centerX, int centerY, int radius, unsigned char * srcPixelData )
{
	ExAPRect regionOfInterest( centerX - radius, centerY - radius, 2 * radius, 2 * radius );

	regionOfInterest.intersect( getBoundingRect() );

	if( regionOfInterest.isEmpty() )
	{
		return;
	}

	unsigned char * imageRow = m_buffer + regionOfInterest.getTop() * m_bytesPerLine;
	const double radiusSquared = static_cast<double>( radius * radius );
	const double centerXAsADouble = static_cast<double>( centerX );

	for( int y = regionOfInterest.getTop(); y < regionOfInterest.getBottom(); ++y )
	{
		const double deltaY = static_cast<double>( y - centerY );
		const double secantLengthSquared = radiusSquared - deltaY * deltaY;

		if( secantLengthSquared < 0.0 )
		{
			// An exception might be justified here.
			continue;
		}

		const double secantLength = sqrt( secantLengthSquared );
		const int leftEdge = static_cast<int>( ceil( centerXAsADouble - secantLength ) );
		const int rightEdge = static_cast<int>( floor( centerXAsADouble + secantLength ) );

		if( leftEdge > rightEdge )
		{
			// An exception might be justified here.
			continue;
		}

		unsigned char * imagePixel = imageRow + leftEdge * m_bytesPerPixel;

		for( int x = leftEdge; x <= rightEdge; ++x )
		{
			
			for( int i = 0; i < m_bytesPerPixel; ++i )
			{
				imagePixel[i] = srcPixelData[i];
			}

			imagePixel += m_bytesPerPixel;
		}

		imageRow += m_bytesPerLine;
	}
} // ExAPBitmap16bpch::NonAntiAliasedCircle()
#endif


ExAPAutoPtr<ExAPBitmap> ExAPBitmap16bpch::Duplicate( void ) const throw( ExAPException )
{
	ExAPAutoPtr<ExAPBitmap> rawDupe = CreateCompatibleBitmap( GetWidth(), GetHeight() );
	ExAPBitmap * rawDupeAsPtr = rawDupe;
	ExAPBitmap16bpch * dupe = dynamic_cast<ExAPBitmap16bpch *>( rawDupeAsPtr );

	if( dupe == 0 )
	{
		ThrowExAPException( ExAPException::DynamicCastFailed );
	}

	ExAPBitmap16bpch::Blit( dupe->GetBuffer(), dupe->GetSamplesPerLine(),
		GetBuffer(), GetSamplesPerLine(),
		GetSamplesPerPixel(), GetWidth(), GetHeight() );
	return( rawDupe );
} // ExAPBitmap16bpch::Duplicate()


// **** End of File ****
