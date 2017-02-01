/*
 *  ExAPTIFFTags.h
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Sun Jul 27 2003.
 *  Copyright (c) 2003 by Tom Weatherhead. All rights reserved.
 *
 */


#ifndef _EXAPTIFFTAGS_H_
#define _EXAPTIFFTAGS_H_		1


enum ExAPTIFFTagType
{
	eTag_Invalid = 0,
	eTag_NewSubfileType = 254,							// LONG
	eTag_SubfileType = 255,									// SHORT
	eTag_ImageWidth = 256,									// SHORT or LONG
	eTag_ImageLength = 257,									// SHORT or LONG
	eTag_BitsPerSample = 258,								// SHORT
	eTag_Compression = 259,									// SHORT
	eTag_PhotometricInterpretation = 262,		// SHORT
	eTag_Threshholding = 263,								// SHORT
	eTag_CellWidth = 264,										// SHORT
	eTag_CellLength = 265,									// SHORT
	eTag_FillOrder = 266,										// SHORT
	eTag_ImageDescription = 270,						// ASCII
	eTag_Make = 271,												// ASCII : The scanner manufacturor
	eTag_Model = 272,												// ASCII : The scanner model
	eTag_StripOffsets = 273,								// SHORT or LONG
	eTag_Orientation = 274,									// SHORT
	eTag_SamplesPerPixel = 277,							// SHORT
	eTag_RowsPerStrip = 278,								// SHORT or LONG
	eTag_StripByteCounts = 279,							// SHORT or LONG
	eTag_MinSampleValue = 280,							// SHORT
	eTag_MaxSampleValue = 281,							// SHORT
	eTag_XResolution = 282,									// RATIONAL
	eTag_YResolution = 283,									// RATIONAL
	eTag_PlanarConfiguration = 284,					// SHORT
	eTag_FreeOffsets = 288,									// LONG
	eTag_FreeByteCounts = 289,							// LONG
	eTag_GreyResponseUnit = 290,						// SHORT
	eTag_GreyResponseCurve = 291,						// SHORT
	eTag_ResolutionUnit = 296,							// SHORT
	eTag_Software = 305,										// ASCII
	eTag_DateTime = 306,										// ASCII
	eTag_Artist = 315,											// ASCII
	eTag_HostComputer = 316,								// ASCII
	eTag_DifferencingPredictor = 317,				// SHORT: 1 = None (Default); 2 = Horizontal differencing (LZW only)
	eTag_ColourMap = 320,										// SHORT (eg. White = (65535,65535,65535))
	eTag_ExtraSamples = 338,								// SHORT
	eTag_Copyright = 33432									// ASCII
}; // enum ExAPTIFFTagType


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

enum ExAPTIFFTagDataType
{
	eTagDataType_Byte = 1,
	eTagDataType_ASCII = 2,
	eTagDataType_Short = 3,
	eTagDataType_Long = 4,
	eTagDataType_Rational = 5,
	eTagDataType_SignedByte = 6,
	eTagDataType_Undefined = 7,
	eTagDataType_SignedShort = 8,
	eTagDataType_SignedLong = 9,
	eTagDataType_SignedRational = 10,
	eTagDataType_Float = 11,
	eTagDataType_Double = 12
}; // enum ExAPTIFFTagDataType


class IFDFieldType
{
private:
	IFDFieldType( void );

public:
	unsigned short m_tag;
	unsigned short m_fieldType;		// The type of data that constitute the field.
	unsigned int m_dataCount;			// The number of data in the field.
	unsigned int m_valueOffset;

	IFDFieldType( unsigned short tag, unsigned short fieldType, unsigned int dataCount, unsigned int valueOffset )
		: m_tag( tag ),
			m_fieldType( fieldType ),
			m_dataCount( dataCount ),
			m_valueOffset( valueOffset )
	{
	}
}; // class IFDFieldType


#endif // #ifndef _EXAPTIFFTAGS_H_


// **** End of File ****
