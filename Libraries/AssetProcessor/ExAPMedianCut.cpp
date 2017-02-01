/*
 *  ExAPMedianCut.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Thu Jul 17 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#ifdef _MSC_VER			// TAW 2002/03/11 : ie. if MSVC
#ifndef MACINTOSH		// TAW 2002/02/23 : ie. if MSVC

// Suppress warning C4786: Identifier truncated to 255 chars in the debug info.
#pragma warning(disable : 4786)

#endif
#endif


//#define TAW_TRACE			1


//lint -e1050 : Expected a template argument list '<...>' for template 'std::iterator<<1>,<2>,<3>>'
#ifdef TAW_TRACE
#include <iostream>
#endif

#include <queue>	// For priority_queue.

//#include "ExAPBitmap8BitPaletted.h"
//#include "ExAPBitmapRGB24.h"
#include "ExAPMedianCut.h"
//lint +e1050


// **** Class ExAPRGBHistogram ****


class ExAPRGB24Histogram
{
private:
	// The default constructor is never defined; it is declared here in order to satisfy lint.
	//lint -esym( 1526, ExAPRGB24Histogram::ExAPRGB24Histogram )
	//lint -e1704 : Constructor has private access specification.
	ExAPRGB24Histogram( void );
	//lint +e1704

	const int m_significantBitsPerChannel;
	const int m_insignificantBitsPerChannel;
	const int m_maxScaledChannelValue;
	std::vector<int> m_v;

	int GetIndexFromColour( int r, int g, int b ) const throw( ExAPException );
	int GetIndexFromScaledChannels( int r, int g, int b ) const;

public:
	ExAPRGB24Histogram( int significantBitsPerChannel );

	void Populate( const ExAPBitmapRGB24 & srcImage, const ExAPRect & srcRect ) throw( ExAPException );
	//int LookupFromColour( int r, int g, int b ) const throw( ExAPException );
	int LookupFromScaledChannels( int r, int g, int b ) const;
}; // class ExAPRGB24Histogram


ExAPRGB24Histogram::ExAPRGB24Histogram( int significantBitsPerChannel )
	: m_significantBitsPerChannel( significantBitsPerChannel ),
		m_insignificantBitsPerChannel( 8 - m_significantBitsPerChannel ),
		m_maxScaledChannelValue( ( 1 << significantBitsPerChannel ) - 1 ),
		m_v( 1 << ( 3 * significantBitsPerChannel ), 0 )	// Fill with zeros.
{
} // ExAPRGB24Histogram::ExAPRGB24Histogram()


int ExAPRGB24Histogram::GetIndexFromScaledChannels( int r, int g, int b ) const
{
	//lint -e701 : Shift left of signed quantity.
	return( ( r << ( 2 * m_significantBitsPerChannel ) ) | ( g << m_significantBitsPerChannel ) | b );
	//lint +e701
} // ExAPRGB24Histogram::GetIndexFromScaledChannels()


int ExAPRGB24Histogram::GetIndexFromColour( int r, int g, int b ) const throw( ExAPException )
{
	
	if( r < 0  ||  r > 255  ||  g < 0  ||  g > 255  ||  b < 0  ||  b > 255 )
	{
		ThrowExAPException( ExAPException::IllegalParameterValue );
	}

	//lint -e702 : Shift right of signed quantity.
	r >>= m_insignificantBitsPerChannel;
	g >>= m_insignificantBitsPerChannel;
	b >>= m_insignificantBitsPerChannel;
	//lint +e702
	return( GetIndexFromScaledChannels( r, g, b ) );
} // ExAPRGB24Histogram::GetIndexFromColour()


void ExAPRGB24Histogram::Populate( const ExAPBitmapRGB24 & srcImage, const ExAPRect & srcRect ) throw( ExAPException )
{
	const int srcImageBytesPerRow = srcImage.getBytesPerLine();
	const unsigned char * srcImageRow = srcImage.getBuffer() + srcRect.getTop() * srcImageBytesPerRow + srcRect.getLeft() * 3;
	int numRows = srcRect.getHeight();
	const int numCols = srcRect.getWidth();

	while( numRows-- > 0 )
	{
		const unsigned char * srcImagePixel = srcImageRow;

		srcImageRow += srcImageBytesPerRow;

		for( int x = 0; x < numCols; ++x )
		{
			const int i = GetIndexFromColour( int( srcImagePixel[0] ), int( srcImagePixel[1] ), int( srcImagePixel[2] ) );

			m_v[ std::vector<int>::size_type( i ) ]++;
			srcImagePixel += 3;
		}
	}
} // ExAPRGB24Histogram::Populate()


#if 0
int ExAPRGB24Histogram::LookupFromColour( int r, int g, int b ) const throw( ExAPException )
{
	return( m_v[ std::vector<int>::size_type( GetIndexFromColour( r, g, b ) ) ] );
} // ExAPRGB24Histogram::LookupFromColour()
#endif


int ExAPRGB24Histogram::LookupFromScaledChannels( int r, int g, int b ) const
{
	return( m_v[ std::vector<int>::size_type( GetIndexFromScaledChannels( r, g, b ) ) ] );
} // ExAPRGB24Histogram::LookupFromScaledChannels()


// **** Class ExAPRGB24Subspace ****


class ExAPRGB24Subspace
{
private:
	const int m_significantBitsPerChannel;
	const int m_insignificantBitsPerChannel;
	const int m_scaledChannelSize;
	int m_minRed;
	int m_minGreen;
	int m_minBlue;
	int m_maxRed;
	int m_maxGreen;
	int m_maxBlue;
	int m_population;
	bool m_isSingleColour;	// ie. cannot be subdivided further.
	std::vector<int> m_redChannelHistogram;
	std::vector<int> m_greenChannelHistogram;
	std::vector<int> m_blueChannelHistogram;

	// The default constructor is never defined; it is declared here in order to satisfy lint.
	//lint -esym( 1526, ExAPRGB24Subspace::ExAPRGB24Subspace )
	//lint -e1704 : Constructor has private access specification.
	ExAPRGB24Subspace( void );
	//lint +e1704

	void FindMedian(
		const std::vector<int> & channelHistogram,
		int minChannel, int maxChannel,
		int & subspace1Max, int & subspace2Min,
		int & subspace1Population, int & subspace2Population ) const;

	unsigned char Unscale( int rawInt ) const;

public:

	ExAPRGB24Subspace(
		int significantBitsPerChannel,
		int minRed, int minGreen, int minBlue,
		int maxRed, int maxGreen, int maxBlue,
		int population );

	ExAPRGB24Subspace( const ExAPRGB24Subspace & src );

	ExAPRGB24Subspace & operator=( const ExAPRGB24Subspace & src );

	inline bool IsSingleColour( void ) const
	{
		return( m_isSingleColour );
	}

	bool operator<( const ExAPRGB24Subspace & src ) const;

	//void SetSingleColourFlag( void );

	void Shrink( const ExAPRGB24Histogram & histogram );

	void MedianCut( ExAPRGB24Subspace & newSubspace1, ExAPRGB24Subspace & newSubspace2 ) const;

	ExAPBitmapRGBPaletteEntry GetCentralColour( void ) const;

	bool ContainsScaledColour( int r, int g, int b ) const;

#ifdef TAW_TRACE
	inline int GetPopulation( void ) const
	{
		return( m_population );
	}
#endif
}; // class ExAPRGB24Subspace


ExAPRGB24Subspace::ExAPRGB24Subspace(
	int significantBitsPerChannel,
	// These mins and maxs are scaled down.
	int minRed, int minGreen, int minBlue,
	int maxRed, int maxGreen, int maxBlue,
	int population )
	:	m_significantBitsPerChannel( significantBitsPerChannel ),
		m_insignificantBitsPerChannel( 8 - significantBitsPerChannel ),
		m_scaledChannelSize( 1 << significantBitsPerChannel ),
		m_minRed( minRed ),
		m_minGreen( minGreen ),
		m_minBlue( minBlue ),
		m_maxRed( maxRed ),
		m_maxGreen( maxGreen ),
		m_maxBlue( maxBlue ),
		m_population( population ),
		m_isSingleColour( false ),
		m_redChannelHistogram( std::vector<int>::size_type( m_scaledChannelSize ), 0 ),
		m_greenChannelHistogram( std::vector<int>::size_type( m_scaledChannelSize ), 0 ),
		m_blueChannelHistogram( std::vector<int>::size_type( m_scaledChannelSize ), 0 )
{
} // ExAPRGB24Subspace::ExAPRGB24Subspace()


ExAPRGB24Subspace::ExAPRGB24Subspace( const ExAPRGB24Subspace & src )
	: m_significantBitsPerChannel( src.m_significantBitsPerChannel ),
		m_scaledChannelSize( src.m_scaledChannelSize ),
		//lint -e1729 : Initializer inversion detected for member.
		m_insignificantBitsPerChannel( src.m_insignificantBitsPerChannel ),
		//lint +e1729
		m_minRed( src.m_minRed ),
		m_minGreen( src.m_minGreen ),
		m_minBlue( src.m_minBlue ),
		m_maxRed( src.m_maxRed ),
		m_maxGreen( src.m_maxGreen ),
		m_maxBlue( src.m_maxBlue ),
		m_population( src.m_population ),
		m_isSingleColour( src.m_isSingleColour ),
		m_redChannelHistogram( src.m_redChannelHistogram ),
		m_greenChannelHistogram( src.m_greenChannelHistogram ),
		m_blueChannelHistogram( src.m_blueChannelHistogram )
{
} // ExAPRGB24Subspace::ExAPRGB24Subspace()


// Lint thinks this isn't referenced, but it's used by the priority queue.
//lint -esym( 1714, ExAPRGB24Subspace::operator= )

ExAPRGB24Subspace & ExAPRGB24Subspace::operator=( const ExAPRGB24Subspace & src )
{

	if( this != &src )
	{
		m_minRed = src.m_minRed;
		m_minGreen = src.m_minGreen;
		m_minBlue = src.m_minBlue;
		m_maxRed = src.m_maxRed;
		m_maxGreen = src.m_maxGreen;
		m_maxBlue = src.m_maxBlue;
		m_population = src.m_population;
		m_isSingleColour = src.m_isSingleColour;
		m_redChannelHistogram = src.m_redChannelHistogram;
		m_greenChannelHistogram = src.m_greenChannelHistogram;
		m_blueChannelHistogram = src.m_blueChannelHistogram;
	}

	return( *this );
} // ExAPRGB24Subspace::operator=()


// Lint thinks this isn't referenced, but it's used by the priority queue.
//lint -esym( 1714, ExAPRGB24Subspace::operator< )

bool ExAPRGB24Subspace::operator<( const ExAPRGB24Subspace & src ) const
{
	return( m_population < src.m_population );
} // ExAPRGB24Subspace::operator<()


void ExAPRGB24Subspace::Shrink( const ExAPRGB24Histogram & histogram )
{
	
	for( int i = 0; i < m_scaledChannelSize; ++i )
	{
		m_redChannelHistogram[ std::vector<int>::size_type( i ) ] = 0;
		m_greenChannelHistogram[ std::vector<int>::size_type( i ) ] = 0;
		m_blueChannelHistogram[ std::vector<int>::size_type( i ) ] = 0;
	}

	for( int r = m_minRed; r <= m_maxRed; ++r )
	{

		for( int g = m_minGreen; g <= m_maxGreen; ++g )
		{

			for( int b = m_minBlue; b <= m_maxBlue; ++b )
			{
				const int h = histogram.LookupFromScaledChannels( r, g, b );

				if( h > 0 )
				{
					m_redChannelHistogram[ std::vector<int>::size_type( r ) ] += h;
					m_greenChannelHistogram[ std::vector<int>::size_type( g ) ] += h;
					m_blueChannelHistogram[ std::vector<int>::size_type( b ) ] += h;
				}
			}
		}
	}

	while( m_minRed < m_maxRed  &&  m_redChannelHistogram[ std::vector<int>::size_type( m_minRed ) ] == 0 )
	{
		m_minRed++;
	}

	while( m_maxRed > m_minRed  &&  m_redChannelHistogram[ std::vector<int>::size_type( m_maxRed ) ] == 0 )
	{
		m_maxRed--;
	}

	while( m_minGreen < m_maxGreen  &&  m_greenChannelHistogram[ std::vector<int>::size_type( m_minGreen ) ] == 0 )
	{
		m_minGreen++;
	}

	while( m_maxGreen > m_minGreen  &&  m_greenChannelHistogram[ std::vector<int>::size_type( m_maxGreen ) ] == 0 )
	{
		m_maxGreen--;
	}

	while( m_minBlue < m_maxBlue  &&  m_blueChannelHistogram[ std::vector<int>::size_type( m_minBlue ) ] == 0 )
	{
		m_minBlue++;
	}

	while( m_maxBlue > m_minBlue  &&  m_blueChannelHistogram[ std::vector<int>::size_type( m_maxBlue ) ] == 0 )
	{
		m_maxBlue--;
	}

	m_isSingleColour = (
		m_minRed == m_maxRed  &&
		m_minGreen == m_maxGreen  &&
		m_minBlue == m_maxBlue );
} // ExAPRGB24Subspace::Shrink()


void ExAPRGB24Subspace::FindMedian(
	const std::vector<int> & channelHistogram,
	int minChannel, int maxChannel,
	int & subspace1Max, int & subspace2Min,
	int & subspace1Population, int & subspace2Population ) const
{
	const int halfPopulation = m_population / 2;
	int populationCounter = 0;
	int i = minChannel;

	for( ; ; )
	{
		populationCounter += channelHistogram[ std::vector<int>::size_type( i ) ];

		if( populationCounter >= halfPopulation  ||  i >= maxChannel - 1 )
		{
			break;
		}

		++i;
	}

	subspace1Max = i;
	subspace2Min = i + 1;
	subspace1Population = populationCounter;
	subspace2Population = m_population - populationCounter;
} // ExAPRGB24Subspace::FindMedian()


void ExAPRGB24Subspace::MedianCut( ExAPRGB24Subspace & newSubspace1, ExAPRGB24Subspace & newSubspace2 ) const
{
	const int redRange = m_maxRed - m_minRed + 1;
	const int greenRange = m_maxGreen - m_minGreen + 1;
	const int blueRange = m_maxBlue - m_minBlue + 1;

	if( redRange >= greenRange  &&  redRange >= blueRange )
	{
		FindMedian( m_redChannelHistogram, m_minRed, m_maxRed,
			newSubspace1.m_maxRed, newSubspace2.m_minRed,
			newSubspace1.m_population, newSubspace2.m_population );
	}
	else if( greenRange >= blueRange )
	{
		FindMedian( m_greenChannelHistogram, m_minGreen, m_maxGreen,
			newSubspace1.m_maxGreen, newSubspace2.m_minGreen,
			newSubspace1.m_population, newSubspace2.m_population );
	}
	else
	{
		FindMedian( m_blueChannelHistogram, m_minBlue, m_maxBlue,
			newSubspace1.m_maxBlue, newSubspace2.m_minBlue,
			newSubspace1.m_population, newSubspace2.m_population );
	}
} // ExAPRGB24Subspace::MedianCut()


unsigned char ExAPRGB24Subspace::Unscale( int rawInt ) const
{
	unsigned char rawChar = (unsigned char)rawInt;
	unsigned char rtn = 0;

	rawChar <<= m_insignificantBitsPerChannel;

	while( rawChar != 0 )
	{
		rtn |= rawChar;
		rawChar >>= m_significantBitsPerChannel;
	}

	return( rtn );
} // ExAPRGB24Subspace::Unscale()


ExAPBitmapRGBPaletteEntry ExAPRGB24Subspace::GetCentralColour( void ) const
{
	const unsigned char r = Unscale( ( m_minRed + m_maxRed ) / 2 );
	const unsigned char g = Unscale( ( m_minGreen + m_maxGreen ) / 2 );
	const unsigned char b = Unscale( ( m_minBlue + m_maxBlue ) / 2 );

	return( ExAPBitmapRGBPaletteEntry( r, g, b ) );
} // ExAPRGB24Subspace::GetCentralColour()


bool ExAPRGB24Subspace::ContainsScaledColour( int r, int g, int b ) const
{
	return(
		m_minRed <= r  &&  r <= m_maxRed  &&
		m_minGreen <= g  &&  g <= m_maxGreen  &&
		m_minBlue <= b  &&  b <= m_maxBlue );
} // ExAPRGB24Subspace::ContainsScaledColour()


// **** Entry point ****


ExAPAutoPtr<ExAPBitmap8BitPaletted> CreatePalettedImageViaMedianCut(
	const ExAPBitmapRGB24 & srcImage,
	const ExAPRect & srcRectParam,
	int numColoursInPalette ) throw( ExAPException )
{
	static const int significantBitsPerChannel = 5;
	static const int insignificantBitsPerChannel = 8 - significantBitsPerChannel;
	static const int scaledChannelSize = 1 << significantBitsPerChannel;
	static const int maxScaledChannelValue = scaledChannelSize - 1;
	ExAPAutoPtr<ExAPBitmap8BitPaletted> dstImage = 0;
	ExAPRGB24Histogram histogram( significantBitsPerChannel );
	const ExAPRect srcRect( intersect( srcRectParam, srcImage.getBoundingRect() ) );
	std::priority_queue<ExAPRGB24Subspace> priQ;
	std::vector<ExAPRGB24Subspace> processedSubspaces;

	if( srcRect.isEmpty() )
	{
		ThrowExAPException( ExAPException::IllegalParameterValue );
	}

	ExAPRGB24Subspace seed( significantBitsPerChannel, 0, 0, 0, maxScaledChannelValue, maxScaledChannelValue, maxScaledChannelValue, srcRect.GetArea() );

	histogram.Populate( srcImage, srcRect );
	seed.Shrink( histogram );
	priQ.push( seed );

	while( !priQ.empty()  &&
		priQ.size() + processedSubspaces.size() < (unsigned int)numColoursInPalette )
	{
		const ExAPRGB24Subspace qTop = priQ.top();

		priQ.pop();

		if( qTop.IsSingleColour() )
		{
			processedSubspaces.push_back( qTop );
			continue;
		}

		ExAPRGB24Subspace newSubspace1( qTop );
		ExAPRGB24Subspace newSubspace2( qTop );

		qTop.MedianCut( newSubspace1, newSubspace2 );

		newSubspace1.Shrink( histogram );
		newSubspace2.Shrink( histogram );

		priQ.push( newSubspace1 );
		priQ.push( newSubspace2 );
	}

	// Move the rest of the subspaces from the priority queue to the vector.

	while( !priQ.empty() )
	{
		const ExAPRGB24Subspace qTop = priQ.top();

		priQ.pop();
		processedSubspaces.push_back( qTop );
	}

	// At this point, the subspaces in the vector are already sorted
	// from largest population to smallest.

	std::vector<ExAPRGB24Subspace>::const_iterator psi = processedSubspaces.begin();
	const std::vector<ExAPRGB24Subspace>::const_iterator psiEnd = processedSubspaces.end();
	ExAPBitmapPalette palette( ExAPBitmapPalette::PalCustom );

#ifdef TAW_TRACE
	std::cout << "Generated palette colours and population count." << std::endl;
#endif

	while( psi != psiEnd )
	{
		palette.m_entries.push_back( psi->GetCentralColour() );
#ifdef TAW_TRACE
		std::cout << psi->GetPopulation() << "   ("
			<< int( palette.m_entries.back().GetRed() ) << ","
			<< int( palette.m_entries.back().GetGreen() ) << ","
			<< int( palette.m_entries.back().GetBlue() ) << ")" << std::endl;
#endif
		++psi;
	}

	// Create the paletted image.
	const int imageWidth = srcRect.getWidth();
	const int imageHeight = srcRect.getHeight();

	try
	{
		dstImage = new ExAPBitmap8BitPaletted( palette, imageWidth, imageHeight );
	}
	catch( ExAPException & )
	{
		throw;
	}
	catch( ... )
	{
		// Do nothing.
	}

	if( dstImage == 0 )
	{
		ThrowExAPException( ExAPException::ResourceAcquisitionFailed );
	}

	// Convert the image data.
	// This conversion maintains the subspace population counts.
	const int srcBytesPerRow = srcImage.getBytesPerLine();
	const unsigned char * srcRow = srcImage.getBuffer() + srcRect.getTop() * srcBytesPerRow + srcRect.getLeft() * 3;
	const int dstBytesPerRow = dstImage->getBytesPerLine();
	unsigned char * dstRow = dstImage->getBuffer();
	int numRows = imageHeight;
	const int numSubspaces = int( processedSubspaces.size() );

	while( numRows-- > 0 )
	{
		const unsigned char * srcPixel = srcRow;
		unsigned char * dstPixel = dstRow;

		srcRow += srcBytesPerRow;
		dstRow += dstBytesPerRow;

		for( int x = 0; x < imageWidth; ++x )
		{
			const int rScaled = int( srcPixel[0] ) >> insignificantBitsPerChannel;
			const int gScaled = int( srcPixel[1] ) >> insignificantBitsPerChannel;
			const int bScaled = int( srcPixel[2] ) >> insignificantBitsPerChannel;

			for( int i = 0; i < numSubspaces; ++i )
			{
				
				if( processedSubspaces[ std::vector<ExAPRGB24Subspace>::size_type( i ) ].ContainsScaledColour( rScaled, gScaled, bScaled ) )
				{
					*dstPixel = (unsigned char)i;
					break;
				}
			}

			srcPixel += 3;
			dstPixel++;
		}
	}

	return( dstImage );
} // CreatePalettedImageViaMedianCut()


// **** End of File ****
