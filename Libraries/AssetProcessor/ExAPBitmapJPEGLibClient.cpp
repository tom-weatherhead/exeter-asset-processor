/*
 *  ExAPBitmap8bpch.cpp
 *  Exeter AssetProcessor
 *
 *  Created by Tom Weatherhead on Sat Jun 04 2005.
 *  Copyright (c) 2003-2005 by Tom Weatherhead. All rights reserved.
 *
 */


//#include <cstdio>	// For JPEG I/O
#include <memory>	// For memset()
//#include <cmath>

#include "UtilLib\\ExUtilFileWriterBaseInterface.h"
//#include "ExAPBitmap8bpch.h"
#include "ExAPBitmapGrey8.h"
#include "ExAPBitmapRGB24.h"
#include "ExAPBitmapCMYK32.h"
//#include "ExAPFile.h"

//lint -e526 : Function not defined.
//lint -e755 : Global macro not referenced.
//lint -e756 : Global typedef not referenced.
//lint -e757 : Global declarator not referenced.
//lint -e768 : Global struct member not referenced.
//lint -e769 : Global enumeration constant not referenced.

extern "C"
{
#include "..\\3rdParty\\JPEG6b\\jpeglib.h"
}


/******************** JPEG COMPRESSION SAMPLE INTERFACE *******************/

/* This half of the example shows how to feed data into the JPEG compressor.
 * We present a minimal version that does not worry about refinements such
 * as error recovery (the JPEG code will just exit() if it gets an error).
 */


/*
 * IMAGE DATA FORMATS:
 *
 * The standard input image format is a rectangular array of pixels, with
 * each pixel having the same number of "component" values (color channels).
 * Each pixel row is an array of JSAMPLEs (which typically are unsigned chars).
 * If you are working with color data, then the color values for each pixel
 * must be adjacent in the row; for example, R,G,B,R,G,B,R,G,B,... for 24-bit
 * RGB color.
 *
 * For this example, we'll assume that this data structure matches the way
 * our application has stored the image in memory, so we can just pass a
 * pointer to our image buffer.  In particular, let's say that the image is
 * RGB color and is described by:
 */


// Static function.

//ExAPBitmap8bpch * ExAPBitmap8bpch::ReadJPEG( FILE * fp ) throw( ExAPException )
ExAPBitmap8bpch * ExAPBitmap8bpch::ReadJPEG( ExAPAutoPtr<ExUtilFileReaderBaseInterface> pSrcFile ) throw( ExAPException )
{
	FILE * fp = pSrcFile->GetFilePointer();
	ExAPBitmap8bpch * newImage = 0;

  /* This struct contains the JPEG decompression parameters and pointers to
   * working space (which is allocated as needed by the JPEG library).
   */
  struct jpeg_decompress_struct cinfo;

  /* We use our private extension JPEG error handler.
   * Note that this struct must live as long as the main JPEG parameter
   * struct, to avoid dangling-pointer problems.
   */
  struct jpeg_error_mgr jerr;

  /* More stuff */
  FILE * infile = fp;		/* source file */
  JSAMPARRAY buffer;		/* Output row buffer */

  /* Step 1: allocate and initialize JPEG decompression object */

  /* We set up the normal JPEG error routines. */
  cinfo.err = jpeg_std_error( &jerr );

  /* Now we can initialize the JPEG decompression object. */
  jpeg_create_decompress( &cinfo );

  /* Step 2: specify data source (eg, a file) */

  jpeg_stdio_src( &cinfo, infile );

  /* Step 3: read file parameters with jpeg_read_header() */

  (void) jpeg_read_header( &cinfo, TRUE );
  /* We can ignore the return value from jpeg_read_header since
   *   (a) suspension is not possible with the stdio data source, and
   *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
   * See libjpeg.doc for more info.
   */

  /* Step 4: set parameters for decompression */

  /* In this example, we don't need to change any of the defaults set by
   * jpeg_read_header(), so we do nothing here.
   */

  /* Step 5: Start decompressor */

  (void) jpeg_start_decompress( &cinfo );
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  /* We may need to do some setup of our own at this point before reading
   * the data.  After jpeg_start_decompress() we have the correct scaled
   * output image dimensions available, as well as the output colormap
   * if we asked for color quantization.
   * In this example, we need to make an output work buffer of the right size.
   */

	try
	{
		const int newImageWidth = int( cinfo.output_width );
		const int newImageHeight = int( cinfo.output_height );

		switch( cinfo.out_color_space )
		{
			case JCS_GRAYSCALE:
				newImage = new ExAPBitmapGrey8( newImageWidth, newImageHeight );
				break;

			case JCS_RGB:
				newImage = new ExAPBitmapRGB24( newImageWidth, newImageHeight );
				break;

			case JCS_CMYK:
				newImage = new ExAPBitmapCMYK32( newImageWidth, newImageHeight );
				break;

			case JCS_UNKNOWN:
			case JCS_YCbCr:
			case JCS_YCCK:
			default:
				ThrowExAPException( ExAPException::UnsupportedColourModel );
		}
	}
	catch( ExAPException & )
	{
		throw;
	}
	catch( ... )
	{
	}

	if( newImage == 0 )
	{
		ThrowExAPException( ExAPException::ResourceAcquisitionFailed );
	}

	const int row_stride = newImage->m_bytesPerLine;	/* physical row width in output buffer */

  /* Make a one-row-high sample array that will go away when done with image */
	//lint -e740 : Unusual pointer cast (incompatible indirect types)
  buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, (unsigned int)row_stride, 1);
	//lint +e740

  /* Step 6: while (scan lines remain to be read) */
  /*           jpeg_read_scanlines(...); */

  /* Here we use the library's state variable cinfo.output_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   */

  while( cinfo.output_scanline < cinfo.output_height )
	{
    /* jpeg_read_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could ask for
     * more than one scanline at a time if that's more convenient.
     */
    (void) jpeg_read_scanlines( &cinfo, buffer, 1 );

		if( /* cinfo.output_scanline >= 0  && */ cinfo.output_scanline < cinfo.output_height )
		{
			memcpy( &newImage->m_buffer[cinfo.output_scanline * (unsigned int)row_stride], buffer[0], (unsigned int)row_stride );
		}
  }

	if( cinfo.out_color_space == JCS_CMYK )
	{
		newImage->ComplementImageData();
	}

  /* Step 7: Finish decompression */

  (void) jpeg_finish_decompress( &cinfo );
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  /* Step 8: Release JPEG decompression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_decompress( &cinfo );

  /* At this point you may want to check to see whether any corrupt-data
   * warnings occurred (test whether jerr.pub.num_warnings is nonzero).
   */

  /* And we're done! */
	return( newImage );
} // ExAPBitmap8bpch::ReadJPEG()


/*
 * Sample routine for JPEG compression.  We assume that the target file name
 * and a compression quality factor are passed in.
 */

void ExAPBitmap8bpch::WriteJPEG( const char * filename, int quality ) const throw( ExAPException )
{
	J_COLOR_SPACE in_color_space = JCS_UNKNOWN;

	switch( m_colourModel.GetModelType() )
	{
		case ExAPColourModel::Grey8:
			in_color_space = JCS_GRAYSCALE; 	/* colorspace of input image */
			break;

		case ExAPColourModel::RGB24:
			in_color_space = JCS_RGB; 				/* colorspace of input image */
			break;

		case ExAPColourModel::CMYK32:
			in_color_space = JCS_CMYK; 				/* colorspace of input image */
			break;

		case ExAPColourModel::Paletted:
		case ExAPColourModel::RGBA32:
		case ExAPColourModel::RGBA64:
		default:
			ThrowExAPException( ExAPException::UnsupportedColourModel );
	}

	//const ExAPFile file( filename, "wb" );
  //FILE * outfile = file.GetFilePointer();
	const std::string strFilename( filename );
	ExAPAutoPtr<ExUtilFileWriterBaseInterface> pDstFile = ExUtilFileWriterBaseInterface::StdioCreate( strFilename );
  FILE * outfile = pDstFile->GetFilePointer();

  /* This struct contains the JPEG compression parameters and pointers to
   * working space (which is allocated as needed by the JPEG library).
   * It is possible to have several such structures, representing multiple
   * compression/decompression processes, in existence at once.  We refer
   * to any one struct (and its associated working data) as a "JPEG object".
   */
  struct jpeg_compress_struct cinfo;

  /* This struct represents a JPEG error handler.  It is declared separately
   * because applications often want to supply a specialized error handler
   * (see the second half of this file for an example).  But here we just
   * take the easy way out and use the standard error handler, which will
   * print a message on stderr and call exit() if compression fails.
   * Note that this struct must live as long as the main JPEG parameter
   * struct, to avoid dangling-pointer problems.
   */
  struct jpeg_error_mgr jerr;

  /* More stuff */
  JSAMPROW row_pointer[1];									/* pointer to JSAMPLE row[s] */
  const int row_stride = m_bytesPerLine;		/* physical row width in image buffer */

  /* Step 1: allocate and initialize JPEG compression object */

  /* We have to set up the error handler first, in case the initialization
   * step fails.  (Unlikely, but it could happen if you are out of memory.)
   * This routine fills in the contents of struct jerr, and returns jerr's
   * address which we place into the link field in cinfo.
   */
  cinfo.err = jpeg_std_error( &jerr );

  /* Now we can initialize the JPEG compression object. */
  jpeg_create_compress( &cinfo );

  /* Step 2: specify data destination (eg, a file) */
  /* Note: steps 2 and 3 can be done in either order. */

  /* Here we use the library-supplied code to send compressed data to a
   * stdio stream.  You can also write your own code to do something else.
   */
  jpeg_stdio_dest( &cinfo, outfile );

  /* Step 3: set parameters for compression */

  /* First we supply a description of the input image.
   * Four fields of the cinfo struct must be filled in:
   */
  cinfo.image_width = (unsigned int)m_width;							/* image width and height, in pixels */
  cinfo.image_height = (unsigned int)m_height;
  cinfo.input_components = m_bitsPerPixel / 8;	/* # of color components per pixel */
  cinfo.in_color_space = in_color_space;		/* colorspace of input image */

  /* Now use the library's routine to set default compression parameters.
   * (You must set at least cinfo.in_color_space before calling this,
   * since the defaults depend on the source color space.)
   */
  jpeg_set_defaults( &cinfo );

  /* Now you can set any non-default parameters you wish to.
   * Here we just illustrate the use of quality (quantization table) scaling:
   */
  jpeg_set_quality( &cinfo, quality, TRUE /* limit to baseline-JPEG values */ );

  /* Step 4: Start compressor */

  /* TRUE ensures that we will write a complete interchange-JPEG file.
   * Pass TRUE unless you are very sure of what you're doing.
   */
  jpeg_start_compress( &cinfo, TRUE );

  /* Step 5: while (scan lines remain to be written) */
  /*           jpeg_write_scanlines(...); */

  /* Here we use the library's state variable cinfo.next_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   * To keep things simple, we pass one scanline per call; you can pass
   * more if you wish, though.
   */
  //row_stride = m_bytesPerLine;	//image_width * 3;	/* JSAMPLEs per row in image_buffer */

	// We can't use an AutoArray here; rowBuffer is automatically freed.
	unsigned char * rowBuffer = 0;
	
	try
	{
		rowBuffer = new unsigned char[size_t(row_stride)];
	}
	catch( ... )
	{
		// Do nothing.
	}

	if( rowBuffer == 0 )
	{
		ThrowExAPException( ExAPException::ResourceAcquisitionFailed );
	}

	row_pointer[0] = rowBuffer;

	const bool imageIsCMYK = in_color_space == JCS_CMYK;

  while( cinfo.next_scanline < cinfo.image_height )
	{
    /* jpeg_write_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could pass
     * more than one scanline at a time if that's more convenient.
     */

		memcpy( rowBuffer, &m_buffer[ cinfo.next_scanline * (unsigned int)row_stride], (unsigned int)row_stride );

		if( imageIsCMYK )
		{

			for( int i = 0; i < row_stride; ++i )
			{
				rowBuffer[i] = ~rowBuffer[i];
			}
		}

    (void) jpeg_write_scanlines( &cinfo, row_pointer, 1 );
  }

  /* Step 6: Finish compression */

  jpeg_finish_compress( &cinfo );

  /* Step 7: release JPEG compression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_compress( &cinfo );

  /* And we're done! */
} // ExAPBitmap8bpch::WriteJPEG()


// End of File
