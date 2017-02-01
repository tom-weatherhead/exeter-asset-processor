/*
 *  ExAPColourModel.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Sat May 03 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#ifndef _EXAPCOLOURMODEL_H_
#define _EXAPCOLOURMODEL_H_


class ExAPColourModel
{
public:

	enum ModelType
	{
		Paletted,
		Grey8,
		RGB24,
		RGBA32,
		CMYK32,
		RGBA64
	};

private:
	const ModelType m_type;
	const int m_bytesPerPixel;
	// Also consider the colour profile.

public:

	// Constructor.
	ExAPColourModel( ModelType type ) throw( ExAPException );

	static int CalculateBitsPerPixel( ModelType type ) throw( ExAPException );
	static int CalculateBytesPerPixel( ModelType type ) throw( ExAPException );

	inline bool operator==( const ExAPColourModel & cm ) const
	{
		// Also consider the colour profile.
		return( m_type == cm.m_type );
	}
	
	inline bool operator!=( const ExAPColourModel & cm ) const
	{
		return( !( *this == cm ) );
	}
	
	inline ModelType GetModelType( void ) const
	{
		return( m_type );
	}

	inline int getBytesPerPixel( void ) const
	{
		return( m_bytesPerPixel );
	}
}; // class ExAPColourModel


#endif //_EXAPCOLOURMODEL_H_


// **** End of File ****
