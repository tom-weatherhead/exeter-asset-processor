// ExCryptoBigNumFixedLength.cpp - Formerly part of Helix
// Copyright (c) 2002-2005 by Tom Weatherhead.  All rights reserved.
// Started December 21, 2002

// This file contains an implementation of a limited-size big number class.
// This allows the buffer to be allocated as an array when the object is created,
// so that the buffer is contiguous and doesn't need to grow.

// The CLBigNumFactory class allows CLBigNum objects to be reused,
// instead of unnecessarily created and destroyed.

// The CLBigNum class currently requires a little-endian architecture.

#include <ctime>
#include "General\\ExAPMinMax.h"
#include "ExCryptoBigNumFixedLength.h"


// **** Class ExCryptoBigNumFixedLength ****


ExCryptoBigNumFixedLength::ExCryptoBigNumFixedLength( ExCryptoBigNumFixedLengthFactory * pFactory, int bitCapacity )
	: m_pFactory( pFactory ),
		m_pBuffer( 0 ),
		//m_byteCapacity( ( bitCapacity + 7 ) / 8 ),	// Ideally should be a multiple of 4, or even 8.
		m_byteCapacity( ( bitCapacity + 31 ) / 32 * 4 ),	// Ideally should be a multiple of 4, or even 8.
		m_bitCapacity( m_byteCapacity * 8 ),
		m_bitSize( 0 )
{

	try
	{
		m_pBuffer = new unsigned char[m_byteCapacity];
	}
	catch( ... )
	{
	}

	if( m_pBuffer == 0 )
	{
		ThrowExAPException( ExAPException::ResourceAcquisitionFailed );
	}

	memset( m_pBuffer, 0, m_byteCapacity );
}


ExCryptoBigNumFixedLength::~ExCryptoBigNumFixedLength( void )
{

	if( m_pBuffer != 0 )
	{
		delete [] m_pBuffer;
		m_pBuffer = 0;
	}
}


ExCryptoBigNumFixedLength & ExCryptoBigNumFixedLength::operator=( const ExCryptoBigNumFixedLength & src )
{

	if( &src != this )
	{
		ExAssert( src.m_bitSize <= m_bitCapacity );	// Reallocate if this condition is false?
		memset( m_pBuffer, 0, m_byteCapacity );
		memcpy( m_pBuffer, src.m_pBuffer, ( src.m_bitSize + 7 ) / 8 );
		m_bitSize = src.m_bitSize;
	}

	return( *this );
}


ExCryptoBigNumFixedLength & ExCryptoBigNumFixedLength::operator=( unsigned int unSrc )
{
	ExAssert( m_bitCapacity >= 8 * sizeof( unsigned int ) );
	memset( m_pBuffer, 0, m_byteCapacity );
	*(unsigned int *)m_pBuffer = unSrc;			// Inherently little-endian.
	m_bitSize = 8 * sizeof( unsigned int );
	TrimLeadingZeros();
	return( *this );
}


bool ExCryptoBigNumFixedLength::operator==( const ExCryptoBigNumFixedLength & src ) const
{

	if( GetNumBits() != src.GetNumBits() )
	{
		return( false );
	}

	int numBits = GetNumBits();
	const unsigned char * p1 = m_pBuffer;
	const unsigned char * p2 = src.m_pBuffer;

	while( numBits > 0 )
	{

		if( numBits >= 32 )
		{

			if( *(const unsigned int *)p1 != *(const unsigned int *)p2 )
			{
				return( false );
			}

			p1 += 4;
			p2 += 4;
			numBits -= 32;
		}
		else
		{
			const unsigned int unMask = ( 1 << numBits ) - 1;

			if( ( ( *(const unsigned int *)p1 - *(const unsigned int *)p2 ) & unMask ) != 0 )
			{
				return( false );
			}

			numBits = 0;
		}
	}

	return( true );
}


bool ExCryptoBigNumFixedLength::operator!=( const ExCryptoBigNumFixedLength & src ) const
{
	return( !( *this == src ) );
}


bool ExCryptoBigNumFixedLength::operator==( unsigned int n ) const
{

	if( m_bitSize > 32 )
	{
		return( false );
	}

	const unsigned int bigNumAsUInt = *(unsigned int *)m_pBuffer;

	if( m_bitSize == 32 )
	{
		return( bigNumAsUInt == n );
	}

	const unsigned int mask = ( 1 << m_bitSize ) - 1;

	return( ( bigNumAsUInt & mask ) == n );
}


bool ExCryptoBigNumFixedLength::operator!=( unsigned int n ) const
{
	return( !( *this == n ) );
}


ExCryptoBigNumFixedLengthFactory * ExCryptoBigNumFixedLength::GetFactory() const
{
	return( m_pFactory );
}


int ExCryptoBigNumFixedLength::GetNumBits( void ) const	// GetSize, not GetCapacity.
{
	return( m_bitSize );
}


int ExCryptoBigNumFixedLength::GetBitCapacity( void ) const
{
	return( m_bitCapacity );
}


void ExCryptoBigNumFixedLength::TrimLeadingZeros( void )
{

	while( m_bitSize > 0  &&  !BitTest( m_bitSize - 1 ) )
	{
		--m_bitSize;
	}
}


bool ExCryptoBigNumFixedLength::BitTest( int bitNum ) const
{
	// ExAssert( bitNum >= 0 );
	// ExAssert( bitNum < m_bitCapacity );
	const unsigned char c = m_pBuffer[bitNum / 8];
	const unsigned char mask = 1 << ( bitNum % 8 );

	return( ( c & mask ) != 0 );
}


void ExCryptoBigNumFixedLength::BitSet( int bitNum, bool newBitState )
{
	// ExAssert( bitNum >= 0 );
	// ExAssert( bitNum < m_bitCapacity );
	unsigned char & c = m_pBuffer[bitNum / 8];
	const unsigned char mask = 1 << ( bitNum % 8 );

	if( newBitState )
	{
		// Set bit.
		c |= mask;
	}
	else
	{
		// Clear bit.
		c &= ~mask;
	}
}


void ExCryptoBigNumFixedLength::ShiftLeftByOne( void )
{
	unsigned char * p = m_pBuffer;
	const int numDWords = ( m_bitSize + 31 ) / 32;

	// Return if byteSize is zero?

	if( m_bitSize == 0 )
	{
		return;
	}

	ExAssert( m_bitSize < m_bitCapacity )

	__asm
	{
		; eax - contains the data to be shifted
		; ebx - (now unused) caches the carry bit
		; ecx - the loop counter
		; edi - the address of the data to be shifted

		clc
		; xor ebx, ebx
		mov ecx, numDWords
		mov edi, p

Label000:
		mov eax, [edi]				; mov doesn't affect any flags.
		; clc										; Clear the carry bit, just to be safe.
		; rcr ebx, 1						; Load the carry bit with the right value.
		rcl eax, 1						; Rotate 33 bits to the left one position.
		; rcl ebx, 1						; Cache the carry bit
		mov [edi], eax
		lea edi, [edi + 4]		; lea doesn't affect any flags.
		; add edi, 4

		loop Label000					; loop doesn't affect any flags.
		; dec ecx
		; jnz Label000

		; test ebx, ebx					; ebx equals either zero or one.
		jnc Label001
		mov [edi], 1					; Use a PPro cmov instruction here?  No: a cmov can only write to registers.
Label001:

		; Done.
	}

	++m_bitSize;
}


void ExCryptoBigNumFixedLength::ShiftRightByOne( void )
{

	if( m_bitSize == 0 )
	{
		return;
	}

	const int numDWords = ( m_bitSize + 31 ) / 32;
	unsigned char * p = m_pBuffer + 4 * ( numDWords - 1 );

	__asm
	{
		; eax - contains the data to be shifted
		; ecx - the loop counter
		; edi - the address of the data to be shifted

		clc
		mov ecx, numDWords
		mov edi, p

Label000:
		mov eax, [edi]				; mov doesn't affect any flags.
		rcr eax, 1						; Rotate 33 bits to the right one position.
		mov [edi], eax
		lea edi, [edi - 4]		; lea doesn't affect any flags.

		loop Label000					; loop doesn't affect any flags.
	}

	--m_bitSize;
}


bool ExCryptoBigNumFixedLength::IsGreaterThan( const ExCryptoBigNumFixedLength & bigNum ) const
{

	if( m_bitSize > bigNum.m_bitSize )
	{
		return( true );
	}
	else if( m_bitSize < bigNum.m_bitSize )
	{
		return( false );
	}

	const int numDWords = ( m_bitSize + 31 ) / 32;
	const unsigned char * pSrc1 = m_pBuffer + 4 * (numDWords - 1);
	const unsigned char * pSrc2 = bigNum.m_pBuffer + 4 * (numDWords - 1);
	int retValue = 0;

	__asm
	{
		; eax - contains data from this BigNum
		; ebx - contains data from the other BigNum
		; ecx - the loop counter
		; edx - the function's return value
		; esi - pointer to the other BigNum's data
		; edi - pointer to this BigNum's data
		mov ecx, numDWords
		xor edx, edx
		mov esi, pSrc2
		mov edi, pSrc1

Label000:
		mov eax, [edi]				; mov doesn't affect any flags.
		mov ebx, [esi]
		lea esi, [esi - 4]
		lea edi, [edi - 4]
		cmp eax, ebx
		loope Label000				; Jump if ecx != 0 and zf == 1.  loop doesn't affect any flags.

		jle Label001
		inc edx
Label001:
		mov retValue, edx
	}

	return( retValue != 0 );
}


void ExCryptoBigNumFixedLength::Add( const ExCryptoBigNumFixedLength & bigNum )
{
	unsigned char * pDst = m_pBuffer;
	unsigned char * pSrc = bigNum.m_pBuffer;
	const int maxBitSize = max( m_bitSize, bigNum.m_bitSize );
	const int numDWords = ( maxBitSize + 31 ) / 32;

	__asm
	{
		; eax - contains the data to be read from and written to
		; ebx - contains the data to be read from
		; ecx - the loop counter
		; esi - the address of the data to be read from
		; edi - the address of the data to be read from and written to

		clc
		mov ecx, numDWords
		mov esi, pSrc
		mov edi, pDst

Label000:
		mov eax, [edi]				; mov doesn't affect any flags.
		mov ebx, [esi]
		lea esi, [esi + 4]
		adc eax, ebx
		mov [edi], eax
		lea edi, [edi + 4]
		loop Label000					; loop doesn't affect any flags.

		jnc Label001
		mov [edi], 1					; Use a PPro cmov instruction here?  No: a cmov can only write to registers.
Label001:

		; Done.
	}

	m_bitSize = maxBitSize;

	if( m_bitSize < m_bitCapacity  &&  BitTest( m_bitSize ) )
	{
		++m_bitSize;
	}
}


void ExCryptoBigNumFixedLength::Subtract( const ExCryptoBigNumFixedLength & bigNum )
{
	ExAssert( !( bigNum.IsGreaterThan( *this ) ) )

	const int numDWords = ( m_bitSize + 31 ) / 32;
	unsigned char * pDst = m_pBuffer; // + 4 * ( numDWords - 1 );
	unsigned char * pSrc = bigNum.m_pBuffer; // + 4 * ( numDWords - 1 );

	__asm
	{
		; eax - contains the data to be read from and written to
		; ebx - contains the data to be read from
		; ecx - the loop counter
		; esi - the address of the data to be read from
		; edi - the address of the data to be read from and written to

		clc
		mov ecx, numDWords
		mov esi, pSrc
		mov edi, pDst

Label000:
		mov eax, [edi]				; mov doesn't affect any flags.
		mov ebx, [esi]
		lea esi, [esi + 4]
		sbb eax, ebx
		mov [edi], eax
		lea edi, [edi + 4]
		loop Label000					; loop doesn't affect any flags.
	}

	TrimLeadingZeros();
}


// *this = x1 * x2;

void ExCryptoBigNumFixedLength::Multiply( const ExCryptoBigNumFixedLength & x1, const ExCryptoBigNumFixedLength & x2 )
{
	ExAssert( GetBitCapacity() >= x1.GetNumBits() + x2.GetNumBits() );

	ExAPAutoPtr<ExCryptoBigNumFixedLengthRefObj> pShiftedX2RefObj = m_pFactory->GetBigNum( GetBitCapacity() + 1 );
	ExCryptoBigNumFixedLength & shiftedX2 = pShiftedX2RefObj->GetBigNumNative();

	shiftedX2 = x2;
	*this = 0;

	for( int i = 0; i < x1.GetNumBits(); ++i )
	{
		
		if( x1.BitTest( i ) )
		{
			Add( shiftedX2 );
		}

		shiftedX2.ShiftLeftByOne();
	}
}


void ExCryptoBigNumFixedLength::ModularMultiply( const ExCryptoBigNumFixedLength & bigNum, const ExCryptoBigNumFixedLength & mod )		// *this := ( *this * *pBigNum ) % *pMod.
{
	ExAPAutoPtr<ExCryptoBigNumFixedLengthRefObj> pSumRefObj = m_pFactory->GetBigNum( mod.m_bitCapacity + 1 );
	ExCryptoBigNumFixedLength & sum = pSumRefObj->GetBigNumNative();
	ExAPAutoPtr<ExCryptoBigNumFixedLengthRefObj> pShiftedModdedMultiplierRefObj = m_pFactory->GetBigNum( mod.m_bitCapacity + 1 );
	ExCryptoBigNumFixedLength & shiftedModdedMultiplier = pShiftedModdedMultiplierRefObj->GetBigNumNative();

	shiftedModdedMultiplier = bigNum;

	while( shiftedModdedMultiplier.IsGreaterThan( mod ) )
	{
		shiftedModdedMultiplier.Subtract( mod );
	}

	for( int i = 0; i < m_bitSize; ++i )
	{

		if( BitTest( i ) )
		{
			sum.Add( shiftedModdedMultiplier );

			if( sum.IsGreaterThan( mod ) )
			{
				sum.Subtract( mod );
			}
		}

		shiftedModdedMultiplier.ShiftLeftByOne();

		if( shiftedModdedMultiplier.IsGreaterThan( mod ) )
		{
			shiftedModdedMultiplier.Subtract( mod );
		}
	}

	*this = sum;
}


void ExCryptoBigNumFixedLength::Divide( const ExCryptoBigNumFixedLength & divisor, ExCryptoBigNumFixedLength * pQuotient, ExCryptoBigNumFixedLength * pRemainder ) const
{
	ExAPAutoPtr<ExCryptoBigNumFixedLengthRefObj> pDRefObj = m_pFactory->GetBigNum( m_bitCapacity + 1 );
	ExCryptoBigNumFixedLength & d = pDRefObj->GetBigNumNative();
	ExAPAutoPtr<ExCryptoBigNumFixedLengthRefObj> pQuotientRefObj;
	ExAPAutoPtr<ExCryptoBigNumFixedLengthRefObj> pRemainderRefObj;
	int i;

	if( pQuotient == 0 )
	{
		pQuotientRefObj = m_pFactory->GetBigNum( m_bitCapacity );
		pQuotient = &pQuotientRefObj->GetBigNumNative();
	}

	if( pRemainder == 0 )
	{
		pRemainderRefObj = m_pFactory->GetBigNum( m_bitCapacity );
		pRemainder = &pRemainderRefObj->GetBigNumNative();
	}

	*pQuotient = 0;
	*pRemainder = *this;
	d = divisor;

	for( i = 0; !d.IsGreaterThan( *this ); ++i )
	{
		d.ShiftLeftByOne();
	}

	pQuotient->m_bitSize = i;

	for( ; ; )
	{
		d.ShiftRightByOne();

		if( --i < 0 )
		{
			break;
		}

		if( !d.IsGreaterThan( *pRemainder ) )
		//if( remainder >= d )
		{
			pRemainder->Subtract( d );
			pQuotient->BitSet( i, true );
		}
	}

	pQuotient->TrimLeadingZeros();
}


// *this := ( *pBase ^ *pExponent ) % *pMod.

void ExCryptoBigNumFixedLength::SetToModularExponentiation(
	const ExCryptoBigNumInterface * pBase,
	const ExCryptoBigNumInterface * pExponent,
	const ExCryptoBigNumInterface * pMod )
{
	const ExCryptoBigNumFixedLength * pBaseX = dynamic_cast<const ExCryptoBigNumFixedLength *>( pBase );
	const ExCryptoBigNumFixedLength * pExponentX = dynamic_cast<const ExCryptoBigNumFixedLength *>( pExponent );
	const ExCryptoBigNumFixedLength * pModX = dynamic_cast<const ExCryptoBigNumFixedLength *>( pMod );

	if( pBaseX == 0  ||  pExponentX == 0  ||  pModX == 0 )
	{
		ThrowExAPException( ExAPException::DynamicCastFailed );
	}

	// TODO: Get this number from the Factory.
	//ExCryptoBigNumFixedLength product( pModX->m_bitCapacity );
	ExAPAutoPtr<ExCryptoBigNumFixedLengthRefObj> pProductRefObj = m_pFactory->GetBigNum( pModX->m_bitCapacity );
	ExCryptoBigNumFixedLength & product = pProductRefObj->GetBigNumNative();

	// Set product = 1 .
	//product.BitSet( 0, true );
	//product.m_bitSize = 1;
	product = 1;

	for( int i = pExponentX->m_bitSize - 1; i >= 0; --i )
	{
		product.ModularMultiply( product, *pModX );

		if( pExponentX->BitTest( i ) )
		{
			product.ModularMultiply( *pBaseX, *pModX );
		}
	}

	*this = product;
}


void ExCryptoBigNumFixedLength::SetToRandomValue( int numBits, bool bEnforceOdd )
{
	ExAssert( numBits <= m_bitCapacity );
	srand( time( 0 ) );

	unsigned char * p = m_pBuffer;

	while( numBits > 0 )
	{
		
		if( numBits >= 32 )
		{
			*(unsigned int *)p = rand();
			p += 4;
			numBits -= 32;
		}
		else
		{
			*(unsigned int *)p = rand() & ( ( 1 << numBits ) - 1 );
			numBits = 0;
		}
	}

	if( bEnforceOdd )
	{
		BitSet( 0, true );	// Make it an odd number, so that it might be prime.
	}

	BitSet( numBits - 1, true );		// Ensure that the number actually contains numBits significant bits.
}


// From "Introduction to Algorithms", p. 840.

bool ExCryptoBigNumFixedLength::MillerRabinWitness( const ExCryptoBigNumFixedLength & a ) const
{
	ExAPAutoPtr<ExCryptoBigNumFixedLengthRefObj> pDRefObj = m_pFactory->GetBigNum( m_bitCapacity );
	ExCryptoBigNumFixedLength & d = pDRefObj->GetBigNumNative();
	ExAPAutoPtr<ExCryptoBigNumFixedLengthRefObj> pOneRefObj = m_pFactory->GetBigNum( 1 );
	ExCryptoBigNumFixedLength & one = pOneRefObj->GetBigNumNative();
	ExAPAutoPtr<ExCryptoBigNumFixedLengthRefObj> pNMinus1RefObj = m_pFactory->GetBigNum( m_bitCapacity );
	ExCryptoBigNumFixedLength & nMinus1 = pNMinus1RefObj->GetBigNumNative();
	ExAPAutoPtr<ExCryptoBigNumFixedLengthRefObj> pXRefObj = m_pFactory->GetBigNum( m_bitCapacity );
	ExCryptoBigNumFixedLength & x = pXRefObj->GetBigNumNative();

	d = 1;
	one = 1;
	nMinus1 = *this;
	nMinus1.Subtract( one );

	for( int i = nMinus1.GetNumBits() - 1; i >= 0; --i )
	{
		x = d;

#if 1
		d.ModularMultiply( d, *this );
#else
		d = ( d * d ) % n;
#endif

		if( d == one  &&  x != one  &&  x != nMinus1 )
		{
			// x is a non-trivial square root of 1 (mod n).
			return( true );
		}

		if( nMinus1.BitTest( i ) )
		{
#if 1
			d.ModularMultiply( a, *this );
#else
			d = ( d * a ) % n;
#endif
		}
	}

	return( d != one );
}


// From "Introduction to Algorithms", p. 841.

bool ExCryptoBigNumFixedLength::MillerRabinIsComposite( int s ) const
{
	ExAPAutoPtr<ExCryptoBigNumFixedLengthRefObj> pARefObj = m_pFactory->GetBigNum( m_bitCapacity );
	ExCryptoBigNumFixedLength & a = pARefObj->GetBigNumNative();

	//printf( "Witness # " );

	for( int i = 1; i <= s; ++i )
	{
		a.SetToRandomValue( GetNumBits() - 1, false );

		//printf( "%d ", i );

		if( MillerRabinWitness( a ) )
		{
			//printf( "Composite.\n" );
			return( true );
		}
	}

	//printf( "Probably prime.\n" );
	return( false );	// n is probably prime.
}


void ExCryptoBigNumFixedLength::SetToRandomPrime( int nPrimeBitLength /*, int nWhichPrime */ )
{
	// The probability of returning a composite number is 2^-20.
	static const int knMillerRabinIterations = 20;
	//int i = 0;

	do
	{
		SetToRandomValue( nPrimeBitLength, true );

		//if( nWhichPrime > 0 )
		//{
		//	printf( "Prime #%d, ", nWhichPrime );
		//}

		//printf( "Attempt %d: Testing for primality: ", ++i );
		//PrintHex();
	}
	while( MillerRabinIsComposite( knMillerRabinIterations ) );

}


// From "Introduction to Algorithms", p. 812.
// Given a and b, finds d, x, and y such that:
// 1) d = gcd( a, b ), and
// 2) d = a * x + b * y.

// The algorithm has been modified so that d == a * x - b * y,

void ExCryptoBigNumFixedLength::ExtendedEuclid(
	const ExCryptoBigNumFixedLength & a, const ExCryptoBigNumFixedLength & b,
	ExCryptoBigNumFixedLength & d, ExCryptoBigNumFixedLength & x, ExCryptoBigNumFixedLength & y )
{
	
	if( b.GetNumBits() == 0 )		// i.e. b == 0
	{
		d = a;
		x = 1;
		y = 0;
		return;
	}

	const int bitCapacity = max( a.GetBitCapacity(), b.GetBitCapacity() );
	ExCryptoBigNumFixedLengthFactory * pFactory = a.m_pFactory;
	ExAPAutoPtr<ExCryptoBigNumFixedLengthRefObj> pADivBRefObj = pFactory->GetBigNum( bitCapacity );
	ExCryptoBigNumFixedLength & ADivB = pADivBRefObj->GetBigNumNative();
	ExAPAutoPtr<ExCryptoBigNumFixedLengthRefObj> pAModBRefObj = pFactory->GetBigNum( bitCapacity );
	ExCryptoBigNumFixedLength & AModB = pAModBRefObj->GetBigNumNative();
	ExAPAutoPtr<ExCryptoBigNumFixedLengthRefObj> pX2RefObj = pFactory->GetBigNum( bitCapacity );
	ExCryptoBigNumFixedLength & x2 = pX2RefObj->GetBigNumNative();
	ExAPAutoPtr<ExCryptoBigNumFixedLengthRefObj> pY2RefObj = pFactory->GetBigNum( bitCapacity );
	ExCryptoBigNumFixedLength & y2 = pY2RefObj->GetBigNumNative();

	a.Divide( b, &ADivB, &AModB );
	ExtendedEuclid( b, AModB, d, x2, y2 );

	// Modify the algorithm so that d == a * x - b * y,
	// with x >= 0 and y >= 0.
	//Assert( d == b * x2 - AModB * y2 );

	//const BigNum temp1 = x2 + ADivB * y2;
	//BigNum n = y2 / b + 1;
	//const BigNum n2 = temp1 / a + 1;
	ExAPAutoPtr<ExCryptoBigNumFixedLengthRefObj> pOneRefObj = pFactory->GetBigNum( 1 );
	ExCryptoBigNumFixedLength & one = pOneRefObj->GetBigNumNative();
	ExAPAutoPtr<ExCryptoBigNumFixedLengthRefObj> pTemp1RefObj = pFactory->GetBigNum( bitCapacity );
	ExCryptoBigNumFixedLength & temp1 = pTemp1RefObj->GetBigNumNative();
	ExAPAutoPtr<ExCryptoBigNumFixedLengthRefObj> pNRefObj = pFactory->GetBigNum( bitCapacity );
	ExCryptoBigNumFixedLength & n = pNRefObj->GetBigNumNative();
	ExAPAutoPtr<ExCryptoBigNumFixedLengthRefObj> pN2RefObj = pFactory->GetBigNum( bitCapacity );
	ExCryptoBigNumFixedLength & n2 = pN2RefObj->GetBigNumNative();

	temp1.Multiply( ADivB, y2 );
	temp1.Add( x2 );
	one = 1;
	y2.Divide( b, &n, 0 );
	n.Add( one );
	temp1.Divide( a, &n2, 0 );

	if( n2.IsGreaterThan( n ) )
	{
		n = n2;
	}

	n.Add( one );

	//x = n * b - y2;
	//y = n * a - temp1;
	x.Multiply( n, b );
	x.Subtract( y2 );
	y.Multiply( n, a );
	y.Subtract( temp1 );
	//Assert( d == a * x - b * y );
}


// If possible, find x such that *this * x == 1 (mod n).

bool ExCryptoBigNumFixedLength::MultiplicativeInverse(
	const ExCryptoBigNumInterface * pGenericN,
	ExCryptoBigNumInterface * pGenericX ) const
{
	const ExCryptoBigNumFixedLength * pN = dynamic_cast<const ExCryptoBigNumFixedLength *>( pGenericN );
	ExCryptoBigNumFixedLength * pX = dynamic_cast<ExCryptoBigNumFixedLength *>( pGenericX );

	if( pN == 0  ||  pX == 0 )
	{
		ThrowExAPException( ExAPException::DynamicCastFailed );
	}

	ExAPAutoPtr<ExCryptoBigNumFixedLengthRefObj> pDRefObj = m_pFactory->GetBigNum( m_bitCapacity );
	ExCryptoBigNumFixedLength & d = pDRefObj->GetBigNumNative();
	ExAPAutoPtr<ExCryptoBigNumFixedLengthRefObj> pKRefObj = m_pFactory->GetBigNum( m_bitCapacity );
	ExCryptoBigNumFixedLength & k = pKRefObj->GetBigNumNative();

	ExtendedEuclid( *this, *pN, d, *pX, k );
	
	if( d != 1 )
	{
		return( false );
	}

	ExAPAutoPtr<ExCryptoBigNumFixedLengthRefObj> pX2RefObj = m_pFactory->GetBigNum( m_bitCapacity );
	ExCryptoBigNumFixedLength & x2 = pX2RefObj->GetBigNumNative();

	//x %= n;
	x2 = *pX;
	x2.Divide( *pN, 0, pX );
	return( true );
}


// **** Class ExCryptoBigNumFixedLengthRefObj ****


ExCryptoBigNumFixedLengthRefObj::ExCryptoBigNumFixedLengthRefObj( ExCryptoBigNumFixedLengthFactory * pFactory, ExCryptoBigNumFixedLength * pBigNum )
	: m_pFactory( pFactory ),
		m_pBigNum( pBigNum )
{
}


ExCryptoBigNumFixedLengthRefObj::~ExCryptoBigNumFixedLengthRefObj( void )
{

	try
	{
		// Reinsert the pBigNum into the factory's cache.
		m_pFactory->CacheBigNum( m_pBigNum );
	}
	catch( ... )
	{
		// This is a destructor; eat all exceptions generated herein.
	}
}


ExCryptoBigNumInterface * ExCryptoBigNumFixedLengthRefObj::GetBigNum( void ) const
{
	return( m_pBigNum );
}


ExCryptoBigNumFixedLength & ExCryptoBigNumFixedLengthRefObj::GetBigNumNative( void ) const
{
	return( *m_pBigNum );
}


// **** Class ExCryptoBigNumFixedLengthFactory ****


ExCryptoBigNumFixedLengthFactory::ExCryptoBigNumFixedLengthFactory( void )
{
}


ExCryptoBigNumFixedLengthFactory::~ExCryptoBigNumFixedLengthFactory( void )
{

	while( m_cache.empty() )
	{
		ExCryptoBigNumFixedLength * pBigNum = m_cache.back();

		m_cache.pop_back();
		delete pBigNum;
	}
}


ExAPAutoPtr<ExCryptoBigNumFixedLengthRefObj> ExCryptoBigNumFixedLengthFactory::GetBigNum( int numBits )
{
	ExAPAutoPtr<ExCryptoBigNumFixedLengthRefObj> pRefObj;
	ExCryptoBigNumFixedLength * pBigNum = 0;

	// First, look for the shortest BigNum in the factory's cache whose capacity is at least as long as numBits.
	std::vector<ExCryptoBigNumFixedLength *>::iterator i;
	ExCryptoBigNumFixedLength * pBestFit = 0;
	std::vector<ExCryptoBigNumFixedLength *>::iterator bestFitIterator = m_cache.end();
	//int bestFitMargin = 0;

	for( i = m_cache.begin(); i != m_cache.end(); ++i )
	{
		
		if( (*i)->GetBitCapacity() < numBits )
		{
			continue;
		}
		else if( pBestFit == 0  ||  (*i)->GetBitCapacity() < pBestFit->GetBitCapacity() )  // Find the tightest fit.
		{
			pBestFit = *i;
			bestFitIterator = i;
			//bestFitMargin = i->GetBitCapacity - numBits;
		}
	}

	// If no sufficiently long BigNum is in the factory's cache, create one.

	if( pBestFit != 0 )
	{
		m_cache.erase( bestFitIterator );
	}
	else
	{
		//const int numBytes = ( numBits + 31 ) / 32 * 4;

		pBestFit = new ExCryptoBigNumFixedLength( this, numBits );
	}

	pRefObj = new ExCryptoBigNumFixedLengthRefObj( this, pBestFit );
	return( pRefObj );
}


void ExCryptoBigNumFixedLengthFactory::CacheBigNum( ExCryptoBigNumFixedLength * pBigNum )
{
	m_cache.push_back( pBigNum );
}


// **** End of File ****
