/*
 *  ExAPBitmap8bpch.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Sat May 03 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

//#include <cstdio>	// For JPEG I/O
//#include <memory>	// For memset()
#include <cmath>

#include "ExAPBitmap8bpch.h"
//#include "ExAPBitmapGrey8.h"
//#include "ExAPBitmapRGB24.h"
//#include "ExAPBitmapCMYK32.h"
//#include "ExAPFile.h"

//lint -e526 : Function not defined.
//lint -e755 : Global macro not referenced.
//lint -e756 : Global typedef not referenced.
//lint -e757 : Global declarator not referenced.
//lint -e768 : Global struct member not referenced.
//lint -e769 : Global enumeration constant not referenced.

//extern "C"
//{
//#include "..\\3rdParty\\JPEG6b\\jpeglib.h"
//}


// Constructor.
	
ExAPBitmap8bpch::ExAPBitmap8bpch( ExAPColourModel::ModelType colourModelType, int width, int height ) throw( ExAPException )
	: ExAPBitmap( colourModelType, ExAPColourModel::CalculateBitsPerPixel( colourModelType ), width, height ), m_buffer( 0 )
{
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


// Destructor.

ExAPBitmap8bpch::~ExAPBitmap8bpch()
{
	delete [] m_buffer;
	m_buffer = 0;
}


void ExAPBitmap8bpch::Blit(
	unsigned char * dst, int dstBytesPerLine,
	const unsigned char * src, int srcBytesPerLine,
	int bytesPerPixel, int width, int height, bool srcIsSinglePixel )
{
	const int bytesToCopyPerRow = bytesPerPixel * width;
	
	if( srcIsSinglePixel )
	{
	
		while( height-- > 0 )
		{
			unsigned char * dstPixel = dst;
			int x = width;
			
			dst += dstBytesPerLine;
			
			while( x-- > 0 )
			{
			
				for( int i = 0; i < bytesPerPixel; ++i )
				{
					dstPixel[i] = src[i];
				}
				
				dstPixel += bytesPerPixel;
			}
		}
	}
	else if( bytesToCopyPerRow != dstBytesPerLine  ||  bytesToCopyPerRow != srcBytesPerLine )
	{
	
		while( height-- > 0 )
		{
			memcpy( dst, src, size_t( bytesToCopyPerRow ) );
			dst += dstBytesPerLine;
			src += srcBytesPerLine;
		}
	}
	else
	{
		// Do it all in one operation.
		memcpy( dst, src, size_t( bytesToCopyPerRow * height ) );
	}
} // ExAPBitmap8bpch::Blit()


void ExAPBitmap8bpch::Blit(
	unsigned char * dst, int dstBytesPerLine,
	int dstX, int dstY,
	const ExAPRect & dstBoundingRect,
	const unsigned char * src, int srcBytesPerLine,
	const ExAPRect & srcRectParam,
	const ExAPRect & srcBoundingRect,
	int bytesPerPixel )
{
	const int srcToDstX = dstX - srcRectParam.getLeft();
	const int srcToDstY = dstY - srcRectParam.getTop();
	ExAPRect srcRect( srcRectParam );
	
	(void)srcRect.intersect( srcBoundingRect );				// rect &= rect
	(void)srcRect.offset( srcToDstX, srcToDstY );			// rect += vector
	(void)srcRect.intersect( dstBoundingRect );				// rect &= rect
	
	const ExAPRect dstRect( srcRect );
	
	(void)srcRect.offset( -srcToDstX, -srcToDstY );		// rect -= vector
	
	if( srcRect.isEmpty() )
	{
		return;
	}

	unsigned char * dstRow = dst + dstRect.getTop() * dstBytesPerLine + dstRect.getLeft() * bytesPerPixel;
	const unsigned char * srcRow = src + srcRect.getTop() * srcBytesPerLine + srcRect.getLeft() * bytesPerPixel;

	ExAPBitmap8bpch::Blit(
		dstRow, dstBytesPerLine,
		srcRow, srcBytesPerLine,
		bytesPerPixel, dstRect.getWidth(), dstRect.getHeight() );
} // ExAPBitmap8bpch::Blit()


void ExAPBitmap8bpch::SolidRectangle( const ExAPRect & dstRectParam, const unsigned char * src )
{
	ExAPRect dstRect( dstRectParam );
	
	(void)dstRect.intersect( getBoundingRect() );
	
	if( dstRect.isEmpty() )
	{
		return;
	}

	unsigned char * dstRow = m_buffer + dstRect.getTop() * m_bytesPerLine + dstRect.getLeft() * m_bitsPerPixel / 8;

	ExAPBitmap8bpch::Blit(
		dstRow, m_bytesPerLine,
		src, 0,
		m_bitsPerPixel / 8, dstRect.getWidth(), dstRect.getHeight(), true );
} // ExAPBitmap8bpch::SolidRectangle()


void ExAPBitmap8bpch::NonAntiAliasedCircle( int centerX, int centerY, int radius, unsigned char * srcPixelData )
{
	ExAPRect regionOfInterest( centerX - radius, centerY - radius, 2 * radius, 2 * radius );

	(void)regionOfInterest.intersect( getBoundingRect() );

	if( regionOfInterest.isEmpty() )
	{
		return;
	}

	unsigned char * imageRow = m_buffer + regionOfInterest.getTop() * m_bytesPerLine;
	const double radiusAsDouble = double( radius );
	const double radiusSquared = radiusAsDouble * radiusAsDouble;
	const double centerXAsADouble = double( centerX );

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

		const int bytesPerPixel = m_bitsPerPixel / 8;
		unsigned char * imagePixel = imageRow + leftEdge * bytesPerPixel;

		for( int x = leftEdge; x <= rightEdge; ++x )
		{
			
			for( int i = 0; i < bytesPerPixel; ++i )
			{
				imagePixel[i] = srcPixelData[i];
			}

			imagePixel += bytesPerPixel;
		}

		imageRow += m_bytesPerLine;
	}
} // ExAPBitmap8bpch::NonAntiAliasedCircle()


void ExAPBitmap8bpch::ComplementImageData( void )
{
	int bytesRemaining = m_bytesPerLine * m_height;
	unsigned char * p = m_buffer;

	while( bytesRemaining > 0 )
	{
		*p = ~*p;
		++p;
		--bytesRemaining;
	}
} // ExAPBitmap8bpch::ComplementImageData()


ExAPAutoPtr<ExAPBitmap> ExAPBitmap8bpch::Duplicate( void ) const throw( ExAPException )
{
	ExAPAutoPtr<ExAPBitmap> rawDupe = CreateCompatibleBitmap( GetWidth(), GetHeight() );
	ExAPBitmap * rawDupeAsPtr = rawDupe;
	ExAPBitmap8bpch * dupe = dynamic_cast<ExAPBitmap8bpch *>( rawDupeAsPtr );

	if( dupe == 0 )
	{
		ThrowExAPException( ExAPException::DynamicCastFailed );
	}

	ExAPBitmap8bpch::Blit( dupe->getBuffer(), dupe->getBytesPerLine(),
		getBuffer(), getBytesPerLine(),
		m_bitsPerPixel / 8, GetWidth(), GetHeight() );
	return( rawDupe );
} // ExAPBitmap8bpch::Duplicate()


// **** End of File ****
