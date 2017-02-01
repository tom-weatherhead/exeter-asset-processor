// ExUtilVersion.h - Originally part of Helix
// Copyright (c) 2002-2005 by Tom Weatherhead.  All rights reserved.
// Started December 17, 2002


#pragma once
//#ifndef _HVERSION_H_
//#define _HVERSION_H_


class ExUtilVersionInterface
{
public:
	virtual ~ExUtilVersionInterface( void );

	static ExAPAutoPtr<ExUtilVersionInterface> CreateVersionObject( unsigned int v1 = 0, unsigned int v2 = 0, unsigned int v3 = 0 );
	//ExUtilVersion( FILE * pSrcFile );

	virtual void ReadFromFile( ExAPAutoPtr<ExUtilReaderBaseInterface> pSrcFile ) = 0;
	virtual void WriteToFile( ExAPAutoPtr<ExUtilWriterBaseInterface> pDstFile ) const = 0;
	//virtual void Print( void ) const = 0;

	virtual bool operator<( ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const = 0;
	virtual bool operator>( ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const = 0;
	virtual bool operator!=( ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const = 0;
	virtual bool operator==( ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const = 0;
	virtual bool operator<=( ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const = 0;
	virtual bool operator>=( ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const = 0;
}; // class ExUtilVersionInterface


#if 0
class ExUtilComparator : public ExUtilVersionInterface
{
public:
	virtual ~ExUtilComparator( void );

	virtual bool operator>( const ExUtilVersionInterface & Src ) const;
	virtual bool operator!=( const ExUtilVersionInterface & Src ) const;
	virtual bool operator==( const ExUtilVersionInterface & Src ) const;
	virtual bool operator<=( const ExUtilVersionInterface & Src ) const;
	virtual bool operator>=( const ExUtilVersionInterface & Src ) const;
}; // class ExUtilComparator
#endif


class ExUtilVersion : public ExUtilVersionInterface
{
private:
	unsigned int m_unPrimaryVersion;
	unsigned int m_unSecondaryVersion;
	unsigned int m_unTertiaryVersion;

public:
	ExUtilVersion( unsigned int v1 = 0, unsigned int v2 = 0, unsigned int v3 = 0 );
	//ExUtilVersion( FILE * pSrcFile );
	virtual ~ExUtilVersion( void );

	virtual void ReadFromFile( ExAPAutoPtr<ExUtilReaderBaseInterface> pSrcFile );
	virtual void WriteToFile( ExAPAutoPtr<ExUtilWriterBaseInterface> pDstFile ) const;
	//virtual void Print( void ) const;

	virtual bool operator<( ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const;
	virtual bool operator>( ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const;
	virtual bool operator!=( ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const;
	virtual bool operator==( ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const;
	virtual bool operator<=( ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const;
	virtual bool operator>=( ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const;

#if 0
	inline unsigned int GetPrimaryVersion( void ) const
	{
		return( m_unPrimaryVersion );
	}

	inline unsigned int GetSecondaryVersion( void ) const
	{
		return( m_unSecondaryVersion );
	}

	inline unsigned int GetTertiaryVersion( void ) const
	{
		return( m_unTertiaryVersion );
	}
#endif
}; // class ExUtilVersion


//#endif // _HVERSION_H_


// **** End of File ****
