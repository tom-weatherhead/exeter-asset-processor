/*
 *  ExAPRect.h
 *  AssetProcessor
 *
 *  Created by Tom Weatherhead on Thu May 01 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#ifndef _EXAPRECT_H_
#define _EXAPRECT_H_


class ExAPPoint	// This could be renamed to ExAPVector.
{
private:
	int m_x;
	int m_y;
	
public:
	
	// Constructors.
	
	ExAPPoint( int x, int y )
		: m_x( x ), m_y( y )
	{
	}
	
	// Accessors.
	
	inline int getX( void ) const
	{
		return( m_x );
	}
	
	inline int getY( void ) const
	{
		return( m_y );
	}
	
	inline void setX( int x )
	{
		m_x = x;
	}
	
	inline void setY( int y )
	{
		m_y = y;
	}
	
	// **** Operations ****

	inline bool operator==( const ExAPPoint & src ) const
	{
		return( m_x == src.m_x  &&  m_y == src.m_y );
	}
	
	ExAPPoint & operator+=( const ExAPPoint & pt )
	{
		m_x += pt.m_x;
		m_y += pt.m_y;
		return( *this );
	}

	ExAPPoint operator+( const ExAPPoint & pt ) const
	{
		//return( ExAPPoint( getX() + pt.getX(), getY() + pt.getY() ) );
		return( ExAPPoint( m_x + pt.m_x, m_y + pt.m_y ) );
	}
	
	ExAPPoint & operator-=( const ExAPPoint & pt )
	{
		m_x -= pt.m_x;
		m_y -= pt.m_y;
		return( *this );
	}
	
	ExAPPoint operator-( const ExAPPoint & pt ) const
	{
		//return( ExAPPoint( getX() - pt.getX(), getY() - pt.getY() ) );
		return( ExAPPoint( m_x - pt.m_x, m_y - pt.m_y ) );
	}
}; // class ExAPPoint


class ExAPDimensions
{
private:
	int m_width;
	int m_height;

public:

	// **** Constructor ****
	
	ExAPDimensions( int width, int height )
		: m_width( width ), m_height( height )
	{
	}
	
	// **** Accessors ****
	
	inline int getWidth( void ) const
	{
		return( m_width );
	}
	
	inline int getHeight( void ) const
	{
		return( m_height );
	}
	
	inline void setWidth( int width )
	{
		m_width = width;
	}
	
	inline void setHeight( int height )
	{
		m_height = height;
	}

	// **** Operations ****

	inline bool operator==( const ExAPDimensions & src ) const
	{
		return( m_width == src.m_width  &&  m_height == src.m_height );
	}

	inline bool isEmpty( void ) const
	{
		return( m_width <= 0  ||  m_height <= 0 );
	}

	inline int GetArea( void ) const
	{

		if( isEmpty() )
		{
			return( 0 );
		}

		return( m_width * m_height );
	}
}; // class ExAPDimensions


class ExAPRect
{
private:
	ExAPPoint m_pt;
	ExAPDimensions m_dims;
	
public:

	ExAPRect( void )
		: m_pt( 0, 0 ), m_dims( 0, 0 )
	{
	}

	ExAPRect( int x, int y, int width, int height )
		: m_pt( x, y ), m_dims( width, height )
	{
	}

	ExAPRect( const ExAPPoint & pt, const ExAPDimensions & dims )
		: m_pt( pt ), m_dims( dims )
	{
	}
	
	// **** Accessors ****

	inline const ExAPPoint & getPoint( void ) const
	{
		return( m_pt );
	}

	inline const ExAPDimensions & getDimensions( void ) const
	{
		return( m_dims );
	}

	inline int getLeft( void ) const
	{
		return( m_pt.getX() );
	}
	
	inline int getRight( void ) const
	{
		return( m_pt.getX() + m_dims.getWidth() );
	}
	
	inline int getTop( void ) const
	{
		return( m_pt.getY() );
	}
	
	inline int getBottom( void ) const
	{
		return( m_pt.getY() + m_dims.getHeight() );
	}

	inline void setLeft( int l )
	{
		m_dims.setWidth( getRight() - l );
		m_pt.setX( l );
	}
	
	inline void setRight( int r )
	{
		m_dims.setWidth( r - m_pt.getX() );
	}
	
	inline void setTop( int t )
	{
		m_dims.setHeight( getBottom() - t );
		m_pt.setY( t );
	}
	
	inline void setBottom( int b )
	{
		m_dims.setHeight( b - m_pt.getY() );
	}
	
	// **** Operations ****

	inline bool operator==( const ExAPRect & src ) const
	{
		return( m_pt == src.m_pt  &&  m_dims == src.m_dims );
	}

	inline int getWidth( void ) const
	{
		return( m_dims.getWidth() );
	}
	
	inline int getHeight( void ) const
	{
		return( m_dims.getHeight() );
	}
	
	inline bool isEmpty( void ) const
	{
		return( m_dims.isEmpty() );
	}
	
	inline bool containsPoint( const ExAPPoint & pt ) const
	{
		const ExAPPoint pt2 = pt - m_pt;

		return( pt2.getX() >= 0  &&  pt2.getX() < m_dims.getWidth()
			&&  pt2.getY() >= 0  &&  pt2.getY() < m_dims.getHeight() );
	}
	
	ExAPRect & intersect( const ExAPRect & r )
	{
		const int rl = r.getLeft();
		const int rr = r.getRight();
		const int rt = r.getTop();
		const int rb = r.getBottom();

		if( getLeft() < rl )
		{
			setLeft( rl );
		}
		
		if( getRight() > rr )
		{
			setRight( rr );
		}
		
		if( getTop() < rt )
		{
			setTop( rt );
		}
		
		if( getBottom() > rb )
		{
			setBottom( rb );
		}
		
		return( *this );
	}
	
	inline ExAPRect & offset( const ExAPPoint & pt )
	{
		m_pt += pt;
		return( *this );
	}
	
	inline ExAPRect & operator+=( const ExAPPoint & pt )
	{
		return( offset( pt ) );
	}

	inline ExAPRect & offset( int deltaX, int deltaY )
	{
		return( offset( ExAPPoint( deltaX, deltaY ) ) );
	}

	inline int GetArea( void ) const
	{
		return( m_dims.GetArea() );
	}
}; // class ExAPRect


inline ExAPRect intersect( const ExAPRect & r1, const ExAPRect & r2 )
{
	ExAPRect r3( r1 );
	
	r3.intersect( r2 );
	return( r3 );
}


#endif //_EXAPRECT_H_


// **** End of File ****
