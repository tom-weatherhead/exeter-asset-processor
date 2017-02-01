/*
 *  ExAPImportJPEG.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Tue Apr 20 2004.
 *  Copyright (c) 2004 by Tom Weatherhead. All rights reserved.
 *
 */

#ifdef _MSC_VER			// TAW 2002/03/11 : ie. if MSVC
#ifndef MACINTOSH		// TAW 2002/02/23 : ie. if MSVC

// Suppress warning C4786: Identifier truncated to 255 chars in the debug info.
#pragma warning(disable : 4786)

#endif
#endif


//lint -e1050 : Expected a template argument list '<...>' for template 'std::iterator<<1>,<2>,<3>>'
#include <vector>

#include "UtilLib\\ExUtilFileReaderBaseInterface.h"
//lint +e1050


#define TAW_TRACE		1


void ExploreJPEGFile( const std::string & filename )
{
	ExAPAutoPtr<ExUtilFileReaderBaseInterface> file = ExUtilFileReaderBaseInterface::StdioCreate( filename, false );	// Big-endian.
	const int headerByte0 = file->GetByteAsInt();
	const int headerByte1 = file->GetByteAsInt();

	if( file->EndOfFile() )
	{
		printf( "Error: End Of File encountered before Start Of Image marker found.\n" );;
		return;
	}

	if( headerByte0 != 0xFF  ||  headerByte1 != 0xD8 )
	{
		printf( "Error: JPEG file Start Of Image marker not found.\n" );
		printf( "Expected 0xFF 0xD8; found 0x%02X 0x%02X.\n", headerByte0, headerByte1 );
		return;
	}

	printf( "JPEG file Start Of Image marker found.\n" );

	// Here is the marker reader.

	for( ; ; )
	{
		int markerByte0;
		int markerByte1;

		// Search for the next 0xFF byte, or EOF.

		do
		{
			markerByte0 = file->GetByteAsInt();

			if( file->EndOfFile() )
			{
				printf( "Error: Searching for 0xFF, found End Of File.\n" );
				return;
			}

			printf( "Searching for 0xFF, found 0x%02X.\n", markerByte0 );
		}
		while( markerByte0 != 0xFF );

		// Search for the next non-0xFF byte, or EOF.

		do
		{
			markerByte1 = file->GetByteAsInt();

			if( file->EndOfFile() )
			{
				printf( "Error: Searching for non-0xFF, found End Of File.\n" );
				return;
			}

			printf( "Searching for non-0xFF, found 0x%02X.\n", markerByte1 );
		}
		while( markerByte1 == 0xFF );

		char * markerTypeString = "unknown";

		switch( markerByte1 )
		{
			case 0xC0:
				markerTypeString = "M_SOF0";
				break;

			case 0xC1:
				markerTypeString = "M_SOF1";
				break;

			case 0xC2:
				markerTypeString = "M_SOF2";
				break;

			case 0xC3:
				markerTypeString = "M_SOF3";
				break;

			case 0xC4:
				markerTypeString = "M_DHT (Huffman Table?)";
				break;

			case 0xC5:
				markerTypeString = "M_SOF5";
				break;

			case 0xC6:
				markerTypeString = "M_SOF6";
				break;

			case 0xC7:
				markerTypeString = "M_SOF7";
				break;

			case 0xC9:
				markerTypeString = "M_SOF9";
				break;

			case 0xCA:
				markerTypeString = "M_SOF10";
				break;

			case 0xCB:
				markerTypeString = "M_SOF11";
				break;

			case 0xCD:
				markerTypeString = "M_SOF13";
				break;

			case 0xCE:
				markerTypeString = "M_SOF14";
				break;

			case 0xCF:
				markerTypeString = "M_SOF15";
				break;

			case 0xD8:
				markerTypeString = "M_SOI (Start of Image)";
				break;

			case 0xD9:
				markerTypeString = "M_EOI (End of Image)";
				break;

			case 0xDA:
				markerTypeString = "M_SOS (Start of Scan)";
				break;

			case 0xDB:
				markerTypeString = "M_DQT (Quantization Table?)";
				break;

			case 0xE0:
				markerTypeString = "M_APP0";
				break;

			case 0xE1:
				markerTypeString = "M_APP1";
				break;

			case 0xE2:
				markerTypeString = "M_APP2";
				break;

			case 0xE3:
				markerTypeString = "M_APP3";
				break;

			case 0xE4:
				markerTypeString = "M_APP4";
				break;

			case 0xE5:
				markerTypeString = "M_APP5";
				break;

			case 0xE6:
				markerTypeString = "M_APP6";
				break;

			case 0xE7:
				markerTypeString = "M_APP7";
				break;

			case 0xE8:
				markerTypeString = "M_APP8";
				break;

			case 0xE9:
				markerTypeString = "M_APP9";
				break;

			case 0xEA:
				markerTypeString = "M_APP10";
				break;

			case 0xEB:
				markerTypeString = "M_APP11";
				break;

			case 0xEC:
				markerTypeString = "M_APP12";
				break;

			case 0xFE:
				markerTypeString = "M_COM (Comment)";
				break;

			default:
				break;
		}

		printf( "Marker type is '%s'.\n", markerTypeString );

		if( markerByte1 == 0xD9  ||  markerByte1 == 0xDA )
		{
			printf( "Closing file.\n" );
			break;
		}

		const unsigned short markerLength = file->GetWord();
		/* const */ int markerBodyLength = int( markerLength ) - 2;

		if( markerBodyLength < 0 )
		{
			printf( "Error: Marker body length is %d.\n", markerBodyLength );
			break;
		}

		if( markerByte1 == 0xC0 )	// M_SOF0 : Baseline JPEG image.
		{
			const int dataPrecision = file->GetByteAsInt();
			const int imageHeight = int( file->GetWord() );
			const int imageWidth = int( file->GetWord() );
			const int numComponents = file->GetByteAsInt();

			printf( "Image metadata:\n" );
			printf( "  Data precision: %d\n", dataPrecision );
			printf( "  Image height: %d\n", imageHeight );
			printf( "  Image width: %d\n", imageWidth );
			printf( "  Number of components: %d\n", numComponents );

			for( int i = 0; i < numComponents; ++i )
			{
				const int componentIDCode = file->GetByteAsInt();
				const int HVSamplingFactors = file->GetByteAsInt();
				const int quantizationTableNumber = file->GetByteAsInt();

				printf( "Component %d info:\n", i );
				printf( "  Component ID code: %d\n", componentIDCode );
				printf( "  H V sampling factors: %d\n", HVSamplingFactors );
				printf( "  Quantization table number: %d\n", quantizationTableNumber );
			}
		}
		else if( markerByte1 == 0xC4 )	// M_DHT
		{

			while( markerBodyLength > 16 )
			{
				int index = file->GetByteAsInt();
				int count = 0;
				std::vector<int> bits( 17 );
				std::vector<int> huffVal( 256 );

				for( int i = 1; i <= 16; ++i )
				{
					bits[i] = file->GetByteAsInt();
					count += bits[i];
				}

				markerBodyLength -= 17;

				printf( "  M_DHT: count == %d; remaining marker body length == %d\n", count, markerBodyLength );

				if( count > 256  ||  count > markerBodyLength )
				{
					printf( "Error: count is too large.\n" );
					break;	// Throw exception.
				}

				for( int j = 0; j < count; ++j )
				{
					huffVal[j] = file->GetByteAsInt();
				}

				markerBodyLength -= count;

				if( ( index & 0x10 ) != 0 )
				{
					// AC table definition.
					index -= 0x10;
					printf( "  This has been an AC table definition; index == %d.\n", index );
				}
				else
				{
					// DC table definition.
					printf( "  This has been a DC table definition; index == %d.\n", index );
				}
			}

			if( markerBodyLength != 0 )
			{
				printf( "Error: %d extra bytes in this marker.\n", markerBodyLength );
				break;	// Throw exception.
			}
		}
		else if( markerByte1 == 0xDB )	// M_DQT
		{
			static const int numQuantizationTables = 4;
			static const int DCTSize2 = 64;
			static const int jpegNaturalOrder[DCTSize2 + 16] = {
				  0,  1,  8, 16,  9,  2,  3, 10,
				 17, 24, 32, 25, 18, 11,  4,  5,
				 12, 19, 26, 33, 40, 48, 41, 34,
				 27, 20, 13,  6,  7, 14, 21, 28,
				 35, 42, 49, 56, 57, 50, 43, 36,
				 29, 22, 15, 23, 30, 37, 44, 51,
				 58, 59, 52, 45, 38, 31, 39, 46,
				 53, 60, 61, 54, 47, 55, 62, 63,
				 63, 63, 63, 63, 63, 63, 63, 63, // Extra entries for safety in decoder.
				 63, 63, 63, 63, 63, 63, 63, 63
			};

			while( markerBodyLength > 0 )
			{
				int n = file->GetByteAsInt();
				const int prec = n >> 4;

				n &= 0x0F;

				if( n >= numQuantizationTables )
				{
					printf( "Quantization table number is too large (%d); should not exceed %d.\n", n, numQuantizationTables );
					break;
				}

				// Allocate quantization table #n.
				std::vector<unsigned short> qTableN( DCTSize2 );

				for( int i = 0; i < DCTSize2; ++i )
				{
					const unsigned short temp = ( prec != 0 ) ? (unsigned short)( file->GetWord() ) : (unsigned short)( file->GetByteAsInt() );

					qTableN[jpegNaturalOrder[i]] = temp;
				}

#if 1
				printf( "Contents of Quantization Table #%d:\n", n );

				for( int j = 0; j < DCTSize2; ++j )
				{
					printf( "  %02X", qTableN[j] );

					if( j % 8 == 7 )
					{
						printf( "\n" );
					}
				}
#endif

				markerBodyLength -= ( prec != 0 ) ? 2 * DCTSize2 + 1 : DCTSize2 + 1;
			}
		}
		else if( markerByte1 == 0xE0 )	// M_APP0
		{

			if( markerBodyLength != 14 )
			{
				printf( "Error: Length of body of APP0 marker is %d; should be 14.\n", markerBodyLength );
				break;
			}

			const int name0 = file->GetByteAsInt();
			const int name1 = file->GetByteAsInt();
			const int name2 = file->GetByteAsInt();
			const int name3 = file->GetByteAsInt();
			const int name4 = file->GetByteAsInt();

			if( name0 == 0x4A  &&  name1 == 0x46  &&  name2 == 0x49  &&  name3 == 0x46  &&  name4 == 0 )
			{
				// JFIF
				const int JFIFMajorVersion = file->GetByteAsInt();
				const int JFIFMinorVersion = file->GetByteAsInt();
				const int densityUnit = file->GetByteAsInt();
				const int xDensity = int( file->GetWord() );
				const int yDensity = int( file->GetWord() );
				const int byte12 = file->GetByteAsInt();
				const int byte13 = file->GetByteAsInt();

				printf( "M_APP0 marker of type JFIF:\n" );
				printf( "  JFIF major version number: %d\n", JFIFMajorVersion );
				printf( "  JFIF minor version number: %d\n", JFIFMinorVersion );
				printf( "  Density unit: %d\n", densityUnit );
				printf( "  X density: %d\n", xDensity );
				printf( "  Y density: %d\n", yDensity );
				printf( "  Bytes 12 and 13: %02X %02X\n", byte12, byte13 );
			}
			else if( name0 == 0x4A  &&  name1 == 0x46  &&  name2 == 0x58  &&  name3 == 0x58  &&  name4 == 0 )
			{
				// JFXX
				printf( "M_APP0 marker of type JFXX.\n" );
				file->SkipBytes( 9 );
			}
			else
			{
				printf( "Warning: Unrecognized type of M_APP0 marker: %02X %02X %02X %02X.\n",
					name0, name1, name2, name3 );
				break;
			}
		}
		else if( markerBodyLength > 0 )
		{
			printf( "Skipping %d bytes...\n", markerBodyLength );
			file->SkipBytes( markerBodyLength );

			if( file->EndOfFile() )
			{
				printf( "Error: End of File encountered.\n" );
				break;
			}
		}
	}
} // ExploreJPEGFile()


// **** End of File ****
