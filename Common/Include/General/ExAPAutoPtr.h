/*SDOC************************************************************************

	$Header: /usr/local/cvsrep/Daring/Common/Include/General/ExAPAutoPtr.h,v 1.1 2004/08/29 20:35:15 tomw Exp $

	Module: ExAPAutoPtr.h

	Author:	Tom Weatherhead

	Description: Contains an auto_ptr-like implementation; an auto_ptr behaves
	like a regular pointer, except that the object to which it points is destroyed
	when the auto_ptr is destroyed.  This implementation uses a reference counting
	system, so that several auto_ptr objects can refer to the same dynamically
	allocated resource, which is destroyed when there are no longer any auto_ptr
	objects referring to it.

************************************************************************EDOC*/

/*SDOC************************************************************************

  Revision Record

	Rev		Date				Auth	Changes
	===		====				====	=======
		0		2002/03/17	TAW		Created.

************************************************************************EDOC*/


#ifndef _EXAPAUTOPTR_H_
#define _EXAPAUTOPTR_H_

#include "General\\ExAPException.h"


// auto_ptr example: Stroustrup, Special Edition, p. 368.

// TAW 2002/03/16 : New version of ExAPAutoPtr which uses ExAPAutoPtrRefs.


// TAW 2002/03/26 : Allow ptr to create a new copy of the referred object.
#define AUTOPTR_SUPPORT_SPLIT		1


// ***************************************
// **** class template ExAPAutoPtrRef ****
// ***************************************


template<class X> class ExAPAutoPtrRef
{
private:
	X * m_ptr;								// This member could be const.
	int m_nReferenceCount;		// This member could be unsigned.
	bool m_ownResource;				// If true, we may and should delete the resource when the reference count reaches zero.

	// Private copy constructor; ie. disallow copy construction.

	inline ExAPAutoPtrRef(
		const ExAPAutoPtrRef & Src
		) //throw( CException )
		: m_ptr( 0 ),
			m_nReferenceCount( 0 ),
			m_ownResource( false )
	{
		//ThrowException( eStatus_NotImplemented );
		// Because of the exception, the destructor won't be run,
		// so the zero reference count isn't a problem.
	}

	// Private assignment operator; ie. disallow assignment.

	inline ExAPAutoPtrRef & operator=(
		const ExAPAutoPtrRef & Src
		) //throw( CException )
	{

		if( &Src != this )
		{
			// Don't modify this object; just throw the exception.
			//ThrowException( eStatus_NotImplemented );
		}

		return( *this );
	}

public:
	//typedef X element_type;

	// Main (default) constructor.

	explicit inline ExAPAutoPtrRef(
		X * ptr = 0
		) throw()
		: m_ptr( ptr ),
			m_nReferenceCount( 1 ),
			m_ownResource( true )
	{
	}

	// Destructor.

	virtual ~ExAPAutoPtrRef( void ) throw()
	{
		//AssertCS( m_pClientServices, m_nReferenceCount == 0 );

		if( m_ownResource )
		{
			delete m_ptr;	// TAW 2002/03/16 : Should have no effect on a zero pointer.
		}

		m_ptr = 0;
	}

	// Accessor functions.

	inline X * GetPtr( void ) const throw()
	{
		return( m_ptr );
	}

	inline void IncrementReferenceCount( void ) throw()
	{
		//AssertCS( m_pClientServices, m_nReferenceCount > 0 );
		m_nReferenceCount++;
	}

	inline bool DecrementReferenceCount( void ) throw()
	{
		//AssertCS( m_pClientServices, m_nReferenceCount > 0 );
		// Destroy this object if and only if true is returned here.
		return( ( --m_nReferenceCount <= 0 ) ? true : false );
	}

	inline bool ReferenceCountIsOne( void ) throw()
	{
		//AssertCS( m_pClientServices, m_nReferenceCount > 0 );
		// TAW 2002/03/27 : Use "<= 1" instead of "== 1" for safety (Paranoid).
		return( ( m_nReferenceCount <= 1 ) ? true : false );
	}

	void SetResourceOwnership( bool own )
	{
		m_ownResource = own;
	}
}; // ExAPAutoPtrRef


// ************************************
// **** class template ExAPAutoPtr ****
// ************************************


template<class X> class ExAPAutoPtr
{
protected:
	ExAPAutoPtrRef<X> * m_pRef;

	inline X * GetPtrFromRef( void ) const throw()
	{
		return( ( m_pRef != 0 ) ? m_pRef->GetPtr() : 0 );
	}

	void Reset( X * p = 0 ) //throw( CException )
	{
		
		if( m_pRef != 0 )
		{
			
			if( p == GetPtrFromRef() )
			{
				return;	// We're already in the correct state.
			}

			if( m_pRef->DecrementReferenceCount() )
			{
				delete m_pRef;
			}

			// To prevent double-freeing the old pointer in case the following ExAPAutoPtrRef construction fails.
			// We want to set m_pRef to zero even if we haven't deleted the object to which it points;
			// we simply no longer refer to it.
			m_pRef = 0;
		}

		if( p != 0 )
		{

			try
			{
				// ExAPAutoPtrRef<X> constructor won't throw any exceptions, but the "new" might.
				m_pRef = new ExAPAutoPtrRef<X>( p );
			}
			catch( ... )
			{
			}

			if( m_pRef == 0 )
			{
				//SignalCS( m_pClientServices );
				//ThrowException( eStatus_ResourceAcquisitionFailed );
			}
		}
	} // Reset()

public:
	//typedef X element_type;

	// Main (default) constructor.

	inline ExAPAutoPtr( X * p = 0 ) //throw( CException )
		: m_pRef( 0 )
	{
		Reset( p );
	} // Main (default) constructor.

	// Public copy constructor.

	ExAPAutoPtr( const ExAPAutoPtr & Src ) throw()
		: m_pRef( Src.m_pRef )
	{

		if( m_pRef != 0 )
		{
			m_pRef->IncrementReferenceCount();
		}
	} // Copy constructor.

	// Destructor.

	virtual ~ExAPAutoPtr( void ) throw()
	{

		if( m_pRef != 0  &&  m_pRef->DecrementReferenceCount() )
		{
			delete m_pRef;
		}

		m_pRef = 0;
	} // Destructor.

	// Public assignment operator.

	inline ExAPAutoPtr & operator=( const ExAPAutoPtr & Src ) throw()
	{

		if( &Src != this )
		{
			//m_pClientServices = Src.m_pClientServices;

			if( m_pRef != 0  &&  m_pRef->DecrementReferenceCount() )
			{
				delete m_pRef;
			}

			m_pRef = Src.m_pRef;

			if( m_pRef != 0 )
			{
				m_pRef->IncrementReferenceCount();
			}
		}

		return( *this );
	} // Assignment operator.

	// Other functions.

	// Implicit cast of ExAPAutoPtr<X> to X *.

	operator X *( void ) const throw()
	{
		return( GetPtrFromRef() );
	}

	X & operator*() const //throw( CException )
	{
		X * ptr = GetPtrFromRef();

		if( ptr == 0 )
		{
			//SignalCS( m_pClientServices );
			//ThrowException( eStatus_InternalError );
		}

		return *ptr;
	}

	X * operator->() const //throw( CException )
	{
		X * ptr = GetPtrFromRef();

		if( ptr == 0 )
		{
			//SignalCS( m_pClientServices );
			//ThrowException( eStatus_InternalError );
		}

		return ptr;
	} // operator->()

	// TAW 2002/03/09 : I added these 3, to make ExAPAutoPtr behave more like a real pointer.

	inline bool operator==( X * p ) const throw()
	{
		return( p == GetPtrFromRef() );
	}

	inline bool operator!=( X * p ) const throw()
	{
		return( p != GetPtrFromRef() );
	}

	inline ExAPAutoPtr & operator=( X * p ) //throw( CException )
	{
		Reset( p );
		return( *this );
	}

#ifdef AUTOPTR_SUPPORT_SPLIT
	void EnsurePrivateCopy( void ) //throw( CException )
	{
		X * pOld = GetPtrFromRef();
		X * pNew = 0;

		if( pOld == 0  ||  m_pRef == 0  ||  m_pRef->ReferenceCountIsOne() )
		{
			// Nothing to do; either there is no object, or
			// this AutoPtr is already the sole owner of the object.
			return;
		}

		try
		{
			// X's copy constructor is used here.
			pNew = new X( *pOld );
		}
		catch( const ExAPException & )
		{
			throw;
		}
		catch( ... )
		{
			// Do nothing; pNew is already zero, and we'll throw an exception below.
		}

		if( pNew == 0 )
		{
			//SignalCS( m_pClientServices );
			//ThrowException( eStatus_ResourceAcquisitionFailed );
		}

		Reset( pNew );
	}
#endif

	void SetResourceOwnership( bool own ) const
	{

		if( m_pRef != 0 )
		{
			m_pRef->SetResourceOwnership( own );
		}
	}
}; // ExAPAutoPtr


// **************************************
// **** template class ExAPAutoArray ****
// **************************************

// This class is not reference counted; it does not support object sharing like an AutoPtr does.
// It simply facilitates the freeing of arrays when an exception is thrown.


template<class X> class ExAPAutoArray
{
private:
	X * m_ptr;
	int m_size;

public:

	explicit ExAPAutoArray( int size )
		: m_ptr( 0 ),
			m_size( size )
	{

		if( m_size <= 0 )
		{
			ThrowExAPException( ExAPException::IllegalParameterValue );
		}

		try
		{
			m_ptr = new X[static_cast<size_t>( m_size )];
		}
		catch( ... )
		{
			// Do nothing.
		}

		if( m_ptr == 0 )
		{
			ThrowExAPException( ExAPException::ResourceAcquisitionFailed );
		}
	}

	~ExAPAutoArray( void )
	{
		delete [] m_ptr;
		m_ptr = 0;
	}

	operator X *( void ) const
	{
		return( m_ptr );
	}

	X & operator[]( int i ) const
	{

		if( i < 0  ||  i >= m_size )
		{
			ThrowExAPException( ExAPException::ArrayIndexOutOfBounds );
		}

		return( m_ptr[i] );
	}
}; // ExAPAutoArray


#endif	//#ifndef _EXAPAUTOPTR_H_


// **** End of File ****
