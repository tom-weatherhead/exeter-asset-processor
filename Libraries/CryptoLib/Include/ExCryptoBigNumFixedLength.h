/*
 *  ExCryptoBigNumFixedLength.h
 *  Exeter Cryptological Library
 *
 *  Created by Tom Weatherhead on Tue Jun 07 2005.
 *  Copyright (c) 2005 by Tom Weatherhead. All rights reserved.
 *
 */

#pragma once

#include <vector>
#include "ExCryptoBigNumInterface.h"


class ExCryptoBigNumFixedLengthFactory;


class ExCryptoBigNumFixedLength : public ExCryptoBigNumInterface
{
private:
	ExCryptoBigNumFixedLengthFactory * m_pFactory;
	unsigned char * m_pBuffer;
	const int m_byteCapacity;	// Size of buffer in bytes
	const int m_bitCapacity;	// Size of buffer in bits
	int m_bitSize;			// Number of bits currently in use, excluding leading zeros.

	ExCryptoBigNumFixedLengthFactory * GetFactory() const;
	int GetNumBits( void ) const;	// GetSize, not GetCapacity.
	void TrimLeadingZeros( void );
	bool BitTest( int bitNum ) const;
	void BitSet( int bitNum, bool newBitState );
	void ShiftLeftByOne( void );
	void ShiftRightByOne( void );
	bool IsGreaterThan( const ExCryptoBigNumFixedLength & bigNum ) const;
	void Add( const ExCryptoBigNumFixedLength & bigNum );
	void Subtract( const ExCryptoBigNumFixedLength & bigNum );
	void Multiply( const ExCryptoBigNumFixedLength & x1, const ExCryptoBigNumFixedLength & x2 );
	void ModularMultiply( const ExCryptoBigNumFixedLength & bigNum, const ExCryptoBigNumFixedLength & mod );	// *this := ( *this * *pBigNum ) % *pMod.
	void Divide( const ExCryptoBigNumFixedLength & divisor, ExCryptoBigNumFixedLength * pQuotient, ExCryptoBigNumFixedLength * pRemainder ) const;
	void SetToRandomValue( int numBits, bool bEnforceOdd );
	bool MillerRabinWitness( const ExCryptoBigNumFixedLength & a ) const;
	bool MillerRabinIsComposite( int s ) const;

	bool operator==( const ExCryptoBigNumFixedLength & src ) const;
	bool operator!=( const ExCryptoBigNumFixedLength & src ) const;
	bool operator==( unsigned int n ) const;
	bool operator!=( unsigned int n ) const;

public:
	ExCryptoBigNumFixedLength( ExCryptoBigNumFixedLengthFactory * pFactory, int bitCapacity );
	virtual ~ExCryptoBigNumFixedLength( void );

	ExCryptoBigNumFixedLength & operator=( const ExCryptoBigNumFixedLength & src );
	ExCryptoBigNumFixedLength & operator=( unsigned int unSrc );

	int GetBitCapacity( void ) const;
	//virtual int GetNumBits( void ) const;	// GetSize, not GetCapacity.
	//virtual void TrimLeadingZeros( void );
	//virtual bool BitTest( int bitNum ) const;
	//virtual void BitSet( int bitNum, bool newBitState );
	//virtual void ShiftLeftByOne( void );
	//virtual void ShiftRightByOne( void );
	//virtual bool IsGreaterThan( const ExCryptoBigNumInterface * pBigNum ) const;
	//virtual void Add( const ExCryptoBigNumInterface * pBigNum );
	//virtual void Subtract( const ExCryptoBigNumInterface * pBigNum );
	//virtual void ModularMultiply( const ExCryptoBigNumInterface * pBigNum, const ExCryptoBigNumInterface * pMod );	// *this := ( *this * *pBigNum ) % *pMod.
	void SetToRandomPrime( int nPrimeBitLength /*, int nWhichPrime */ );
	static void ExtendedEuclid( const ExCryptoBigNumFixedLength & a, const ExCryptoBigNumFixedLength & b,
		ExCryptoBigNumFixedLength & d, ExCryptoBigNumFixedLength & x, ExCryptoBigNumFixedLength & y );

	// *this := ( *pBase ^ *pExponent ) % *pMod.
	virtual void SetToModularExponentiation( const ExCryptoBigNumInterface * pBase, const ExCryptoBigNumInterface * pExponent, const ExCryptoBigNumInterface * pMod );

	virtual bool MultiplicativeInverse( const ExCryptoBigNumInterface * pGenericN, ExCryptoBigNumInterface * pGenericX ) const;

	//ReadFromFile(  );
	//WriteToFile(  );
};


class ExCryptoBigNumFixedLengthRefObj : public ExCryptoBigNumRefObjInterface
{
private:
	ExCryptoBigNumFixedLengthFactory * m_pFactory;
	ExCryptoBigNumFixedLength * m_pBigNum;

public:
	ExCryptoBigNumFixedLengthRefObj( ExCryptoBigNumFixedLengthFactory * pFactory, ExCryptoBigNumFixedLength * pBigNum );
	virtual ~ExCryptoBigNumFixedLengthRefObj( void );

	virtual ExCryptoBigNumInterface * GetBigNum( void ) const;
	ExCryptoBigNumFixedLength & GetBigNumNative( void ) const;
	//virtual ExAPAutoPtr<ExCryptoBigNumRefObjInterface> CreateCopy( void ) const;
};


class ExCryptoBigNumFixedLengthFactory //: public ExCryptoBigNumFactoryInterface
{
private:
	std::vector<ExCryptoBigNumFixedLength *> m_cache;

public:
	ExCryptoBigNumFixedLengthFactory( void );
	virtual ~ExCryptoBigNumFixedLengthFactory( void );

	virtual ExAPAutoPtr<ExCryptoBigNumFixedLengthRefObj> GetBigNum( int numBits );
	virtual void CacheBigNum( ExCryptoBigNumFixedLength * pBigNum );
};


// End of File
