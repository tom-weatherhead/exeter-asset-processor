/*
 *  ExAPBitmap1BitBlackWhite.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Wed May 18 2005.
 *  Copyright (c) 2005 by Tom Weatherhead. All rights reserved.
 *
 */

#include "ExAPBitmap1BitBlackWhite.h"


#ifdef _MSC_VER			// TAW 2002/03/11 : ie. if MSVC
#ifndef MACINTOSH		// TAW 2002/02/23 : ie. if MSVC

// Suppress warning C4786: Identifier truncated to 255 chars in the debug info.
#pragma warning(disable : 4786)

#endif
#endif


// **** Class ExAPBitmap1BitBlackWhite ****


// Constructors.

ExAPBitmap1BitBlackWhite::ExAPBitmap1BitBlackWhite( int width, int height, int fileRowAlignmentInBytes ) throw( ExAPException )
: ExAPBitmapNon8BitPaletted( ExAPBitmapPalette::PalBlackWhite, 1, width, height, fileRowAlignmentInBytes )
{
}


// Destructor.

ExAPBitmap1BitBlackWhite::~ExAPBitmap1BitBlackWhite( void )
{
}


void ExAPBitmap1BitBlackWhite::Invert( void )
{
	unsigned char * p = getBuffer();
	int numBytesToGo = getBytesPerLine() * GetHeight();

	while( numBytesToGo-- > 0 )
	{
		*p++ = ~*p;
	}
}


//int ExAPBitmap1BitBlackWhite::PopulationCount( void ) const


ExAPAutoPtr<ExAPBitmap1BitBlackWhite> ExAPBitmap1BitBlackWhite::Create( int width, int height, int fileRowAlignmentInBytes ) throw( ExAPException )
{
	ExAPAutoPtr<ExAPBitmap1BitBlackWhite> image;	// Initialized to zero.

	try
	{
		ExAPBitmap1BitBlackWhite * p = new ExAPBitmap1BitBlackWhite( width, height, fileRowAlignmentInBytes );

		image = p;
	}
	catch( ExAPException & )
	{
		// We caught a known exception; re-throw it.
		throw;
	}
	catch( ... )
	{
		// We caught an unknown exception.
		ThrowExAPException( ExAPException::UnknownException );
	}

	if( image == 0 )
	{
		ThrowExAPException( ExAPException::ResourceAcquisitionFailed );
	}

	return( image );
}


void ExAPBitmap1BitBlackWhite::ProcessImage(
	ExUtilByteSourceInterface * pByteSource,
	ExUtilByteSinkInterface * pByteSink,
  ExAPBitmap1BitBWProcessorInterface & bitProcessor,
	const ExAPRect & rect )
{
	ExAPAutoPtr<ExUtilByteSourceInterface> pDefaultByteSource;
	ExAPAutoPtr<ExUtilBitSourceInterface> pBitSource;
	ExAPAutoPtr<ExUtilBitSinkInterface> pBitSink;
	unsigned char * pByte = 0;

	if( pByteSource == 0 )
	{
		pByte = getBuffer() + rect.getTop() * getBytesPerLine() + rect.getLeft() * GetBitsPerPixel() / 8;
		pDefaultByteSource = ExUtilByteSourceInterface::CreateByteSourceMemory( pByte, getBytesPerLine() );
		pByteSource = pDefaultByteSource;
	}

	pBitSource = ExUtilBitSourceInterface::CreateBitSource( *pByteSource );

	if( pByteSink != 0 )
	{
		pBitSink = ExUtilBitSinkInterface::CreateBitSink( *pByteSink );
	}

#if 1
	const int numExtraBitsAtStart = rect.getLeft() * GetBitsPerPixel() % 8;
	int numExtraBitsAtEnd = 8 - rect.getRight() * GetBitsPerPixel() % 8;

	if( numExtraBitsAtEnd == 8 )
	{
		numExtraBitsAtEnd = 0;
	}
#else
	const int initialBitOffset = rect.left % 8;
	const int finalBitOffset = rect.right % 8;
#endif

	for( int y = rect.getTop(); y < rect.getBottom(); ++y )
	{

#if 1
		if( pBitSink != 0  &&  numExtraBitsAtStart > 0 )
		{
			int extraBitsAtStart = 0;
			const numBitsRead = pBitSource->ReadBits( numExtraBitsAtStart, extraBitsAtStart );

			pBitSink->WriteBits( extraBitsAtStart, numExtraBitsAtStart );
		}
#else
		if( pBitSink != 0  &&  initialBitOffset > 0 )
		{
			unsigned char mask = 128;

			for( int i = 0; i < initialBitOffset; ++i )
			{
				pBitSink->WriteBits( ( ( initialByteInLine & mask ) != 0 ) ? 1 : 0, 1 );
			}
		}
#endif

		for( int x = rect.getLeft(); x < rect.getRight(); ++x )
		{
			int bitAsInt = 0;
			const int numBitsRead = pBitSource->ReadBits( 1, bitAsInt );
			bool bit = bitAsInt != 0;

			if( pBitSink != 0 )
			{
				bit = bitProcessor.ModifyBit( bit );
				pBitSink->WriteBits( bit ? 1 : 0, 1 );
			}
			else
			{
				bitProcessor.ReadBit( bit );
			}
		}

#if 1
		if( pBitSink != 0  &&  numExtraBitsAtEnd > 0 )
		{
			int extraBitsAtEnd = 0;
			const numBitsRead = pBitSource->ReadBits( numExtraBitsAtEnd, extraBitsAtEnd );

			pBitSink->WriteBits( extraBitsAtEnd, numExtraBitsAtEnd );
		}
#else
		if( pBitSink != 0  &&  finalBitOffset < 7 )
		{
			unsigned char mask = 128 >> ( finalBitOffset + 1 );

			for( int i = finalBitOffset + 1; i < 8; ++i )
			{
				pBitSink->WriteBits( ( ( finalByteInLine & mask ) != 0 ) ? 1 : 0, 1 );
			}
		}
#endif

		pByteSource->AdvanceToNextLine();

		if( pByteSink != 0 )
		{
			pByteSink->AdvanceToNextLine();
		}
	}
}


// End of File
