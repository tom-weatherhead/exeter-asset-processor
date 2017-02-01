/*
 *  ExAPBitmapPalette.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Sat Jun 21 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

//lint -e1050 : Expected a template argument list '<...>' for template 'std::iterator<<1>,<2>,<3>>'
#include "General\\ExAPException.h"
#include "ExAPBitmapPalette.h"
#include "ExAPBitmapDitherNone.h"
#include "ExAPBitmapDitherBayer.h"
#include "ExAPBitmapDitherErrorDiffusion.h"
//lint +e1050


ExAPBitmapPalette::ExAPBitmapPalette( ExAPBitmapPalette::PaletteType t )
	: m_type( t )
{

	switch( m_type )
	{
		case ExAPBitmapPalette::Pal6R6G6B20:	// A total of 236 colours; not the full 256.
		{

			for( int r = 0; r <= 255; r += 51 )
			{

				for( int g = 0; g <= 255; g += 51 )
				{

					for( int b = 0; b <= 255; b += 51 )
					{
						m_entries.push_back( ExAPBitmapRGBPaletteEntry( (unsigned char)r, (unsigned char)g, (unsigned char)b ) );
					}
				}
			}

			for( int grey = 0; grey <= 255; grey += 13 )
			{
				const unsigned char greyChar = (unsigned char)grey;

				m_entries.push_back( ExAPBitmapRGBPaletteEntry( greyChar, greyChar, greyChar ) );
			}

			break;
		}

		case ExAPBitmapPalette::PalBlackWhite:
			m_entries.push_back( ExAPBitmapRGBPaletteEntry( (unsigned char)0, (unsigned char)0, (unsigned char)0 ) );
			m_entries.push_back( ExAPBitmapRGBPaletteEntry( (unsigned char)255, (unsigned char)255, (unsigned char)255 ) );
			break;

		case ExAPBitmapPalette::PalVGA:
/*
			Add_Color (P, 0, 0, 0) ; (* 0: Black *)
			Add_Color (P, 0, 0, 192) ; (* 1: Blue *)
			Add_Color (P, 0, 192, 0) ; (* 2: Green *)
			Add_Color (P, 0, 192, 192) ; (* 3: Cyan *)
			Add_Color (P, 192, 0, 0) ; (* 4: Red *)
			Add_Color (P, 192, 0, 192) ; (* 5: Magenta *)
			Add_Color (P, 192, 192, 0) ; (* 6: Brown *)
			Add_Color (P, 192, 192, 192) ; (* 7: LightGray *)
			Add_Color (P, 128, 128, 128) ; (* 8: DarkGray *)
			Add_Color (P, 0, 0, 255) ; (* 9: LightBlue *)
			Add_Color (P, 0, 255, 0) ; (* 10: LightGreen *)
			Add_Color (P, 0, 255, 255) ; (* 11: LightCyan *)
			Add_Color (P, 255, 0, 0) ; (* 12: LightRed *)
			Add_Color (P, 255, 0, 255) ; (* 13: LightMagenta *)
			Add_Color (P, 255, 255, 0) ; (* 14: Yellow *)
			Add_Color (P, 255, 255, 255) ; (* 15: White *) 
*/
			m_entries.push_back( ExAPBitmapRGBPaletteEntry( (unsigned char)0, (unsigned char)0, (unsigned char)0 ) );		// Black
			m_entries.push_back( ExAPBitmapRGBPaletteEntry( (unsigned char)0, (unsigned char)0, (unsigned char)192 ) );		// Dark blue
			m_entries.push_back( ExAPBitmapRGBPaletteEntry( (unsigned char)0, (unsigned char)192, (unsigned char)0 ) );		// Dark green
			m_entries.push_back( ExAPBitmapRGBPaletteEntry( (unsigned char)0, (unsigned char)192, (unsigned char)192 ) );	// Dark cyan
			m_entries.push_back( ExAPBitmapRGBPaletteEntry( (unsigned char)192, (unsigned char)0, (unsigned char)0 ) );		// Dark red
			m_entries.push_back( ExAPBitmapRGBPaletteEntry( (unsigned char)192, (unsigned char)0, (unsigned char)192 ) );	// Dark magenta
			m_entries.push_back( ExAPBitmapRGBPaletteEntry( (unsigned char)192, (unsigned char)192, (unsigned char)0 ) );	// Dark yellow (brown?)
			m_entries.push_back( ExAPBitmapRGBPaletteEntry( (unsigned char)192, (unsigned char)192, (unsigned char)192 ) );	// 75% grey
			m_entries.push_back( ExAPBitmapRGBPaletteEntry( (unsigned char)128, (unsigned char)128, (unsigned char)128 ) );	// 50% grey
			m_entries.push_back( ExAPBitmapRGBPaletteEntry( (unsigned char)0, (unsigned char)0, (unsigned char)255 ) );		// Light blue
			m_entries.push_back( ExAPBitmapRGBPaletteEntry( (unsigned char)0, (unsigned char)255, (unsigned char)0 ) );		// Light green
			m_entries.push_back( ExAPBitmapRGBPaletteEntry( (unsigned char)0, (unsigned char)255, (unsigned char)255 ) );	// Light cyan
			m_entries.push_back( ExAPBitmapRGBPaletteEntry( (unsigned char)255, (unsigned char)0, (unsigned char)0 ) );		// Light red
			m_entries.push_back( ExAPBitmapRGBPaletteEntry( (unsigned char)255, (unsigned char)0, (unsigned char)255 ) );	// Light magenta
			m_entries.push_back( ExAPBitmapRGBPaletteEntry( (unsigned char)255, (unsigned char)255, (unsigned char)0 ) );	// Light yellow
			m_entries.push_back( ExAPBitmapRGBPaletteEntry( (unsigned char)255, (unsigned char)255, (unsigned char)255 ) );	// White
			break;

		case ExAPBitmapPalette::Pal2BitGrey:
			m_entries.push_back( ExAPBitmapRGBPaletteEntry( (unsigned char)0, (unsigned char)0, (unsigned char)0 ) );		// Black
			m_entries.push_back( ExAPBitmapRGBPaletteEntry( (unsigned char)85, (unsigned char)85, (unsigned char)85 ) );	// 1/3 Grey
			m_entries.push_back( ExAPBitmapRGBPaletteEntry( (unsigned char)170, (unsigned char)170, (unsigned char)170 ) );	// 2/3 Grey
			m_entries.push_back( ExAPBitmapRGBPaletteEntry( (unsigned char)255, (unsigned char)255, (unsigned char)255 ) );	// White
			break;

		case ExAPBitmapPalette::Pal2BitCMYK:
			m_entries.push_back( ExAPBitmapRGBPaletteEntry( (unsigned char)0, (unsigned char)255, (unsigned char)255 ) );	// Cyan
			m_entries.push_back( ExAPBitmapRGBPaletteEntry( (unsigned char)255, (unsigned char)0, (unsigned char)255 ) );	// Magenta
			m_entries.push_back( ExAPBitmapRGBPaletteEntry( (unsigned char)255, (unsigned char)255, (unsigned char)0 ) );	// Yellow
			m_entries.push_back( ExAPBitmapRGBPaletteEntry( (unsigned char)0, (unsigned char)0, (unsigned char)0 ) );		// Black
			break;

		case ExAPBitmapPalette::Pal2BitRGBW:
			m_entries.push_back( ExAPBitmapRGBPaletteEntry( (unsigned char)255, (unsigned char)0, (unsigned char)0 ) );		// Red
			m_entries.push_back( ExAPBitmapRGBPaletteEntry( (unsigned char)0, (unsigned char)255, (unsigned char)0 ) );		// Green
			m_entries.push_back( ExAPBitmapRGBPaletteEntry( (unsigned char)0, (unsigned char)0, (unsigned char)255 ) );		// Blue
			m_entries.push_back( ExAPBitmapRGBPaletteEntry( (unsigned char)255, (unsigned char)255, (unsigned char)255 ) );	// White
			break;

		case ExAPBitmapPalette::PalCustom:
		default:
			break;
	}

	GenerateClosestColourVector();
	SetDefaultDitherer();
} // ExAPBitmapPalette::ExAPBitmapPalette()


ExAPBitmapPalette::ExAPBitmapPalette( const ExAPBitmapRGBPaletteEntry & start, const ExAPBitmapRGBPaletteEntry & end )
	: m_type( PalDuotone )
{
	const int startRed = (int)start.GetRed();
	const int startGreen = (int)start.GetGreen();
	const int startBlue = (int)start.GetBlue();
	const int deltaRed = (int)end.GetRed() - (int)start.GetRed();
	const int deltaGreen = (int)end.GetGreen() - (int)start.GetGreen();
	const int deltaBlue = (int)end.GetBlue() - (int)start.GetBlue();

	for( int i = 0; i < 256; ++i)
	{
		const unsigned char r = (unsigned char)( startRed + deltaRed * i / 255 );
		const unsigned char g = (unsigned char)( startGreen + deltaGreen * i / 255 );
		const unsigned char b = (unsigned char)( startBlue + deltaBlue * i / 255 );

		m_entries.push_back( ExAPBitmapRGBPaletteEntry( r, g, b ) );
	}

	GenerateClosestColourVector();
	SetDefaultDitherer();
}


ExAPBitmapPalette::ExAPBitmapPalette( const ExAPBitmapPalette & Src )
	: m_type( Src.m_type ),
	  m_entries( Src.m_entries ),
	  m_closestColour( Src.m_closestColour )
{
	SetDefaultDitherer();
}


ExAPBitmapPalette & ExAPBitmapPalette::operator=( const ExAPBitmapPalette & Src )
{

	if( this != &Src )
	{
		m_type = Src.m_type;
		m_entries = Src.m_entries;
		m_closestColour = Src.m_closestColour;
		SetDitherer( Src.m_pDitherer->GetDithererType() );
	}

	return( *this );
}

#if 0
bool ExAPBitmapPalette::operator==( const ExAPBitmapPalette & Src ) const
{
	return( m_type == Src.m_type  &&  m_entries == Src.m_entries );
}


bool ExAPBitmapPalette::operator!=( const ExAPBitmapPalette & Src ) const
{
	return( !( *this == Src ) );
}
#endif

void ExAPBitmapPalette::AddEntry( unsigned char r, unsigned char g, unsigned char b ) throw( ExAPException )
{
	AddEntry( ExAPBitmapRGBPaletteEntry( r, g, b ) );
}


void ExAPBitmapPalette::AddEntry( const ExAPBitmapRGBPaletteEntry & entry ) throw( ExAPException )
{

	if( m_type != PalCustom )
	{
		ThrowExAPException( ExAPException::CantAddToNonCustomPalette );
	}

	if( m_entries.size() >= 256 )
	{
		ThrowExAPException( ExAPException::PaletteIsFull );
	}

	m_entries.push_back( entry );
}


const ExAPBitmapRGBPaletteEntry & ExAPBitmapPalette::GetEntry( int i ) const throw( ExAPException )
{
	// Return an exception if out of bounds.

	if( i < 0  ||  i >= GetNumEntries() )
	{
		ThrowExAPException( ExAPException::ArrayIndexOutOfBounds );
	}

	return( m_entries[ std::vector<ExAPBitmapRGBPaletteEntry>::size_type( i ) ] );
} // ExAPBitmapPalette::GetEntry()


int ExAPBitmapPalette::GetNearestEntry( unsigned char r, unsigned char g, unsigned char b ) const throw( ExAPException )
{

	if( GetNumEntries() <= 0 )
	{
		ThrowExAPException( ExAPException::EmptyPalette );
	}

	const int ri = static_cast<int>( r );
	const int gi = static_cast<int>( g );
	const int bi = static_cast<int>( b );
	int indexOfNearest = 0;
	const int loopLimit = GetNumEntries();
	int nearestDistanceSquared = 3 * 255 * 255 + 1;

	for( int i = 0; i < loopLimit; ++i )
	{
		const ExAPBitmapRGBPaletteEntry & e = m_entries[ std::vector<ExAPBitmapRGBPaletteEntry>::size_type( i ) ];
		const int ri2 = static_cast<int>( e.GetRed() );
		const int gi2 = static_cast<int>( e.GetGreen() );
		const int bi2 = static_cast<int>( e.GetRed() );
		const int dr = ri - ri2;
		const int dg = gi - gi2;
		const int db = bi - bi2;
		const int distanceSquared = dr * dr + dg * dg + db * db;

		if( i == 0  ||  distanceSquared < nearestDistanceSquared )
		{
			indexOfNearest = i;
			nearestDistanceSquared = distanceSquared;
		}
	}

	return( indexOfNearest );
} // ExAPBitmapPalette::GetNearestEntry()


void ExAPBitmapPalette::GenerateClosestColourVector( void )
{
	const int numColoursInPalette = m_entries.size();

	if( numColoursInPalette < 2 )
	{
		return;
	}

	m_closestColour.clear();

	for( int i = 0; i < numColoursInPalette; ++i )
	{
		int closestIndex = -1;
		int closestDistanceSquared = -1;

		for( int j = 0; j < numColoursInPalette; ++j )
		{
			
			if( j == i )
			{
				continue;
			}

			const int distanceSquared = m_entries[i].SquareOfDistance( m_entries[j] );

			if( closestIndex == -1  ||  distanceSquared < closestDistanceSquared )
			{
				closestIndex = j;
				closestDistanceSquared = distanceSquared;
			}
		}

		m_closestColour.push_back( (unsigned char)closestIndex );
	}
}


void ExAPBitmapPalette::SetDefaultDitherer( void )
{
	SetDitherer( ExAPBitmapDither::DitherNone );
}


void ExAPBitmapPalette::SetDitherer( ExAPBitmapDither::DithererType d )
{
	ExAPBitmapDither * p = 0;

	try
	{

		switch( d )
		{
			case ExAPBitmapDither::DitherBayer:
				p = new ExAPBitmapDitherBayer();
				break;

			case ExAPBitmapDither::DitherErrorDiffusion:
				p = new ExAPBitmapDitherErrorDiffusion();
				break;

			case ExAPBitmapDither::DitherNone:
			default:
				p = new ExAPBitmapDitherNone();
				break;
		}
	}
	catch( ExAPException & )
	{
		throw;
	}
	catch( ... )
	{
	}

	m_pDitherer = p;
}


// **** End of File ****
