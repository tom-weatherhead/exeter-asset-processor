/*
 *  ExAPTest.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Wed Jul 09 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

//lint -e1050 : Expected a template argument list '<...>' for template 'std::iterator<<1>,<2>,<3>>'

#define TAW_TRACE		1

#ifdef TAW_TRACE
#include <iostream>
#endif

#include "AssetProcessor\\ExAPExport.h"
#include "ExAPBitmapDocument.h"
#include "ExAPBitmapCMYK32.h"
#include "ExAPMedianCut.h"
#include "ExAPConversionTemplates.h"
//lint +e1050


void ImportAndDisplayDocument( const std::string & filename )
{
	// TO_DO: Report errors properly.

	try
	{
		const ExAPBitmapDocument doc( filename );

		doc.TestDisplay();
	}
	catch( ExAPException & e )
	{
		std::cout << "ImportAndDisplayDocument: ExAPException at file " << e.getFile() << ", line " << e.getLine() << "." << std::endl;
	}
	catch( ... )
	{
		std::cout << "ImportAndDisplayDocument: Unknown exception." << std::endl;
	}
} // ImportAndDisplayDocument()


bool CreateCompositedThumbnailFile(
	const std::string & srcFilename,
	const std::string & dstFilename,
	int dstWidth,
	int dstHeight,
	bool maintainAspectRatio,
	int dstJPEGQuality,
	std::string & errorMsg )
{
	bool success = false;

	try
	{
		// Step 0: Read the source document from its file.
		const ExAPBitmapDocument srcDoc( srcFilename );

		// Step 1: Create a composited (single-layer) version of the source document.
		ExAPAutoPtr<ExAPBitmap> srcImage = srcDoc.GetComposite();
		const int srcWidth = srcImage->GetWidth();
		const int srcHeight = srcImage->GetHeight();

		// Step 2: Create a 24-bit RGB version of the composite.
		ExAPAutoPtr<ExAPBitmapRGB24> midImage = new ExAPBitmapRGB24( srcWidth, srcHeight );
		const ExAPPoint origin( 0, 0 );
		const ExAPDimensions dims( srcWidth, srcHeight );

		midImage->ConvertColourModel( origin, *srcImage, origin, dims );

		// Step 3: Create a thumbnail-sized version of the RGB composite.

		if( !maintainAspectRatio )
		{
			// Do nothing.
		}
		else if( srcWidth * dstHeight < dstWidth * srcHeight )
		{
			// Reduce dstWidth.
			dstWidth = srcWidth * dstHeight / srcHeight;

			if( dstWidth < 1 )
			{
				dstWidth = 1;
			}
		}
		else
		{
			// Reduce dstHeight.
			dstHeight = dstWidth * srcHeight / srcWidth;

			if( dstHeight < 1 )
			{
				dstHeight = 1;
			}
		}

		ExAPAutoPtr<ExAPBitmapRGB24> dstImage = new ExAPBitmapRGB24( dstWidth, dstHeight );

		dstImage->BilinearResample( dstImage->getBoundingRect(), *midImage, midImage->getBoundingRect() );

		// Step 4: Write the thumbnail out as a JPEG file.
		dstImage->WriteJPEG( dstFilename.c_str(), dstJPEGQuality );
#if 1
		dstImage->TestDisplay();
#endif
		success = true;
	}
	catch( ExAPException & e )
	{
		errorMsg = e.getMsg();
	}
	catch( ... )
	{
		errorMsg = "An unknown exception was thrown.";
	}

	return( success );
} // CreateCompositedThumbnailFile()


extern "C"
{
bool CreateCompositedThumbnailFile2(
	char * srcFilenameCStr,
	char * dstFilenameCStr,
	int dstWidth,
	int dstHeight,
	bool maintainAspectRatio,
	int dstJPEGQuality )
{
	const std::string srcFilename( srcFilenameCStr );
	const std::string dstFilename( dstFilenameCStr );
	std::string errorMsg;

	return( CreateCompositedThumbnailFile( srcFilename, dstFilename, dstWidth, dstHeight, maintainAspectRatio, dstJPEGQuality, errorMsg ) );
} // CreateCompositedThumbnailFile2()
}


bool ConvertToPalettedTest(
	const std::string & srcFilename,
	std::string & errorMsg )
{
	bool success = false;

	try
	{
		// Step 0: Read the source document from its file.
		const ExAPBitmapDocument srcDoc( srcFilename );

		// Step 1: Create a composited (single-layer) version of the source document.
		ExAPAutoPtr<ExAPBitmap> srcImage = srcDoc.GetComposite();

		// Step 2: Create a 24-bit RGB version of the source image.
		ExAPAutoPtr<ExAPBitmapRGB24> srcImageRGB24 = ConvertImage<ExAPBitmapRGB24>( srcImage );

		// Step 3: Convert the image to an optimized paletted image.
		ExAPAutoPtr<ExAPBitmap8BitPaletted> dstPalettedImage = CreatePalettedImageViaMedianCut(
			*srcImageRGB24, srcImageRGB24->getBoundingRect(), 256 );

		// Step 4: Display.
		dstPalettedImage->TestDisplay();

		success = true;
	}
	catch( ExAPException & e )
	{
		errorMsg = e.getMsg();
	}
	catch( ... )
	{
		errorMsg = "An unknown exception was thrown.";
	}

	return( success );
} // ConvertToPalettedTest()


bool ExportToGIFTest(
	const std::string & srcFilename,
	const std::string & dstFilename,
	std::string & errorMsg )
{
	bool success = false;

	try
	{
		// Step 0: Read the source document from its file.
		const ExAPBitmapDocument srcDoc( srcFilename );

		// Step 1: Create a composited (single-layer) version of the source document.
		ExAPAutoPtr<ExAPBitmap> srcImage = srcDoc.GetComposite();

#if 1
		ExAPAutoPtr<ExAPBitmapCMYK32> dstImage = ConvertImage<ExAPBitmapCMYK32>( srcImage );

		dstImage->WriteJPEG( dstFilename.c_str(), 75 );
#else
		// Step 2: Export to GIF87a, converting to optimized-paletted, if necessary.
		srcImage->ExportToGIF( dstFilename );

		// Step 3: Read the dest document from its file.
		const ExAPBitmapDocument dstDoc( dstFilename );

		// Step 4: Create a composited (single-layer) version of the dest document.
		ExAPAutoPtr<ExAPBitmap> dstImage = dstDoc.GetComposite();
#endif

		// Step 5: Display.
		dstImage->TestDisplay();

		success = true;
	}
	catch( ExAPException & e )
	{
		errorMsg = e.getMsg();
	}
	catch( ... )
	{
		errorMsg = "An unknown exception was thrown.";
	}

	return( success );
} // ExportToGIFTest()


bool ExportToTIFFTest(
	const std::string & srcFilename,
	const std::string & dstFilename,
	std::string & errorMsg )
{
	bool success = false;

	try
	{
		bool useLZWCompression = true;
		bool littleEndian = true;

		// Step 0: Read the source document from its file.
		const ExAPBitmapDocument srcDoc( srcFilename );

		// Step 2: Export to GIF87a, converting to optimized-paletted, if necessary.
		srcDoc.ExportToTIFF( dstFilename, useLZWCompression, littleEndian );

#if 0
		// Step 3: Read the dest document from its file.
		const ExAPBitmapDocument dstDoc( dstFilename );

		// Step 4: Create a composited (single-layer) version of the dest document.
		ExAPAutoPtr<ExAPBitmap> dstImage = dstDoc.GetComposite();

		// Step 5: Display.
		dstImage->TestDisplay();
#endif

		//errorMsg = "Success!";
		success = true;
	}
	catch( ExAPException & e )
	{
		errorMsg = e.getMsg();
	}
	catch( ... )
	{
		errorMsg = "An unknown exception was thrown.";
	}

	return( success );
} // ExportToTIFFTest()


void ExAPTestCreateThumbnail( void )
{

	try
	{
		const std::string srcFilename = "ThumbnailTestSrc.jpg";
		const std::string dstFilename = "ThumbnailTestDst.jpg";
		const int dstWidth = 100;
		const int dstHeight = 100;
		const bool maintainAspectRatio = false;
		const int dstJPEGQuality = 75;
		std::string errorMsg;

		CreateCompositedThumbnailFile( srcFilename, dstFilename, dstWidth, dstHeight,
			maintainAspectRatio, dstJPEGQuality, errorMsg );
	}
	catch( ... )
	{
		// Do nothing.
	}
} // ExAPTestCreateThumbnail()

#if 0
void AutoEqualizeTest( const std::string & srcFilename )
{

	try
 	{
		const ExAPBitmapDocument srcDoc( srcFilename );
		ExAPAutoPtr<ExAPBitmap> srcImage = srcDoc.GetComposite();
		ExAPBitmap * srcImageRaw = srcImage;
		ExAPBitmapRGB24 * srcImageRawRGB24 = dynamic_cast<ExAPBitmapRGB24 *>( srcImageRaw );

		if( srcImageRawRGB24 != 0 )
		{
			ExAPAutoPtr<ExAPBitmap> equalizedImage = srcImageRawRGB24->AutoEqualize();

			srcImageRawRGB24->TestDisplay();
			equalizedImage->TestDisplay();
		}
	}
	catch( ... )
	{
		// Do nothing.
	}
} // AutoEqualizeTest()
#endif


void Non8BitPaletteTest( const std::string & srcFilename )
{

	try
 	{
		const ExAPBitmapDocument srcDoc( srcFilename );
		ExAPAutoPtr<ExAPBitmap> srcImage = srcDoc.GetComposite();
		const int w = srcImage->GetWidth();
		const int h = srcImage->GetHeight();
		//ExAPAutoPtr<ExAPBitmapPaletted> dstImage = ExAPBitmapPaletted::Create( ExAPBitmapPalette::PalVGA, w, h );
		ExAPAutoPtr<ExAPBitmapPaletted> dstImage = ExAPBitmapPaletted::Create( ExAPBitmapPalette::Pal2BitGrey, w, h );
		//ExAPAutoPtr<ExAPBitmapPaletted> dstImage = ExAPBitmapPaletted::Create( ExAPBitmapPalette::PalBlackWhite, w, h );
		ExAPBitmapPaletted * pRawDstImage = dstImage;
		const ExAPPoint dstPoint( 0, 0 );
		const ExAPPoint srcPoint( 0, 0 );
		const ExAPDimensions dims( w, h );

		dstImage->getPalette().SetDitherer( ExAPBitmapDither::DitherErrorDiffusion );
		dstImage->ConvertColourModel( dstPoint, *srcImage, srcPoint, dims );
		dstImage->TestDisplay();
	}
	catch( ExAPException & e )
	{
		std::cout << "Non8BitPaletteTest: ExAPException at file " << e.getFile() << ", line " << e.getLine() << "." << std::endl;
	}
	catch( ... )
	{
		std::cout << "Non8BitPaletteTest: Unknown exception." << std::endl;
	}
} // Non8BitPaletteTest()


// **** End of File ****
