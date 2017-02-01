/*
 *  ExCryptoBigNumInterface.h
 *  Exeter Cryptological Library
 *
 *  Created by Tom Weatherhead on Tue May 31 2005.
 *  Copyright (c) 2005 by Tom Weatherhead. All rights reserved.
 *
 */

#pragma once

//#ifndef _EXCRYPTOBIGNUMINTERFACE_H_
//#define _EXCRYPTOBIGNUMINTERFACE_H_

#include "General\\ExAPAutoPtr.h"


// The factory object returns an autoptr to a ReferenceObjectInterface which points to both the factory object
// and a BigNumInterface.  The destructor of the ReferenceObjectInterface class releases the BigNumInterface
// back to the factory object for reuse.


// 1) Interface for BigNum class

class ExCryptoBigNumInterface
{
public:
	virtual ~ExCryptoBigNumInterface( void );

	//virtual int GetNumBits( void ) const = 0;	// GetSize, not GetCapacity.
	//virtual void TrimLeadingZeros( void ) = 0;
	//virtual bool BitTest( int bitNum ) const = 0;
	//virtual void BitSet( int bitNum, bool newBitState ) = 0;
	//virtual void ShiftLeftByOne( void ) = 0;
	//virtual void ShiftRightByOne( void ) = 0;
	//virtual bool IsGreaterThan( const ExCryptoBigNumInterface * pBigNum ) const = 0;
	//virtual void Add( const ExCryptoBigNumInterface * pBigNum ) = 0;
	//virtual void Subtract( const ExCryptoBigNumInterface * pBigNum ) = 0;
	//virtual void ModularMultiply( const ExCryptoBigNumInterface * pBigNum, const ExCryptoBigNumInterface * pMod ) = 0;	// *this := ( *this * *pBigNum ) % *pMod.

	// *this := ( *pBase ^ *pExponent ) % *pMod.
	virtual void SetToModularExponentiation( const ExCryptoBigNumInterface * pBase, const ExCryptoBigNumInterface * pExponent, const ExCryptoBigNumInterface * pMod ) = 0;

	virtual bool MultiplicativeInverse( const ExCryptoBigNumInterface * pGenericN, ExCryptoBigNumInterface * pGenericX ) const = 0;

	//ReadFromFile(  );
	//WriteToFile(  );
};


// 2) Interface for BigNumRefObj class

class ExCryptoBigNumRefObjInterface
{
public:
	virtual ~ExCryptoBigNumRefObjInterface( void );

	virtual ExCryptoBigNumInterface * GetBigNum( void ) const = 0;
	//virtual ExAPAutoPtr<ExCryptoBigNumRefObjInterface> CreateCopy( void ) const = 0;
};


// 3) Interface for BigNumFactory class: it creates a BigNum of a given length, or reuses a cached one.

class ExCryptoBigNumFactoryInterface
{
public:
	virtual ~ExCryptoBigNumFactoryInterface( void );

	virtual ExAPAutoPtr<ExCryptoBigNumRefObjInterface> GetBigNum( int numBits ) = 0;
	virtual void CacheBigNum( ExCryptoBigNumInterface * pBigNum ) = 0;
};


//#endif


// End of File
