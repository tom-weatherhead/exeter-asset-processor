/*
 *  ExAPReadTIFFTags.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Tue Jul 1 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */


#ifndef _EXAPREADTIFFTAGS_H_
#define _EXAPREADTIFFTAGS_H_		1


#include <map>
#include <vector>
#include <string>

#include "General\\ExAPAutoPtr.h"
#include "UtilLib\\ExUtilFileReaderBaseInterface.h"
#include "ExAPTIFFTags.h"


class ExAPTIFFImageMetadata
{
public:
	unsigned long m_newSubfileType;
	int m_imageWidth;
	int m_imageLength;	// ie. Height; number of rows.
	unsigned short m_bitsPerSample;
	unsigned short m_compressionType;
	unsigned short m_photometricInterpretation;
	std::string m_imageDescription;
	std::vector<long> m_stripOffsets;
	unsigned short m_orientation;
	unsigned short m_samplesPerPixel;
	int m_rowsPerStrip;
	std::vector<int> m_stripByteCounts;
	unsigned short m_differencingPredictor;
	// Skip the resolution info for now.

	ExAPTIFFImageMetadata( void )
		: m_newSubfileType( 0 ),
			m_imageWidth( 0 ),
			m_imageLength( 0 ),
			m_bitsPerSample( 0 ),
			m_compressionType( 1 ),						// Default: no compression
			m_photometricInterpretation( 0 ),
			m_orientation( 1 ),
			m_samplesPerPixel( 0 ),
			m_rowsPerStrip( 0 ),
			m_differencingPredictor( 1 )			// Default: none
	{
	}
}; // class ExAPTIFFImageMetadata


class ExAPTIFFTagReader		// Abstract base class (interface).
{
public:
	virtual void ReadTag(
		ExAPAutoPtr<ExUtilFileReaderBaseInterface> file,
		const IFDFieldType & field,
		ExAPTIFFImageMetadata & tiffImageMetadata ) = 0;
}; // class ExAPTIFFTagReader


class ExAPTIFFTagReaderMaster
{
public:
	typedef std::map<ExAPTIFFTagType, ExAPAutoPtr<ExAPTIFFTagReader> > MapType;

	MapType m_readerObjectMap;

	ExAPTIFFTagReaderMaster( void ) throw( ExAPException );

	void AddTagReader( ExAPTIFFTagType t ) throw( ExAPException );
	static ExAPAutoPtr<ExAPTIFFTagReader> TagReaderFactory( ExAPTIFFTagType t ) throw( ExAPException );

	void ReadTag(
		ExAPAutoPtr<ExUtilFileReaderBaseInterface> file,
		const IFDFieldType & field,
		ExAPTIFFImageMetadata & tiffImageMetadata ) const;
}; // class ExAPTIFFTagReaderMaster


#endif // #ifndef _EXAPREADTIFFTAGS_H_


// **** End of File ****
