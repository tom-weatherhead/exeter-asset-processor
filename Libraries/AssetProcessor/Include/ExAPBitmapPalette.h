/*
 *  ExAPBitmapPalette.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Fri Jun 20 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */


#ifndef _EXAPBITMAPPALETTE_H_
#define _EXAPBITMAPPALETTE_H_

//#define TAW_TRACE_PALETTE_COLOURS		1

#ifdef TAW_TRACE_PALETTE_COLOURS
#include <iostream>
#endif

#include <vector>

#include "General\ExAPAutoPtr.h"
#include "ExAPBitmapDither.h"


// **** Class ExAPBitmapRGBPaletteEntry ****


class ExAPBitmapRGBPaletteEntry
{
private:
	unsigned char m_red;
	unsigned char m_green;
	unsigned char m_blue;

public:

	ExAPBitmapRGBPaletteEntry( unsigned char r = 0, unsigned char g = 0, unsigned char b = 0 )
		: m_red( r ), m_green( g ), m_blue( b )
	{
	}

	ExAPBitmapRGBPaletteEntry( const ExAPBitmapRGBPaletteEntry & Src )
		: m_red( Src.m_red ), m_green( Src.m_green ), m_blue( Src.m_blue )
	{
	}

	inline bool operator==( const ExAPBitmapRGBPaletteEntry & Src ) const
	{
		return( m_red == Src.m_red  &&  m_green == Src.m_green  &&  m_blue == Src.m_blue );
	}

	inline bool operator!=( const ExAPBitmapRGBPaletteEntry & Src ) const
	{
		return( !( *this == Src ) );
	}

	inline ExAPBitmapRGBPaletteEntry & operator=( const ExAPBitmapRGBPaletteEntry & Src )
	{

		if( *this != Src )
		{
			m_red = Src.m_red;
			m_green = Src.m_green;
			m_blue = Src.m_blue;
		}

		return( *this );
	}

	inline unsigned char GetRed( void ) const
	{
		return( m_red );
	}

	inline unsigned char GetGreen( void ) const
	{
		return( m_green );
	}

	inline unsigned char GetBlue( void ) const
	{
		return( m_blue );
	}

	inline int SquareOfDistance( const ExAPBitmapRGBPaletteEntry & Src ) const
	{
		const int deltaRed = (int)m_red - (int)Src.m_red;
		const int deltaGreen = (int)m_green - (int)Src.m_green;
		const int deltaBlue = (int)m_blue - (int)Src.m_blue;

		return( deltaRed * deltaRed + deltaGreen * deltaGreen + deltaBlue * deltaBlue );
	}
}; // class ExAPBitmapRGBPaletteEntry


// **** Class ExAPBitmapPalette ****


class ExAPBitmapPalette
{
public:

	enum PaletteType
	{
		PalCustom,
		Pal6R6G6B20,
		PalBlackWhite,
		PalVGA,
		PalDuotone,
		Pal2BitGrey,
		Pal2BitCMYK,
		Pal2BitRGBW
	};

	PaletteType m_type;
	std::vector<ExAPBitmapRGBPaletteEntry> m_entries;
	std::vector<unsigned char> m_closestColour;
	ExAPAutoPtr<ExAPBitmapDither> m_pDitherer;

	ExAPBitmapPalette( PaletteType t );
	ExAPBitmapPalette( const ExAPBitmapRGBPaletteEntry & start, const ExAPBitmapRGBPaletteEntry & end );  // For duotones.
	ExAPBitmapPalette( const ExAPBitmapPalette & Src );
	ExAPBitmapPalette & operator=( const ExAPBitmapPalette & Src );
	//bool operator==( const ExAPBitmapPalette & Src ) const;
	//bool operator!=( const ExAPBitmapPalette & Src ) const;

	inline PaletteType GetPaletteType( void ) const
	{
		return( m_type );
	}

	inline int GetNumEntries( void ) const
	{
		return( m_entries.size() );
	}

	void AddEntry( unsigned char r, unsigned char g, unsigned char b ) throw( ExAPException );
	void AddEntry( const ExAPBitmapRGBPaletteEntry & entry ) throw( ExAPException );
	const ExAPBitmapRGBPaletteEntry & GetEntry( int i ) const throw( ExAPException );

	int GetNearestEntry( unsigned char r, unsigned char g, unsigned char b ) const throw( ExAPException );

	void GenerateClosestColourVector( void );

	void SetDefaultDitherer( void );
	void SetDitherer( ExAPBitmapDither::DithererType d );
}; // class ExAPBitmapPalette


#endif	// #ifndef _EXAPBITMAPPALETTE_H_


// **** End of File ****
