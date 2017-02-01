// UUCode.cpp - Part of Helix
// Copyright (c) 2002 by Tom Weatherhead.  All rights reserved.
// Started December 14, 2002

#include "CryptoLib\\ExCryptoUUCode.h"


//void UUEncode( char * pSrcFilename, FILE * pSrcFile, FILE * pDstFile )
void ExCryptoUUCode::UUEncode( const std::string & SrcFilenameStdStr, ExAPAutoPtr<ExUtilFileReaderBaseInterface> pSrcFile, ExAPAutoPtr<ExUtilFileWriterBaseInterface> pDstFile )
{
	const char * pSrcFilename = SrcFilenameStdStr.c_str();
	unsigned char acTable[64];
	int i;

	acTable[0] = '`';	// We could use a space instead.

	for( i = 1; i < 64; ++i )
	{
		acTable[i] = (unsigned char)( i + 32 );
	}

	ExAssert( pSrcFilename != 0 );

	for( i = strlen( pSrcFilename ); i > 0  &&  pSrcFilename[i - 1] != '\\'; --i )
	{
	}

	ExAssert( strlen( pSrcFilename + i ) > 0 );
	//fprintf( pDstFile, "begin 664 %s\n", pSrcFilename + i );
	pDstFile->WriteNullTerminatedString( (const unsigned char *)"begin 664 " );
	pDstFile->WriteNullTerminatedString( (const unsigned char *)( pSrcFilename + i ) );
	pDstFile->WriteEndLine();

	while( !pSrcFile->EndOfFile() )
	{
		unsigned char aucSrcData[45];
		//const int knBytesRead = fread( aucSrcData, 1, 45, pSrcFile );
		const int knBytesRead = pSrcFile->GetBytes( aucSrcData, 45 );

		//fputc( acTable[knBytesRead], pDstFile );
		pDstFile->WriteByte( acTable[knBytesRead] );

		for( i = 0; i < knBytesRead; i += 3 )
		{
			const unsigned int k = (unsigned int)aucSrcData[i] + ( (unsigned int)aucSrcData[i + 1] << 8 ) + ( (unsigned int)aucSrcData[i + 2] << 16 );

			//fputc( acTable[k & 63], pDstFile );
			//fputc( acTable[(k >> 6) & 63], pDstFile );
			//fputc( acTable[(k >> 12) & 63], pDstFile );
			//fputc( acTable[k >> 18], pDstFile );
			pDstFile->WriteByte( acTable[k & 63] );
			pDstFile->WriteByte( acTable[(k >> 6) & 63] );
			pDstFile->WriteByte( acTable[(k >> 12) & 63] );
			pDstFile->WriteByte( acTable[k >> 18] );
		}

		//fputc( '\n', pDstFile );
		pDstFile->WriteEndLine();
	}

	//fprintf( pDstFile, "`\n" );
	pDstFile->WriteNullTerminatedString( (const unsigned char *)"end" );
	pDstFile->WriteEndLine();
}


//bool UUDecode( FILE * pSrcFile )
bool ExCryptoUUCode::UUDecode( ExAPAutoPtr<ExUtilFileReaderBaseInterface> pSrcFile )
{
	//FILE * pDstFile = 0;
	ExAPAutoPtr<ExUtilFileWriterBaseInterface> pDstFile;

	while( !pSrcFile->EndOfFile() )
	{
		// Get a line of the src text file.
		char acSrcData[128];

#if 1
		pSrcFile->GetNullTerminatedString( (unsigned char *)acSrcData, sizeof( acSrcData ) );
#else
		fgets( acSrcData, sizeof( acSrcData ), pSrcFile );

		// Truncate any appended newline:
		acSrcData[sizeof( acSrcData ) - 1] = '\0';

		if( acSrcData[strlen( acSrcData ) - 1] == '\n' )
		{
			acSrcData[strlen( acSrcData ) - 1] = '\0';
		}
#endif

		if( strlen( acSrcData ) == 0 )
		{
			// continue;
		}
		else if( !memcmp( acSrcData, "begin", 5 )  &&  strlen( acSrcData ) > 10  &&  pDstFile == 0 )
		{
			const std::string filenameStdStr( acSrcData + 10 );

			//pDstFile = fopen( acSrcData + 10, "wb" );
			pDstFile = ExUtilFileWriterBaseInterface::StdioCreate( filenameStdStr );
		}
		else if( pDstFile == 0 )
		{
			// continue;
		}
		else if( !memcmp( acSrcData, "end", 3 ) )
		{
			//fclose( pDstFile );
			return( true );
		}
		else if( acSrcData[0] > 32  &&  acSrcData[0] <= 77 )
		{
			const int knBytesToWrite = (int)( acSrcData[0] - 32 );
			char acDstData[45];
			int nDstIndex = 0;

			for( int i = 1; i < strlen( acSrcData ); i += 4 )
			{
				int j;
				int anSrc1[4];

				for( j = 0; j < 4; ++j )
				{
					const char c = acSrcData[i + j];

					if( c < 32  ||  c > 96 )
					{
						break;
					}

					anSrc1[j] = (int)( ( c - 32 ) & 63 );
				}

				for( ; j < 4; ++j )
				{
					anSrc1[j] = 0;
				}

				int k = 0;

				for( j = 0; j < 4; ++j )
				{
					k |= ( anSrc1[j] << ( j * 6 ) );
				}

				for( j = 0; j < 3; ++j )
				{
					acDstData[nDstIndex++] = (char)( k >> ( j * 8 ) );
				}
			}

			//fwrite( acDstData, 1, knBytesToWrite, pDstFile );
			pDstFile->WriteBytes( (const unsigned char *)acDstData, knBytesToWrite );
		}
	}

#if 0
	if( pDstFile != 0 )
	{
		fclose( pDstFile ); // Dst file truncated?
	}
#endif

	return( false ); // No "end" found.
}


// **** End of File ****
