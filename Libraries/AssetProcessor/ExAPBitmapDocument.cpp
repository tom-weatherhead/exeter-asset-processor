/*
 *  ExAPBitmapDocument.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Wed Apr 30 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */

#ifdef _MSC_VER			// TAW 2002/03/11 : ie. if MSVC
#ifndef MACINTOSH		// TAW 2002/02/23 : ie. if MSVC

// Suppress warning C4786: Identifier truncated to 255 chars in the debug info.
#pragma warning(disable : 4786)

#endif
#endif


//lint -e1050 : Expected a template argument list '<...>' for template 'std::iterator<<1>,<2>,<3>>'
#include "UtilLib\\ExUtilFileReaderBaseInterface.h"
#include "ExAPBitmapDocument.h"
#include "ExAPBitmap8bpch.h"
#include "ExAPImportTIFF.h"
//lint +e1050


// Function prototypes.
ExAPAutoPtr<ExAPBitmap> ImportGIF( ExAPAutoPtr<ExUtilFileReaderBaseInterface> file ) throw( ExAPException );
ExAPAutoPtr<ExAPBitmap> ImportBMP( ExAPAutoPtr<ExUtilFileReaderBaseInterface> file ) throw( ExAPException );
ExAPAutoPtr<ExAPBitmap> ImportPNG( ExAPAutoPtr<ExUtilFileReaderBaseInterface> file ) throw( ExAPException );


// Constructor.

ExAPBitmapDocument::ExAPBitmapDocument( const std::string & filePath ) throw( ExAPException )
{
	// Throws an exception if the file is not found.
	// "file" is not const because the byte order may need to be set.
	//ExAPFileReaderStdio file( filePath );
	ExAPAutoPtr<ExUtilFileReaderBaseInterface> file = ExUtilFileReaderBaseInterface::StdioCreate( filePath );

	const char * filePathCStr = filePath.c_str();
	const char * lastPeriod = strrchr( filePathCStr, '.' );

	if( lastPeriod == 0 )
	{
		std::string exceptionMsg;

		exceptionMsg = "Error: the file '";
		exceptionMsg += file->GetFilename();
		exceptionMsg += "' lacks an extension.";
		ThrowExAPExceptionWithMsg( ExAPException::DocumentCreationError, exceptionMsg );
	}

	const char * fileExtension = lastPeriod + 1;

	if( strcmp( fileExtension, "tif" ) == 0  ||  strcmp( fileExtension, "tiff" ) == 0 )
	{
		ExAPTIFFReader reader( file );

		while( reader.NextIFDExists() )
		{
			ExAPAutoPtr<ExAPBitmap> image = reader.ImportImage();

			if( image == 0 )
			{
				ThrowExAPException( ExAPException::DocumentCreationError );
			}

#if 0 // def TAW_TRACE
			image->TestDisplay();
#endif
			m_v.push_back( image );
		}
	}
	else
	{
		// The file probably contains a single image.
		ExAPAutoPtr<ExAPBitmap> image = 0;

		if( strcmp( fileExtension, "gif" ) == 0 )
		{
			image = ImportGIF( file );
		}
		else if( strcmp( fileExtension, "jpg" ) == 0  ||  strcmp( fileExtension, "jpeg" ) == 0 )
		{
			image = ExAPBitmap8bpch::ReadJPEG( file /* ->GetFilePointer() */ );
		}
		else if( strcmp( fileExtension, "bmp" ) == 0 )
		{
			file->SetEndianness( true );
			image = ImportBMP( file );
		}
		else if( strcmp( fileExtension, "png" ) == 0 )
		{
			file->SetEndianness( false );
			image = ImportPNG( file );
		}

		if( image == 0 )
		{
			ThrowExAPException( ExAPException::DocumentCreationError );
		}

		m_v.push_back( image );
	}
} // ExAPBitmapDocument::ExAPBitmapDocument()


// Destructor.

ExAPBitmapDocument::~ExAPBitmapDocument( void )
{
	// The bitmaps are freed automatically via the AutoPtrs.
} // ExAPBitmapDocument::~ExAPBitmapDocument()


void ExAPBitmapDocument::TestDisplay( void ) const throw( ExAPException )
{
	BitmapVectorType::const_iterator it = m_v.begin();
	const BitmapVectorType::const_iterator itEnd = m_v.end();

	while( it != itEnd )
	{
		ExAPBitmap * image = *it++;

		if( image != 0 )
		{
			image->TestDisplay();
#if 0
			// Delay for one second.
			sleep( 1000 );
#endif
		}
	}
} // ExAPBitmapDocument::TestDisplay()


ExAPAutoPtr<ExAPBitmap> ExAPBitmapDocument::GetComposite( void ) const throw( ExAPException )
{

	if( m_v.empty() )
	{
		ThrowExAPException( ExAPException::DocumentCreationError );
	}

	ExAPAutoPtr<ExAPBitmap> firstImage = m_v.front();
	ExAPAutoPtr<ExAPBitmap> newImage = firstImage->Duplicate();
	return( newImage );
} // ExAPBitmapDocument::GetComposite()


// **** End of File ****
