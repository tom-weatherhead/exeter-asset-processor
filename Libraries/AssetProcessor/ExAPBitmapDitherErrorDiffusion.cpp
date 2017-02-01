/*
 *  ExAPBitmapDitherErrorDiffusion.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Sat Apr 30 2005.
 *  Copyright (c) 2005 by Tom Weatherhead. All rights reserved.
 *
 */

#include "ExAPBitmapDitherErrorDiffusion.h"
#include "ExAPBitmapPalette.h"


// **** Class ExAPBitmapDitherErrorDiffusion ****


ExAPBitmapDitherErrorDiffusion::ExAPBitmapDitherErrorDiffusion( void )
: ExAPBitmapDither( ExAPBitmapDither::DitherErrorDiffusion ),
	  m_errorLineRed(),
	  m_errorLineGreen(),
	  m_errorLineBlue(),
	  m_errorRed( 0 ),
	  m_errorGreen( 0 ),
	  m_errorBlue( 0 )
{
}


ExAPBitmapDitherErrorDiffusion::~ExAPBitmapDitherErrorDiffusion( void )
{
}


unsigned char ExAPBitmapDitherErrorDiffusion::SaturateIntToByte( int i )
{

	if( i < 0 )
	{
		i = 0;
	}
	else if( i > 255 )
	{
		i = 255;
	}

	return( (unsigned char)i );
}


unsigned char ExAPBitmapDitherErrorDiffusion::DoError2DComponentInitSigned( int nData, int nError, ExAPCircularVector<short> & xErrorLine )
{
	int nWant = nData * 16;

	nWant += xErrorLine.GetPrevious() * 7;
	xErrorLine.SetPrevious( nError );
	xErrorLine.Push( xErrorLine.GetPrevious() );
	return( SaturateIntToByte( nWant / 16 ) );
}


unsigned char ExAPBitmapDitherErrorDiffusion::DoError2DComponentSigned( int nData, int nError, ExAPCircularVector<short> & xErrorLine )
{
	int nWant = nData * 16;

	/*	nWant   += nError * 7; */
	nWant += xErrorLine.GetPrevious();
	xErrorLine.SetPrevious( nError );
	nWant += xErrorLine.GetPrevious() * 7;
	/*	nWant   += *pnErrorLine++ * 5;  */
	nWant += xErrorLine.GetHead() * 5;
	xErrorLine.AdvanceToNext();
	/*	nWant   += *pnErrorLine * 3;  */
	nWant += xErrorLine.GetHead() * 3;
	return( SaturateIntToByte( nWant / 16 ) );
}


bool ExAPBitmapDitherErrorDiffusion::Chunkable( void ) const
{
	return( false );
}


unsigned char ExAPBitmapDitherErrorDiffusion::DitherPixel(
	const ExAPBitmapPalette & palette,
	unsigned char r, unsigned char g, unsigned char b,
	int nXBase, int nYBase, int nXOffset, int nYOffset )
{
	unsigned char r2 = 0;
	unsigned char g2 = 0;
	unsigned char b2 = 0;

	if( nXOffset == 0 )
	{
		m_errorRed = 0;
		m_errorGreen = 0;
		m_errorBlue = 0;
	}

	if( nYOffset == 0 )
	{

		if( nXOffset == 0 )
		{
			m_errorLineRed.push_back( 0 );
			m_errorLineGreen.push_back( 0 );
			m_errorLineBlue.push_back( 0 );
		}

		m_errorLineRed.push_back( 0 );
		m_errorLineGreen.push_back( 0 );
		m_errorLineBlue.push_back( 0 );
		r2 = DoError2DComponentInitSigned( r, m_errorRed, m_errorLineRed );
		g2 = DoError2DComponentInitSigned( g, m_errorGreen, m_errorLineGreen );
		b2 = DoError2DComponentInitSigned( b, m_errorBlue, m_errorLineBlue );
	}
	else
	{

		if( nXOffset == 0 )
		{
			m_errorLineRed.Push( 0 );
			m_errorLineGreen.Push( 0 );
			m_errorLineBlue.Push( 0 );
		}

		r2 = DoError2DComponentSigned( r, m_errorRed, m_errorLineRed );
		g2 = DoError2DComponentSigned( g, m_errorGreen, m_errorLineGreen );
		b2 = DoError2DComponentSigned( b, m_errorBlue, m_errorLineBlue );
	}

	const int index = palette.GetNearestEntry( r2, g2, b2 );
	const ExAPBitmapRGBPaletteEntry colour = palette.GetEntry( index );

	m_errorRed = (short)r2 - (short)colour.GetRed();
	m_errorGreen = (short)g2 - (short)colour.GetGreen();
	m_errorBlue = (short)b2 - (short)colour.GetBlue();

	m_errorLineRed.SetPrevious( m_errorRed );
	m_errorLineGreen.SetPrevious( m_errorGreen );
	m_errorLineBlue.SetPrevious( m_errorBlue );
	return( index );
}


// **** End of File ****
