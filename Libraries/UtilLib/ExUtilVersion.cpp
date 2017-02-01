// ExUtilVersion.cpp - Originally part of Helix
// Copyright (c) 2002-2005 by Tom Weatherhead.  All rights reserved.
// Started December 17, 2002

#include "UtilLib\\ExUtilVersionInterface.h"


// **** Class ExUtilVersion ****


class ExUtilVersion : public ExUtilVersionInterface
{
private:
	typedef bool (ExUtilVersion::* PComparator)( const ExUtilVersion & Src ) const;

	bool Comparator( PComparator pfn, ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const;

	bool operator<( const ExUtilVersion & Src ) const;
	bool operator>( const ExUtilVersion & Src ) const;
	bool operator!=( const ExUtilVersion & Src ) const;
	bool operator==( const ExUtilVersion & Src ) const;
	bool operator<=( const ExUtilVersion & Src ) const;
	bool operator>=( const ExUtilVersion & Src ) const;

	unsigned int m_unPrimaryVersion;
	unsigned int m_unSecondaryVersion;
	unsigned int m_unTertiaryVersion;

public:
	ExUtilVersion( unsigned int v1 = 0, unsigned int v2 = 0, unsigned int v3 = 0 );
	//ExUtilVersion( FILE * pSrcFile );
	virtual ~ExUtilVersion( void );

	virtual void ReadFromFile( ExAPAutoPtr<ExUtilFileReaderBaseInterface> pSrcFile );
	virtual void WriteToFile( ExAPAutoPtr<ExUtilFileWriterBaseInterface> pDstFile ) const;
	//virtual void Print( void ) const;

	virtual bool operator<( ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const;
	virtual bool operator>( ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const;
	virtual bool operator!=( ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const;
	virtual bool operator==( ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const;
	virtual bool operator<=( ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const;
	virtual bool operator>=( ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const;

	virtual unsigned int GetPrimaryVersion( void ) const;
	virtual unsigned int GetSecondaryVersion( void ) const;
	virtual unsigned int GetTertiaryVersion( void ) const;
}; // class ExUtilVersion


ExUtilVersion::ExUtilVersion( unsigned int v1, unsigned int v2, unsigned int v3 )
	: m_unPrimaryVersion( v1 ),
		m_unSecondaryVersion( v2 ),
		m_unTertiaryVersion( v3 )
{
}


#if 0
CVersion::CVersion( FILE * pSrcFile )
{
	ReadFromFile( pSrcFile );
}
#endif


ExUtilVersion::~ExUtilVersion( void )
{
}


void ExUtilVersion::ReadFromFile( ExAPAutoPtr<ExUtilFileReaderBaseInterface> pSrcFile )
{
#if 1
	m_unPrimaryVersion = pSrcFile->GetDoubleWord();
	m_unSecondaryVersion = pSrcFile->GetDoubleWord();
	m_unTertiaryVersion = pSrcFile->GetDoubleWord();
#else
	unsigned int aunV[3];
	const int knIntsRead = fread( aunV, sizeof( unsigned int ), 3, pSrcFile );

	if( knIntsRead != 3 )
	{
		Signal();
		ThrowHelixException( "Failure to read version from file." );
	}

#ifdef BIG_ENDIAN
	
	for( int i = 0; i < 3; ++i )
	{
		ByteSwapUnsignedInt( aunV[i] );
	}
#endif

	m_unPrimaryVersion = aunV[0];
	m_unSecondaryVersion = aunV[1];
	m_unTertiaryVersion = aunV[2];
#endif
}


void ExUtilVersion::WriteToFile( ExAPAutoPtr<ExUtilFileWriterBaseInterface> pDstFile ) const
{
#if 1
	pDstFile->WriteDoubleWord( m_unPrimaryVersion );
	pDstFile->WriteDoubleWord( m_unSecondaryVersion );
	pDstFile->WriteDoubleWord( m_unTertiaryVersion );
#else
	int aunV[3];

	aunV[0] = m_unPrimaryVersion;
	aunV[1] = m_unSecondaryVersion;
	aunV[2] = m_unTertiaryVersion;

#ifdef BIG_ENDIAN
	
	for( int i = 0; i < 3; ++i )
	{
		ByteSwapUnsignedInt( aunV[i] );
	}
#endif

	const int knIntsWritten = fwrite( aunV, sizeof( unsigned int ), 3, pDstFile );

	if( knIntsWritten != 3 )
	{
		Signal();
		ThrowHelixException( "Failure to write version to file." );
	}
#endif
}


#if 0
void CVersion::Print( void ) const
{
	printf( "%d.%d.%d", m_unPrimaryVersion, m_unSecondaryVersion, m_unTertiaryVersion );
}
#endif


// Comparison Operators


bool ExUtilVersion::Comparator( PComparator pfn, ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const
{
	ExUtilVersionInterface * pSrcInterfaceRaw = pSrc;
	ExUtilVersion * pSrcRaw = dynamic_cast<ExUtilVersion *>( pSrcInterfaceRaw );

	if( pSrcRaw == 0 )
	{
		ThrowExAPException( ExAPException::DynamicCastFailed );
	}

	return( (this->*pfn)( *pSrcRaw ) );
}


bool ExUtilVersion::operator<( const ExUtilVersion & Src ) const
{
	return(
		m_unPrimaryVersion < Src.m_unPrimaryVersion  ||
		( m_unPrimaryVersion == Src.m_unPrimaryVersion  &&
			m_unSecondaryVersion < Src.m_unSecondaryVersion )  ||
		( m_unPrimaryVersion == Src.m_unPrimaryVersion  &&
			m_unSecondaryVersion == Src.m_unSecondaryVersion  &&
			m_unTertiaryVersion < Src.m_unTertiaryVersion ) );
}


bool ExUtilVersion::operator>( const ExUtilVersion & Src ) const
{
	return( Src < *this );
}


bool ExUtilVersion::operator!=( const ExUtilVersion & Src ) const
{
	return( !( *this == Src ) );
}


bool ExUtilVersion::operator==( const ExUtilVersion & Src ) const
{
	return(
		m_unPrimaryVersion == Src.m_unPrimaryVersion  &&
		m_unSecondaryVersion == Src.m_unSecondaryVersion  &&
		m_unTertiaryVersion == Src.m_unTertiaryVersion );
}


bool ExUtilVersion::operator<=( const ExUtilVersion & Src ) const
{
	return( !( Src < *this ) );
}


bool ExUtilVersion::operator>=( const ExUtilVersion & Src ) const
{
	return( !( *this < Src ) );
}


bool ExUtilVersion::operator<( ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const
{
	return( Comparator( &ExUtilVersion::operator<, pSrc ) );
}


bool ExUtilVersion::operator>( ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const
{
	return( Comparator( &ExUtilVersion::operator>, pSrc ) );
}


bool ExUtilVersion::operator!=( ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const
{
	return( Comparator( &ExUtilVersion::operator!=, pSrc ) );
}


bool ExUtilVersion::operator==( ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const
{
	return( Comparator( &ExUtilVersion::operator==, pSrc ) );
}


bool ExUtilVersion::operator<=( ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const
{
	return( Comparator( &ExUtilVersion::operator<=, pSrc ) );
}


bool ExUtilVersion::operator>=( ExAPAutoPtr<ExUtilVersionInterface> pSrc ) const
{
	return( Comparator( &ExUtilVersion::operator>=, pSrc ) );
}


unsigned int ExUtilVersion::GetPrimaryVersion( void ) const
{
	return( m_unPrimaryVersion );
}


unsigned int ExUtilVersion::GetSecondaryVersion( void ) const
{
	return( m_unSecondaryVersion );
}


unsigned int ExUtilVersion::GetTertiaryVersion( void ) const
{
	return( m_unTertiaryVersion );
}


// **** Class ExUtilVersionInterface ****


ExUtilVersionInterface::~ExUtilVersionInterface( void )
{
}


ExAPAutoPtr<ExUtilVersionInterface> ExUtilVersionInterface::CreateVersionObject( unsigned int v1, unsigned int v2, unsigned int v3 )
{
	ExAPAutoPtr<ExUtilVersionInterface> p;

	try
	{
		p = new ExUtilVersion( v1, v2, v3 );
	}
	catch( ExAPException & )
	{
		throw;
	}
	catch( ... )
	{
		ThrowExAPException( ExAPException::UnknownException );
	}

	if( p == 0 )
	{
		ThrowExAPException( ExAPException::ResourceAcquisitionFailed );
	}

	return( p );
}


// **** End of File ****
