/*
 *  TDiff.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Thu Jun 26 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#include <cstdio>
#include <memory>
#include <ctime>

#include "General\\ExAPMinMax.h"


bool fileExists( const char * filename )
{
	FILE * fp = fopen( filename, "rb" );

	if( fp == 0 )
	{
		printf( "File '%s' does not exist.\n", filename );
		return( false );
	}

	printf( "File '%s' exists.\n", filename );
	fclose( fp );
	return( true );
} // fileExists()


bool writeRandomFile( const char * filename, size_t fileSize )
{
	FILE * fp = fopen( filename, "wb" );

	if( fp == 0 )
	{
		printf( "writeRandomFile: Couldn't create file '%s'.\n", filename );
		return( false );
	}

	printf( "Creating file '%s' containing %u random bytes.\n", filename, fileSize );

	static const size_t bufSize = 26;
	unsigned char buf[bufSize];

	srand( time( 0 ) );

	while( fileSize > 0 )
	{
		const size_t bytesToWriteThisPass = min( fileSize, bufSize );

		// Fill the buffer with random data.

		for( size_t i = 0; i < bytesToWriteThisPass; ++i )
		{
			// Don't take the least significant bits; they aren't as random.
			buf[i] = static_cast<unsigned char>( rand() & 0xFF );
#ifdef TAW_TRACE
			printf( "%02X ", static_cast<unsigned int>( buf[i] ) );
#endif
		}

#ifdef TAW_TRACE
		printf( "\n" );
#endif

		if( fwrite( buf, sizeof( buf[0] ), bytesToWriteThisPass, fp ) != bytesToWriteThisPass )
		{
			printf( "writeRandomFile: Error while writing file '%s'; aborting.\n", filename );
			break;
		}

		fileSize -= bytesToWriteThisPass;
	}

	fclose( fp );
	return( true );
} // writeRandomFile()


void TDiff2( const char * filename1, const char * filename2 )
{
	FILE * fp1 = fopen( filename1, "rb" );

	if( fp1 == 0 )
	{
		printf( "Unable to read file '%s'.\n", filename1 );
		return;
	}

	FILE * fp2 = fopen( filename2, "rb" );

	if( fp2 == 0 )
	{
		printf( "Unable to read file '%s'.\n", filename2 );
		fclose( fp1 );
		return;
	}

	static const size_t bufSize = 64;
	unsigned char buf1[bufSize];
	unsigned char buf2[bufSize];
	int bytesRead = 0;
	bool differenceFound = false;

	while( !differenceFound  &&  !feof( fp1 )  &&  !feof( fp2 ) )
	{
		memset( buf1, 0, sizeof( buf1 ) );
		memset( buf2, 0, sizeof( buf2 ) );

		const int bytesRead1 = fread( buf1, sizeof( buf1[0] ), bufSize, fp1 );
		const int bytesRead2 = fread( buf2, sizeof( buf2[0] ), bufSize, fp2 );

		if( bytesRead1 != bytesRead2 )
		{
			const int len = min( bytesRead1, bytesRead2 );
			int i = 0;

			differenceFound = true;

			for( ; i < len; ++i )
			{

				if( buf1[i] != buf2[i] )
				{
					break;
				}
			}

			printf( "The two files differ in size, but are identical before byte %d.\n", bytesRead + i );
		}

		for( int i = 0; i < bytesRead1; ++i )
		{

			if( buf1[i] != buf2[i] )
			{
				differenceFound = true;
				printf( "The first difference is at byte %d: 0x%02X vs 0x%02X.\n", bytesRead + i, buf1[i], buf2[i] );
				break;
			}
		}

		bytesRead += static_cast<int>( bufSize );
	}

	if( !differenceFound )
	{
		printf( "The two files are equal.\n" );
	}

	fclose( fp2 );
	fclose( fp1 );
} // TDiff2()


void TDiff( int argc, char * argv[] )
{

	if( argc != 2 )
	{
		printf( "Usage: AssetProcessor.exe tdiff file1 file2\n" );
		return;
	}

	TDiff2( argv[0], argv[1] );
} // TDiff()


// **** End of File ****
