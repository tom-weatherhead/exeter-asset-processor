/*
 *  ExAPExport.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Wed Jul 09 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#ifndef _EXAPTEST_H_
#define _EXAPTEST_H_		1

#include <string>

// Uncomment the following line if you prefer "string" to "std::string".
//using std::string;


// **** File Import and Export Capabilities ****

// The Asset Processor can currently read the following file types:
// BMP (Windows)
//		- 24-bit RGB
//		- Compression: None
// GIF (87a and 89a)
//		- 8-bit paletted only
//		- interlacing is supported
//		- animations are not supported
// JPEG
//		- 8-bit Greyscale, 24-bit RGB, and 32-bit CMYK
// PNG
//		- 24-bit RGB
// TIFF version 6
//		- 8-bit Greyscale, 24-bit RGB, and 32-bit CMYK
//		- Compression: None or LZW

// The Asset Processor can currently write the following file types:
// JPEG
//		- 8-bit Greyscale, 24-bit RGB, and 32-bit CMYK


// **** Function Prototypes ****

// CreateCompositedThumbnailFile does the following:
// 1) Creates a document from the specified source file;
// 2) Composites the document down to a single layer;
// 3) Resamples the layer to the specified dimensions, possibly preserving the image's aspect ratio;
// 4) Writes the resampled image out as a 24-bit RGB JPEG file.

bool CreateCompositedThumbnailFile(
	// srcFilename: The full path of the source file, including file name and extension.
	const std::string & srcFilename,
	// dstFilename: The full path of the file to create, including file name and extension.
	const std::string & dstFilename,
	// dstWidth: The maximum width of the new image.
	int dstWidth,
	// dstHeight: The maximum height of the new image.
	int dstHeight,
	// maintainAspectRatio: If true, the new image will have the same aspect ratio
	// as the source document (with a maximum error of one pixel); thus, one of
	// the new image's dimensions (but not both) may be less than the corresponding
	// maximum specified in dstWidth and dstHeight.  If false, the new image's
	// dimensions will be exactly dstWidth and dstHeight.
	bool maintainAspectRatio,
	// dstJPEGQuality: The desired quality of the new image, if it is to be
	// a JPEG file.  The valid range is currently unknown; use 75.
	int dstJPEGQuality,
	// errorMsg: A string, into which an error message will be written if an error
	// occurs.
	std::string & errorMsg );


extern "C"
{
bool CreateCompositedThumbnailFile2(
	char * srcFilenameCStr,
	char * dstFilenameCStr,
	int dstWidth,
	int dstHeight,
	bool maintainAspectRatio,
	int dstJPEGQuality );
}


#if 1		// For testing only.

void ImportAndDisplayDocument( const std::string & filename );


bool ConvertToPalettedTest(
	const std::string & srcFilename,
	std::string & errorMsg );


bool ExportToGIFTest(
	const std::string & srcFilename,
	const std::string & dstFilename,
	std::string & errorMsg );


bool ExportToTIFFTest(
	const std::string & srcFilename,
	const std::string & dstFilename,
	std::string & errorMsg );


// A no-argument test function.
// This function reads a file called ThumbnailTestSrc.jpg
// and generates a 100*100 thumbnail file called ThumbnailTestDst.jpg .

void ExAPTestCreateThumbnail( void );

void AutoEqualizeTest( const std::string & srcFilename );

#endif	// Testing.

#endif // #ifndef _EXAPTEST_H_


// **** End of File ****
