/*
 *  MainWin.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Mon May 26 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#include <iostream>

#include "General\\ExAPException.h"
#include "AssetProcessor\\ExAPExport.h"
#include "CodecLib\\ExCodecHuffman.h"

using std::cout;
using std::endl;


#if 0
bool fileExists( const char * filename );
bool writeRandomFile( const char * filename, size_t fileSize );
void TDiff2( const char * filename1, const char * filename2 );
#endif
void TDiff( int argc, char * argv[] );

void LZ77Test( const std::string & srcFilename );

void ExploreJPEGFile( const std::string & filename );

void ExploreICOFile( const std::string & filename );

void Non8BitPaletteTest( const std::string & srcFilename );


#if 0
static void DrawTestImage1( int argc, char * argv[] )
{
	ExAPBitmapRGB24 image( 550, 150 );
	ExAPBitmapRGB24 image2( 275, 75 );
	ExAPBitmapARGB32 layer( 150, 150 );	// Initialize all alphas to zero.
	unsigned char red[4] = { 0x00, 0x00, 0xFF, 0xFF };
	unsigned char green[4] = { 0x00, 0xFF, 0x00, 0xFF };
	unsigned char blue[4] = { 0xFF, 0x00, 0x00, 0xFF };
	const ExAPRect imageBoundingRect = image.getBoundingRect();
	
#if 1
	// Test JPEG reading.
	char * srcFilename = "Felicity.jpg";

	if( argc > 0 )
	{
		srcFilename = argv[0];
	}

	ExAPBitmapRGB24 * pDynamicImage = ExAPBitmapRGB24::ReadJPEG( srcFilename );

	if( pDynamicImage == 0 )
	{
		printf( "ReadJPEG returned zero.\n" );
	}
	else
	{
		pDynamicImage->TestDisplay();

		ExAPBitmapRGB24 image3( 100, 100 );

		image3.BilinearResample( image3.getBoundingRect(), *pDynamicImage, pDynamicImage->getBoundingRect() );
		image3.WriteJPEG( "MyFirstAsset.jpg", 75 );
		delete pDynamicImage;
	}
#elif 1
	// Test compositing.
	image.SolidRectangle( imageBoundingRect, blue );
	layer.NonAntiAliasedCircle( 75, 75, 50, red );
	image.Composite( 200, 0, layer, layer.getBoundingRect() );
	image.TestDisplay();
	image.WriteJPEG( "APTest.jpg", 75 );	// 75 is just a guess for the quality.
#else
	// Test bilinear downsampling.
	image.SolidRectangle( imageBoundingRect, blue );
	image.NonAntiAliasedCircle( 275, 75, 50, red );
	image2.SolidRectangle( imageBoundingRect, green );
	image2.BilinearResample( image2.getBoundingRect(), image, imageBoundingRect );
	image2.TestDisplay();
#endif
} // DrawTestImage1()


static void LZWTest( int argc, char * argv[] )
{
	const char * srcFilename = "LZWSrc.src";
	const char * compressedFilename = "LZWCmprs.bin";
	const char * decompressedFilename = "LZWDecmprs.bin";

#if 0
	if( argc > 0 )
	{
		srcFilename = argv[0];
	}

	if( argc > 1 )
	{
		compressedFilename = argv[1];
	}

	if( argc > 2 )
	{
		decompressedFilename = argv[2];
	}
#endif

	if( !fileExists( srcFilename )  &&  !writeRandomFile( srcFilename, 65536 ) )
	{
		return;
	}

	ExAPLZWCodecV1 lzwCodec;
	const int maxDictionarySize = 4096;
	const bool variableCodeSize = false;

	printf( "LZWTest: Compressing data.\n" );

	{
		ExAPByteSourceV1 byteSource( srcFilename );
		ExAPBitSinkV1 bitSink( compressedFilename );

		lzwCodec.CompressData( byteSource, bitSink, maxDictionarySize, variableCodeSize );
	}

	printf( "LZWTest: Decompressing data.\n" );

	{
		ExAPBitSourceV1 bitSource( compressedFilename );
		ExAPByteSinkV1 byteSink( decompressedFilename );

		lzwCodec.DecompressData( bitSource, byteSink, maxDictionarySize, variableCodeSize );
	}

	printf( "LZWTest: Diffing files.\n" );

	TDiff2( srcFilename, decompressedFilename );
} // LZWTest()
#endif


int main( int argc, char * argv[] )
{
	//const char * cmd = "lz77";
	//const char * cmd = "readicon";
	//const char * cmd = "non8test";
	const char * cmd = "display";

	if( argc >= 2 )
	{
		cmd = argv[1];
	}

	try
	{
		const std::string svnSandboxBasePath = "C:\\SVNSandbox\\";
		const std::string testAppPath = svnSandboxBasePath + "Daring\\Apps\\APTestApp\\";
		//const std::string srcImageBasePath = "..\\..\\..\\QualityAssurance\\Images\\";
		const std::string srcImageBasePath = svnSandboxBasePath + "Daring\\trunk\\QualityAssurance\\Images\\";
		//const std::string defaultSrcImageFilename = srcImageBasePath + "JPEG\\FelicityRGB24.jpg";
		//const std::string defaultSrcImageFilename = srcImageBasePath + "JPEG\\FelicityGrey8.jpg";
		//const std::string defaultSrcImageFilename = srcImageBasePath + "TIFF\\RedRGB24LZW.tif";
		//const std::string defaultSrcImageFilename = srcImageBasePath + "PNG\\F_RGB24LZ77.png";
		const std::string defaultSrcImageFilename = "C:\\Archive\\Pictures\\logo_win2000.gif";

		if( strcmp( cmd, "xjpeg" ) == 0 )
		{
			//const std::string autoEqTestSrc = testAppPath + "Debug\\AutoEqTestSrc.jpg";

			ExploreJPEGFile( defaultSrcImageFilename );
		}
#if 0
		else if( strcmp( cmd, "eq" ) == 0 )
		{
			const std::string autoEqTestSrc = testAppPath + "Debug\\AutoEqTestSrc.jpg";

			AutoEqualizeTest( autoEqTestSrc );
		}
#endif
		else if( strcmp( cmd, "thumbtest" ) == 0 )
		{
			ExAPTestCreateThumbnail();
		}
		else if( strcmp( cmd, "lz77" ) == 0 )	// Huffman encoding test
		{
			std::string srcFilenameLZ77 = testAppPath + "LZ77Test.bin";

			if( argc < 3 )
			{
				cout << "LZ77 test: No source filename given; using " << srcFilenameLZ77 << "." << endl;
			}
			else
			{
				srcFilenameLZ77 = argv[2];
			}

			LZ77Test( srcFilenameLZ77 );
		}
		else if( strcmp( cmd, "huff" ) == 0 )	// Huffman encoding test
		{
			std::string srcFilenameHuff = testAppPath + "HuffTest.txt";

			if( argc < 3 )
			{
				cout << "Huffman test: No source filename given; using " << srcFilenameHuff << "." << endl;
			}
			else
			{
				srcFilenameHuff = argv[2];
			}

			HuffmanEncodingTest( srcFilenameHuff );
		}
		else if( strcmp( cmd, "2tiff" ) == 0 )	// Export to TIFF
		{
			std::string srcFilename = defaultSrcImageFilename;

			if( argc < 3 )
			{
				cout << "Export to TIFF: No source filename given; using " << srcFilename << "." << endl;
			}
			else
			{
				srcFilename = argv[2];
			}

			std::string dstFilename = "TestTIFF.tif";

			if( argc < 4 )
			{
				cout << "Export to TIFF: No destination filename given; using " << dstFilename << "." << endl;
			}
			else
			{
				dstFilename = argv[3];
			}

			std::string errorMsg;
			const bool success = ExportToTIFFTest( srcFilename, dstFilename, errorMsg );

			if( success )
			{
				cout << "Export to TIFF: Success!" << endl;
			}
			else
			{
				cout << "Export to TIFF: Failure" << endl;
				cout << "  Error message: '" << errorMsg << "'" << endl;
			}
		}
		else if( strcmp( cmd, "2gif" ) == 0 )	// Export to GIF87a
		{
			std::string srcFilename = defaultSrcImageFilename;

			if( argc < 3 )
			{
				cout << "Export to GIF: No source filename given; using " << srcFilename << "." << endl;
			}
			else
			{
				srcFilename = argv[2];
			}

			std::string dstFilename = "TestGIF.gif";

			if( argc < 4 )
			{
				cout << "Export to GIF: No destination filename given; using " << dstFilename << "." << endl;
			}
			else
			{
				dstFilename = argv[3];
			}

			std::string errorMsg;
			const bool success = ExportToGIFTest( srcFilename, dstFilename, errorMsg );

			if( success )
			{
				cout << "Export to GIF: Success!" << endl;
			}
			else
			{
				cout << "Export to GIF: Failure" << endl;
				cout << "  Error message: '" << errorMsg << "'" << endl;
			}
		}
		else if( strcmp( cmd, "mc" ) == 0 )	// Median Cut (optimized palette generation)
		{
			std::string srcFilename = defaultSrcImageFilename;

			if( argc < 3 )
			{
				cout << "Median Cut: No filename given; using " << srcFilename << "." << endl;
			}
			else
			{
				srcFilename = argv[2];
			}

			std::string errorMsg;
			const bool success = ConvertToPalettedTest( srcFilename, errorMsg );

			if( success )
			{
				cout << "Median Cut: Success!" << endl;
			}
			else
			{
				cout << "Median Cut: Failure" << endl;
				cout << "  Error message: '" << errorMsg << "'" << endl;
			}
		}
		else if( strcmp( cmd, "thumb" ) == 0 )
		{
			std::string srcFilename = defaultSrcImageFilename;

			if( argc < 3 )
			{
				cout << "Thumbnail: No filename given; using " << srcFilename << "." << endl;
			}
			else
			{
				srcFilename = argv[2];
			}

			{
				const std::string dstFilename( "Thumbnail.jpg" );
				const int dstWidth = 100;
				const int dstHeight = 100;
				const bool maintainAspectRatio = false;
				const int dstJPEGQuality = 75;
				std::string errorMsg;
#if 1
				//char * dstFilenameCStr = "Thumbnail.jpg";
				const bool success = CreateCompositedThumbnailFile2(
					(char *)srcFilename.c_str(),
					(char *)dstFilename.c_str(),
					dstWidth,
					dstHeight,
					maintainAspectRatio,
					dstJPEGQuality );
#else
				const bool success = CreateCompositedThumbnailFile(
					srcFilename,
					dstFilename,
					dstWidth,
					dstHeight,
					maintainAspectRatio,
					dstJPEGQuality,
					errorMsg );
#endif

				if( success )
				{
					cout << "Thumbnail: Success!" << endl;
				}
				else
				{
					cout << "Thumbnail: Failure" << endl;
					cout << "  Error message: '" << errorMsg << "'" << endl;
				}
			}
		}
		else if( strcmp( cmd, "display" ) == 0 )
		{
			std::string srcFilename = defaultSrcImageFilename;

			if( argc < 3 )
			{
				cout << "Display: No filename given; using '" << srcFilename << "'." << endl;
			}
			else
			{
				srcFilename = argv[2];
			}

			ImportAndDisplayDocument( srcFilename );
		}
		else if( strcmp( cmd, "diff" ) == 0 )
		{
			TDiff( argc - 2, argv + 2 );
		}
#if 0
		else if( strcmp( cmd, "lzwtest" ) == 0 )
		{
			LZWTest( argc - 2, argv + 2 );
		}
		else if( strcmp( cmd, "readgif" ) == 0 )
		{
			ReadGIF( ( argc > 2 ) ? argv[2] : srcImageBasePath + "GIF87a\\RedEnsgn.gif" );
		}
		else
		{
			const int i = ( strcmp( cmd, "test" ) == 0 ) ? 2 : 1;

			DrawTestImage1( argc - i, argv + i );
		}
#endif
		else if( strcmp( cmd, "readicon" ) == 0 )
		{
			ExploreICOFile( ( argc > 2 ) ? argv[2] : svnSandboxBasePath + "Personal\\trunk\\Pictures\\Icons\\Cross.ico" );
		}
		else if( strcmp( cmd, "non8test" ) == 0 )
		{
			Non8BitPaletteTest( defaultSrcImageFilename );
		}
	}
	catch( ExAPException & e )
	{
		cout << "ExAPException was thrown at file " << e.getFile() << ", line " << e.getLine() << "." << endl;
		cout << "  Exception message: '" << e.getMsg() << "'." << endl;
	}
	catch( ... )
	{
		cout << "Unrecognized exception was caught in main()." << endl;
	}

	return( 0 );
} // main()


// **** End of File ****
