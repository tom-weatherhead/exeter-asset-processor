/*
 *  ExAPBitmapDitherErrorDiffusion.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Sat Apr 30 2005.
 *  Copyright (c) 2005 by Tom Weatherhead. All rights reserved.
 *
 */

#include <vector>
#include "ExAPBitmapDither.h"


template<class X> class ExAPCircularVector
{
private:
	std::vector<X> m_vector;
	int m_length;
	int m_head;

	inline int GetPreviousHead( void ) const
	{

		if( m_head > 0 )
		{
			return( m_head - 1 );
		}

		return( m_length - 1 );
	}

	inline int GetNextHead( void ) const
	{
		return( ( m_head + 1 ) % m_length );
	}

public:
	inline ExAPCircularVector( void /* int length, const X & initializer = 0 */ )
		: //m_vector( length, initializer ),
		  m_length( 0 /* length */ ),
		  m_head( 1 )
	{
	}

	inline const X & GetHead( void ) const
	{
		return( m_vector[m_head] );
	}

	inline void SetHead( const X & Src )
	{
		m_vector[m_head] = Src;
	}

	inline void AdvanceToNext( void )
	{
		m_head = GetNextHead();
	}

	inline void Push( const X & Src )		// SetHead and advance head.
	{
		SetHead( Src );
		AdvanceToNext();
	}

	inline const X & GetPrevious( void ) const
	{
		return( m_vector[GetPreviousHead()] );
	}

	inline void SetPrevious( const X & Src )
	{
		m_vector[GetPreviousHead()] = Src;
	}

	inline void push_back( const X & Src )
	{
		m_vector.push_back( Src );
		++m_length;
	}
};


class ExAPBitmapDitherErrorDiffusion : public ExAPBitmapDither
{
private:
	ExAPCircularVector<short> m_errorLineRed;
	ExAPCircularVector<short> m_errorLineGreen;
	ExAPCircularVector<short> m_errorLineBlue;
	short m_errorRed;
	short m_errorGreen;
	short m_errorBlue;

	static unsigned char SaturateIntToByte( int i );
	static unsigned char DoError2DComponentInitSigned( int nData, int nError, ExAPCircularVector<short> & xErrorLine );
	static unsigned char DoError2DComponentSigned( int nData, int nError, ExAPCircularVector<short> & xErrorLine );

public:
	ExAPBitmapDitherErrorDiffusion( void );
	virtual ~ExAPBitmapDitherErrorDiffusion( void );

	virtual bool Chunkable( void ) const;
	virtual unsigned char DitherPixel(
		const ExAPBitmapPalette & palette,
		unsigned char r, unsigned char g, unsigned char b,
		int nXBase, int nYBase, int nXOffset, int nYOffset );
};


// **** End of File ****
