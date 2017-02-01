/*
 *  ExAPConversionTemplates.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Fri Jul 18 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */


#ifndef _EXAPCONVERSIONTEMPLATES_H_
#define _EXAPCONVERSIONTEMPLATES_H_		1

#include "General\\ExAPAutoPtr.h"
#include "General\\ExAPException.h"
#include "ExAPRect.h"


//#define EXAP_USE_CREATEIMAGE_FUNCTION_TEMPLATE		1


#ifdef EXAP_USE_CREATEIMAGE_FUNCTION_TEMPLATE
// Obviously, this template can't create paletted images.

template<class C,class D> ExAPAutoPtr<D> CreateImage( int width, int height )
{
	ExAPAutoPtr<D> image;	// Initialized to zero.

	try
	{
		image = new C( width, height );
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
} // CreateImage()
#endif


template<class C> ExAPAutoPtr<C> ConvertImage(
	const ExAPAutoPtr<ExAPBitmap> & srcImage,
	const ExAPRect * srcRectPtr = 0,
	ExAPRect * dstRectPtr = 0 )
{
	ExAPAutoPtr<C> dstImage;	// Initialized to zero.
	ExAPBitmap * srcImageRawPtr = srcImage;
	C * srcImageCastRawPtr = dynamic_cast<C *>( srcImageRawPtr );
	ExAPRect srcRect = srcImage->getBoundingRect();

	if( srcRectPtr != 0 )
	{
		srcRect.intersect( *srcRectPtr );
	}

	if( dstRectPtr != 0 )
	{
		*dstRectPtr = srcRect;
	}

	if( srcRect.isEmpty() )
	{
		ThrowExAPException( ExAPException::IllegalParameterValue );
	}
	else if( srcImageCastRawPtr != 0  &&  srcRect == srcImage->getBoundingRect() )
	{
		// The source image is actually already a bitmap of the correct colour model.
		// We'll return an AutoPtr to it...
		// BUT BEWARE: The returned AutoPtr must be destroyed before the source image AutoPtr!
		dstImage = srcImageCastRawPtr;
		dstImage.SetResourceOwnership( false );	// This AutoPtr won't try to free the real pointer that it has borrowed.
	}
	else
	{
		// Create an image of the correct type.
#ifdef EXAP_USE_CREATEIMAGE_FUNCTION_TEMPLATE
		dstImage = CreateImage<C,C>( srcRect.getWidth(), srcRect.getHeight() );
#else
		try
		{
			dstImage = new C( srcRect.getWidth(), srcRect.getHeight() );
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

		if( dstImage == 0 )
		{
			ThrowExAPException( ExAPException::ResourceAcquisitionFailed );
		}
#endif

		const ExAPPoint dstPoint( 0, 0 );

		dstImage->ConvertColourModel( dstPoint,
			*srcImage, srcRect.getPoint(), srcRect.getDimensions() );
	}

	return( dstImage );
} // ConvertImage()


#endif // #ifndef _EXAPCONVERSIONTEMPLATES_H_


// **** End of File ****
