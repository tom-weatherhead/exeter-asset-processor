/*
 *  ExAPReadTIFFTags.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Tue Jul 1 2003.
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
#include "General\\ExAPException.h"
#include "ExAPReadTIFFTags.h"
//lint +e1050


//#define TAW_TRACE		1


// There are lots of unreferenced symbols in this file.
//lint -e715 : Symbol not referenced.


// **** class ExAPTIFFTagReader_NewSubfileType ****


class ExAPTIFFTagReader_NewSubfileType : public ExAPTIFFTagReader
{
public:
	virtual void ReadTag(
		ExAPAutoPtr<ExUtilFileReaderBaseInterface> file,
		const IFDFieldType & field,
		ExAPTIFFImageMetadata & tiffImageMetadata );
}; // class ExAPTIFFTagReader_NewSubfileType


void ExAPTIFFTagReader_NewSubfileType::ReadTag(
	ExAPAutoPtr<ExUtilFileReaderBaseInterface> file,
	const IFDFieldType & field,
	ExAPTIFFImageMetadata & tiffImageMetadata )
{
	tiffImageMetadata.m_newSubfileType = field.m_valueOffset;
#ifdef TAW_TRACE
	printf( "TagReader: NewSubfileType:\n" );
	printf( "  Image is %sa reduced-resolution version of another image in this file.\n", ( tiffImageMetadata.m_newSubfileType & 1 ) != 0 ? "" : "not " );
	printf( "  Image is %sa single page of a multi-page image.\n", ( tiffImageMetadata.m_newSubfileType & 2 ) != 0 ? "" : "not " );
	printf( "  Image is %sa transparency mask for another image in this file.\n", ( tiffImageMetadata.m_newSubfileType & 4 ) != 0 ? "" : "not " );
#endif
} // ExAPTIFFTagReader_NewSubfileType::ReadTag()


// **** class ExAPTIFFTagReader_ImageWidth ****


class ExAPTIFFTagReader_ImageWidth : public ExAPTIFFTagReader
{
public:
	virtual void ReadTag(
		ExAPAutoPtr<ExUtilFileReaderBaseInterface> file,
		const IFDFieldType & field,
		ExAPTIFFImageMetadata & tiffImageMetadata );
}; // class ExAPTIFFTagReader_ImageWidth


void ExAPTIFFTagReader_ImageWidth::ReadTag(
	ExAPAutoPtr<ExUtilFileReaderBaseInterface> file,
	const IFDFieldType & field,
	ExAPTIFFImageMetadata & tiffImageMetadata )
{
	tiffImageMetadata.m_imageWidth = static_cast<int>( field.m_valueOffset );
#ifdef TAW_TRACE
	printf( "TagReader: ImageWidth set to %d.\n", tiffImageMetadata.m_imageWidth );
#endif
} // ExAPTIFFTagReader_ImageWidth::ReadTag()


// **** class ExAPTIFFTagReader_ImageLength ****


class ExAPTIFFTagReader_ImageLength : public ExAPTIFFTagReader
{
public:
	virtual void ReadTag(
		ExAPAutoPtr<ExUtilFileReaderBaseInterface> file,
		const IFDFieldType & field,
		ExAPTIFFImageMetadata & tiffImageMetadata );
}; // class ExAPTIFFTagReader_ImageLength


void ExAPTIFFTagReader_ImageLength::ReadTag(
	ExAPAutoPtr<ExUtilFileReaderBaseInterface> file,
	const IFDFieldType & field,
	ExAPTIFFImageMetadata & tiffImageMetadata )
{
	tiffImageMetadata.m_imageLength = static_cast<int>( field.m_valueOffset );
#ifdef TAW_TRACE
	printf( "TagReader: ImageLength set to %d.\n", tiffImageMetadata.m_imageLength );
#endif
} // ExAPTIFFTagReader_ImageLength::ReadTag()


// **** class ExAPTIFFTagReader_BitsPerSample ****


class ExAPTIFFTagReader_BitsPerSample : public ExAPTIFFTagReader
{
public:
	virtual void ReadTag(
		ExAPAutoPtr<ExUtilFileReaderBaseInterface> file,
		const IFDFieldType & field,
		ExAPTIFFImageMetadata & tiffImageMetadata );
}; // class ExAPTIFFTagReader_BitsPerSample


void ExAPTIFFTagReader_BitsPerSample::ReadTag(
	ExAPAutoPtr<ExUtilFileReaderBaseInterface> file,
	const IFDFieldType & field,
	ExAPTIFFImageMetadata & tiffImageMetadata )
{
	ExAssert( field.m_dataCount > 0 )

	if( field.m_dataCount == 1 )
	{
		tiffImageMetadata.m_bitsPerSample = static_cast<unsigned short>( field.m_valueOffset );
	}
	else
	{
		const long offset = static_cast<long>( field.m_valueOffset );
	
		file->SetOffset( offset );
		tiffImageMetadata.m_bitsPerSample = file->GetWord();

		for( unsigned int i = 1; i < field.m_dataCount; ++i )
		{
			const unsigned short bitsPerSample2 = file->GetWord();

			ExAssert( tiffImageMetadata.m_bitsPerSample == bitsPerSample2 )
		}
	}

#ifdef TAW_TRACE
	printf( "TagReader: BitsPerSample set to %hu.\n", tiffImageMetadata.m_bitsPerSample );
#endif
} // ExAPTIFFTagReader_BitsPerSample::ReadTag()


// **** class ExAPTIFFTagReader_CompressionType ****


class ExAPTIFFTagReader_CompressionType : public ExAPTIFFTagReader
{
public:
	virtual void ReadTag(
		ExAPAutoPtr<ExUtilFileReaderBaseInterface> file,
		const IFDFieldType & field,
		ExAPTIFFImageMetadata & tiffImageMetadata );
}; // class ExAPTIFFTagReader_CompressionType


void ExAPTIFFTagReader_CompressionType::ReadTag(
	ExAPAutoPtr<ExUtilFileReaderBaseInterface> file,
	const IFDFieldType & field,
	ExAPTIFFImageMetadata & tiffImageMetadata )
{
	// 1: No compression.
	// 2: Modified Huffman compression (CCITT 1D).
	// 3: Group 3 Fax.
	// 4: Group 4 Fax.
	// 5: LZW.
	// 6: JPEG.
	// 32773: PackBits compression.
	tiffImageMetadata.m_compressionType = static_cast<unsigned short>( field.m_valueOffset );
#ifdef TAW_TRACE
	const char * str = "???";

	switch( tiffImageMetadata.m_compressionType )
	{
		case 1:
			str = "No compression";
			break;

		case 2:
			str = "Modified Huffman compression";
			break;

		case 3:
			str = "Group 3 Fax";
			break;

		case 4:
			str = "Group 4 Fax";
			break;

		case 5:
			str = "LZW compression";
			break;

		case 6:
			str = "JPEG compression";
			break;

		case 32773:
			str = "PackBits compression";
			break;

		default:
			break;
	}

	printf( "TagReader: CompressionType set to %hu (%s).\n",
		tiffImageMetadata.m_compressionType, str );
#endif
} // ExAPTIFFTagReader_CompressionType::ReadTag()


// **** class ExAPTIFFTagReader_PhotometricInterpretation ****


class ExAPTIFFTagReader_PhotometricInterpretation : public ExAPTIFFTagReader
{
public:
	virtual void ReadTag(
		ExAPAutoPtr<ExUtilFileReaderBaseInterface> file,
		const IFDFieldType & field,
		ExAPTIFFImageMetadata & tiffImageMetadata );
}; // class ExAPTIFFTagReader_PhotometricInterpretation


void ExAPTIFFTagReader_PhotometricInterpretation::ReadTag(
	ExAPAutoPtr<ExUtilFileReaderBaseInterface> file,
	const IFDFieldType & field,
	ExAPTIFFImageMetadata & tiffImageMetadata )
{
	// 0: White is zero. (1-bit B&W, 4-bit Greyscale, or 8-bit Greyscale)
	// 1: Black is zero. (1-bit B&W, 4-bit Greyscale, or 8-bit Greyscale)
	// 2: RGB.
	// 3: RGB Paletted.
	// 4: Transparency mask.
	// 5: CMYK.
	// 6: YCbCr.
	// 8: CIELab.
	tiffImageMetadata.m_photometricInterpretation = static_cast<unsigned short>( field.m_valueOffset );
#ifdef TAW_TRACE
	const char * str = "???";

	switch( tiffImageMetadata.m_photometricInterpretation )
	{
		case 0:
			str = "White is zero";
			break;

		case 1:
			str = "Black is zero";
			break;

		case 2:
			str = "RGB";
			break;

		case 3:
			str = "Paletted";
			break;

		case 4:
			str = "Transparency mask";
			break;

		case 5:
			str = "CMYK";
			break;

		case 6:
			str = "YCbCr";
			break;

		case 8:
			str = "CIELab";
			break;

		default:
			break;
	}

	printf( "TagReader: PhotometricInterpretation set to %hu (%s).\n",
		tiffImageMetadata.m_photometricInterpretation, str );
#endif
} // ExAPTIFFTagReader_PhotometricInterpretation::ReadTag()


// **** class ExAPTIFFTagReader_ImageDescription ****


class ExAPTIFFTagReader_ImageDescription : public ExAPTIFFTagReader
{
public:

	static void ReadStringFromTag(
		ExAPAutoPtr<ExUtilFileReaderBaseInterface> file,
		const IFDFieldType & field,
		std::string & dstStr );

	virtual void ReadTag(
		ExAPAutoPtr<ExUtilFileReaderBaseInterface> file,
		const IFDFieldType & field,
		ExAPTIFFImageMetadata & tiffImageMetadata );
}; // class ExAPTIFFTagReader_ImageDescription


void ExAPTIFFTagReader_ImageDescription::ReadStringFromTag(
	ExAPAutoPtr<ExUtilFileReaderBaseInterface> file,
	const IFDFieldType & field,
	std::string & dstStr )
{
	int numBytes = static_cast<int>( field.m_dataCount );
	const long offset = static_cast<long>( field.m_valueOffset );
	
	file->SetOffset( offset );
	(void)dstStr.erase( dstStr.begin(), dstStr.end() );

	while( numBytes-- > 0 )
	{
		const int ch = file->GetByteAsInt();

		if( file->EndOfFile() )
		{
			return;
		}
		else if( isprint( ch ) == 0 )
		{
			// ch is not printable.
			continue;
		}

		dstStr += static_cast<char>( ch );
	}
} // ExAPTIFFTagReader_ImageDescription::ReadStringFromTag()


void ExAPTIFFTagReader_ImageDescription::ReadTag(
	ExAPAutoPtr<ExUtilFileReaderBaseInterface> file,
	const IFDFieldType & field,
	ExAPTIFFImageMetadata & tiffImageMetadata )
{
	ReadStringFromTag( file, field, tiffImageMetadata.m_imageDescription );
#ifdef TAW_TRACE
	printf( "TagReader: Image description: '%s'\n", tiffImageMetadata.m_imageDescription.c_str() );
#endif
} // ExAPTIFFTagReader_ImageDescription::ReadTag()


// **** class ExAPTIFFTagReader_StripOffsets ****


class ExAPTIFFTagReader_StripOffsets : public ExAPTIFFTagReader
{
public:
	virtual void ReadTag(
		ExAPAutoPtr<ExUtilFileReaderBaseInterface> file,
		const IFDFieldType & field,
		ExAPTIFFImageMetadata & tiffImageMetadata );
}; // class ExAPTIFFTagReader_StripOffsets


void ExAPTIFFTagReader_StripOffsets::ReadTag(
	ExAPAutoPtr<ExUtilFileReaderBaseInterface> file,
	const IFDFieldType & field,
	ExAPTIFFImageMetadata & tiffImageMetadata )
{
	unsigned int dataCount = field.m_dataCount;
	const long offset = static_cast<long>( field.m_valueOffset );
	
	file->SetOffset( offset );

#ifdef TAW_TRACE
	printf( "TagReader: There are %u strip offsets.\n", dataCount );
#endif

	tiffImageMetadata.m_stripOffsets.clear();

	while( dataCount-- > 0 )
	{
		const long stripOffset = static_cast<long>( file->GetDoubleWord() );

#if 0 //def TAW_TRACE
		printf( "TagReader: StripOffset: %ld.\n", stripOffset );
#endif

		tiffImageMetadata.m_stripOffsets.push_back( stripOffset );
	}
} // ExAPTIFFTagReader_StripOffsets::ReadTag()


// **** class ExAPTIFFTagReader_Orientation ****


class ExAPTIFFTagReader_Orientation : public ExAPTIFFTagReader
{
public:
	virtual void ReadTag(
		ExAPAutoPtr<ExUtilFileReaderBaseInterface> file,
		const IFDFieldType & field,
		ExAPTIFFImageMetadata & tiffImageMetadata );
}; // class ExAPTIFFTagReader_Orientation


void ExAPTIFFTagReader_Orientation::ReadTag(
	ExAPAutoPtr<ExUtilFileReaderBaseInterface> file,
	const IFDFieldType & field,
	ExAPTIFFImageMetadata & tiffImageMetadata )
{
	tiffImageMetadata.m_orientation = static_cast<unsigned short>( field.m_valueOffset );
#ifdef TAW_TRACE
	printf( "TagReader: Orientation set to %hu.\n", tiffImageMetadata.m_orientation );
#endif
} // ExAPTIFFTagReader_Orientation::ReadTag()


// **** class ExAPTIFFTagReader_SamplesPerPixel ****


class ExAPTIFFTagReader_SamplesPerPixel : public ExAPTIFFTagReader
{
public:
	virtual void ReadTag(
		ExAPAutoPtr<ExUtilFileReaderBaseInterface> file,
		const IFDFieldType & field,
		ExAPTIFFImageMetadata & tiffImageMetadata );
}; // class ExAPTIFFTagReader_SamplesPerPixel


void ExAPTIFFTagReader_SamplesPerPixel::ReadTag(
	ExAPAutoPtr<ExUtilFileReaderBaseInterface> file,
	const IFDFieldType & field,
	ExAPTIFFImageMetadata & tiffImageMetadata )
{
	tiffImageMetadata.m_samplesPerPixel = static_cast<unsigned short>( field.m_valueOffset );
#ifdef TAW_TRACE
	printf( "TagReader: SamplesPerPixel set to %hu.\n", tiffImageMetadata.m_samplesPerPixel );
#endif
} // ExAPTIFFTagReader_SamplesPerPixel::ReadTag()


// **** class ExAPTIFFTagReader_RowsPerStrip ****


class ExAPTIFFTagReader_RowsPerStrip : public ExAPTIFFTagReader
{
public:
	virtual void ReadTag(
		ExAPAutoPtr<ExUtilFileReaderBaseInterface> file,
		const IFDFieldType & field,
		ExAPTIFFImageMetadata & tiffImageMetadata );
}; // class ExAPTIFFTagReader_RowsPerStrip


void ExAPTIFFTagReader_RowsPerStrip::ReadTag(
	ExAPAutoPtr<ExUtilFileReaderBaseInterface> file,
	const IFDFieldType & field,
	ExAPTIFFImageMetadata & tiffImageMetadata )
{
	tiffImageMetadata.m_rowsPerStrip = static_cast<int>( field.m_valueOffset );
#ifdef TAW_TRACE
	printf( "TagReader: RowsPerStrip set to %d.\n", tiffImageMetadata.m_rowsPerStrip );
#endif
} // ExAPTIFFTagReader_RowsPerStrip::ReadTag()


// **** class ExAPTIFFTagReader_StripByteCounts ****


class ExAPTIFFTagReader_StripByteCounts : public ExAPTIFFTagReader
{
public:
	virtual void ReadTag(
		ExAPAutoPtr<ExUtilFileReaderBaseInterface> file,
		const IFDFieldType & field,
		ExAPTIFFImageMetadata & tiffImageMetadata );
}; // class ExAPTIFFTagReader_StripByteCounts


void ExAPTIFFTagReader_StripByteCounts::ReadTag(
	ExAPAutoPtr<ExUtilFileReaderBaseInterface> file,
	const IFDFieldType & field,
	ExAPTIFFImageMetadata & tiffImageMetadata )
{
	unsigned int dataCount = field.m_dataCount;
	// Field types:
	// 1: Byte
	// 2: ASCII (zero-terminated string)
	// 3: Short
	// 4: Long
	// 5: Rational (two Longs)
	// 6: SignedByte
	// 7: Undefined (8 bits)
	// 8: SignedShort
	// 9: SignedLong
	// 10: SignedRational (two SignedLongs)
	// 11: Float (4 bytes)
	// 12: Double (8 bytes)
	const bool dataIsShort = field.m_fieldType == 3;
	const long offset = static_cast<long>( field.m_valueOffset );
#ifdef TAW_TRACE
	int totalByteCount = 0;
#endif

	file->SetOffset( offset );

#ifdef TAW_TRACE
	printf( "TagReader: There are %u strip byte counts.\n", dataCount );
#endif

	tiffImageMetadata.m_stripByteCounts.clear();

	while( dataCount-- > 0 )
	{
		int stripByteCount = 0;

		if( dataIsShort )
		{
			stripByteCount = static_cast<int>( file->GetWord() );
		}
		else
		{
			stripByteCount = static_cast<int>( file->GetDoubleWord() );
		}

#if 0 //def TAW_TRACE
		printf( "TagReader: StripByteCount: %d.\n", stripByteCount );
		totalByteCount += stripByteCount;
#endif

		tiffImageMetadata.m_stripByteCounts.push_back( stripByteCount );
	}

#if 0 //def TAW_TRACE
	printf( "TagReader: Total (compressed) StripByteCount: %d.\n", totalByteCount );
#endif
} // ExAPTIFFTagReader_StripByteCounts::ReadTag()


// **** class ExAPTIFFTagReader_DifferencingPredictor ****


class ExAPTIFFTagReader_DifferencingPredictor : public ExAPTIFFTagReader
{
public:
	virtual void ReadTag(
		ExAPAutoPtr<ExUtilFileReaderBaseInterface> file,
		const IFDFieldType & field,
		ExAPTIFFImageMetadata & tiffImageMetadata );
}; // class ExAPTIFFTagReader_DifferencingPredictor


void ExAPTIFFTagReader_DifferencingPredictor::ReadTag(
	ExAPAutoPtr<ExUtilFileReaderBaseInterface> file,
	const IFDFieldType & field,
	ExAPTIFFImageMetadata & tiffImageMetadata )
{
	tiffImageMetadata.m_differencingPredictor = static_cast<unsigned short>( field.m_valueOffset );
#ifdef TAW_TRACE
	printf( "TagReader: DifferencingPredictor set to %hu.\n", tiffImageMetadata.m_differencingPredictor );
#endif
} // ExAPTIFFTagReader_DifferencingPredictor::ReadTag()


// **** class ExAPTIFFTagReaderMaster ****


ExAPTIFFTagReaderMaster::ExAPTIFFTagReaderMaster( void ) throw( ExAPException )
{
	AddTagReader( eTag_NewSubfileType );
	AddTagReader( eTag_ImageWidth );
	AddTagReader( eTag_ImageLength );
	AddTagReader( eTag_BitsPerSample );
	AddTagReader( eTag_Compression );
	AddTagReader( eTag_PhotometricInterpretation );
	AddTagReader( eTag_ImageDescription );
	AddTagReader( eTag_StripOffsets );
	AddTagReader( eTag_Orientation );
	AddTagReader( eTag_SamplesPerPixel );
	AddTagReader( eTag_RowsPerStrip );
	AddTagReader( eTag_StripByteCounts );
	AddTagReader( eTag_DifferencingPredictor );
} // ExAPTIFFTagReaderMaster::ExAPTIFFTagReaderMaster()


void ExAPTIFFTagReaderMaster::AddTagReader( ExAPTIFFTagType t ) throw( ExAPException )
{
	m_readerObjectMap[t] = TagReaderFactory( t );
} // ExAPTIFFTagReaderMaster::AddTagReader()


ExAPAutoPtr<ExAPTIFFTagReader> ExAPTIFFTagReaderMaster::TagReaderFactory( ExAPTIFFTagType t ) throw( ExAPException )
{
	ExAPAutoPtr<ExAPTIFFTagReader> rtn;

	try
	{

		switch( t )
		{
			case eTag_NewSubfileType:
				rtn = new ExAPTIFFTagReader_NewSubfileType;
				break;

			case eTag_ImageWidth:
				rtn = new ExAPTIFFTagReader_ImageWidth;
				break;

			case eTag_ImageLength:
				rtn = new ExAPTIFFTagReader_ImageLength;
				break;

			case eTag_BitsPerSample:
				rtn = new ExAPTIFFTagReader_BitsPerSample;
				break;

			case eTag_Compression:
				rtn = new ExAPTIFFTagReader_CompressionType;
				break;

			case eTag_PhotometricInterpretation:
				rtn = new ExAPTIFFTagReader_PhotometricInterpretation;
				break;

			case eTag_ImageDescription:
				rtn = new ExAPTIFFTagReader_ImageDescription;
				break;

			case eTag_StripOffsets:
				rtn = new ExAPTIFFTagReader_StripOffsets;
				break;

			case eTag_Orientation:
				rtn = new ExAPTIFFTagReader_Orientation;
				break;

			case eTag_SamplesPerPixel:
				rtn = new ExAPTIFFTagReader_SamplesPerPixel;
				break;

			case eTag_RowsPerStrip:
				rtn = new ExAPTIFFTagReader_RowsPerStrip;
				break;

			case eTag_StripByteCounts:
				rtn = new ExAPTIFFTagReader_StripByteCounts;
				break;

			case eTag_DifferencingPredictor:
				rtn = new ExAPTIFFTagReader_DifferencingPredictor;
				break;

			case eTag_Invalid:
			case eTag_SubfileType:
			case eTag_Threshholding:
			case eTag_CellWidth:
			case eTag_CellLength:
			case eTag_FillOrder:
			case eTag_Make:
			case eTag_Model:
			case eTag_MinSampleValue:
			case eTag_MaxSampleValue:
			case eTag_XResolution:
			case eTag_YResolution:
			case eTag_PlanarConfiguration:
			case eTag_FreeOffsets:
			case eTag_FreeByteCounts:
			case eTag_GreyResponseUnit:
			case eTag_GreyResponseCurve:
			case eTag_ResolutionUnit:
			case eTag_Software:
			case eTag_DateTime:
			case eTag_Artist:
			case eTag_HostComputer:
			case eTag_ColourMap:
			case eTag_ExtraSamples:
			case eTag_Copyright:
			default:
				break;
		}
	}
	catch( ... )
	{
		// Do nothing; rtn remains zero.
	}

	if( rtn == 0 )
	{
		ThrowExAPException( ExAPException::ResourceAcquisitionFailed );
	}

	return( rtn );
} // ExAPTIFFTagReaderMaster::TagReaderFactory()


void ExAPTIFFTagReaderMaster::ReadTag(
	ExAPAutoPtr<ExUtilFileReaderBaseInterface> file,
	const IFDFieldType & field,
	ExAPTIFFImageMetadata & tiffImageMetadata ) const
{
#ifdef TAW_TRACE
	bool success = false;
#endif
	MapType::const_iterator it = m_readerObjectMap.find( static_cast<ExAPTIFFTagType>( field.m_tag ) );

	if( it != m_readerObjectMap.end() )
	{
		ExAPTIFFTagReader * tagReader = it->second;
		
		if( tagReader != 0 )
		{
			tagReader->ReadTag( file, field, tiffImageMetadata );
#ifdef TAW_TRACE
			success = true;
#endif
		}
	}

#ifdef TAW_TRACE
	if( !success )
	{
		printf( "**** Tag labeled '%hu' not read. ****\n", field.m_tag );
	}
#endif
} // ExAPTIFFTagReaderMaster::ReadTag()


// **** End of File ****
