/**********************************************************/
/**
 * 234Compositor - Image data merging library
 *
 * Copyright (c) 2013-2015 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 **/
/**********************************************************/

// @file   test_234float.c

// @brief  Test program for 234Compositor
//         Offline 234Compositor for RGBA32 Images
//         mpicc -o test_234float test_234float.c -lm lib234comp.a -ljpeg

// @note Requirement: libjpeg

// @author Jorji Nonaka (jorji@riken.jp)


#include "234compositor.h"

#include <assert.h>
#include <stdio.h>  
#include <jpeglib.h>

int write_jpeg_image ( char*, float*, unsigned int, unsigned int, unsigned int );

// MPI Library
#ifndef _MPI_INCLUDE
	#include <mpi.h>
	#define _MPI_INCLUDE
#endif

/*=========================================*/
/**
 *  @brief Main Program
 */
/*=========================================*/
int main( int argc, char* argv[] )
{
	int rank;
	int nnodes;

	unsigned int width, height, image_size;

	float* rgba_image;
	char*  image_filename;
	char*  node_num;
	
	FILE* in_fp;

	//=====================================
	if ( argc == 2 ) {
		width  = atoi(argv[1]);
		height = width;
	}
	else if ( argc == 3 ) {
		width  = atoi(argv[1]);
		height = atoi(argv[2]);
	}
	else { 
		printf ("\n Usage: %s Width Height \n\n", argv[0] );
		exit( EXIT_FAILURE );
	}		

	//=====================================
	// Initialize MPI
	//=====================================
	MPI_Init(&argc, &argv);
	MPI_Comm_rank( MPI_COMM_WORLD, &rank   );
	MPI_Comm_size( MPI_COMM_WORLD, &nnodes );

	global_my_rank = rank;
	global_nnodes = nnodes;

	//=====================
	// IMAGE TYPE
	pixel_ID = ID_RGBA128;
	//=====================

	//=====================================
	// Read Image Files
	//=====================================
	image_size = width * height;

    rgba_image = allocate_float_memory_region( image_size * RGBA);
    assert(rgba_image); 

    image_filename = (char *)allocate_byte_memory_region( 255 );
	assert(image_filename);

    node_num = (char *)allocate_byte_memory_region( 255 );
	assert(node_num);

	//=====================================
	// 			Read Image File
	//=====================================
	image_filename[0] = '\0';		
	
	if ( rank < 10 ) {
		image_filename = strcat ( image_filename, "./input_images/000" );
	}
	else if (( rank > 9 ) && ( rank < 100 )) 
	{
		image_filename = strcat ( image_filename, "./input_images/00" );
	}
	else if (( rank > 99 ) && ( rank < 10000 )) 
	{
		image_filename = strcat ( image_filename, "./input_images/0" );
	}

	sprintf( node_num, "%d", rank );

	image_filename = strcat ( image_filename, node_num );
	image_filename = strcat ( image_filename, ".rgba128\0" );

	if ( (in_fp = fopen( image_filename, "rb")) == NULL ) 
	{
		printf( "<<< ERROR >>> Cannot open \"%s\" for reading \n", \
			    image_filename );
		exit ( EXIT_FAILURE );
	}

	if ( fread( rgba_image, sizeof(float), image_size * RGBA, in_fp ) != image_size * RGBA ) 
	{
		printf("<<< ERROR >>> Image reading error \n" );
		exit ( EXIT_FAILURE );
	}

	//=====================================
	// Execute 234Compositor
	//=====================================
	Init_234Composition ( rank, nnodes, width, height, pixel_ID );
    Do_234Composition   ( rank, nnodes, width, height, pixel_ID, ALPHA_BtoF, rgba_image, MPI_COMM_WORLD );
	Destroy_234Composition ( pixel_ID );
	//=====================================

	if ( rank == 0 ) {
		sprintf( image_filename, "output.jpeg" );
		write_jpeg_image( image_filename, rgba_image, width, height, RGBA128 );
	}

	MPI_Finalize();
	return ( EXIT_SUCCESS );
}

/*===========================================================================*/
/**
 *  @brief Write output RGBA image. 
 *
 *  @param  rgba_filename [in] Filename 
 *  @param  image         [in] Image to be written
 *  @param  width         [in]  Image width
 *  @param  height        [in]  Image height
 *  @param  pixel_size    [in]  Pixel size
 */
/*===========================================================================*/
int write_rgbaf_image \
		( char*  rgba_filename, \
		  float* image, \
		  unsigned int  width, \
		  unsigned int  height, \
		  unsigned int  pixel_size )
{

	unsigned int image_size;
	FILE* out_fp;
	
	if ( (out_fp = fopen( rgba_filename, "wb")) == NULL ) 
	{
		printf( "<<< ERROR >>> Cannot open \"%s\" for writing \n", \
			    rgba_filename );
		return EXIT_FAILURE;
	}

	image_size = width * height * pixel_size;

	if ( fwrite( image, image_size, 1, out_fp ) != 1 ) 
	{
		printf("<<< ERROR >>> Cannot write image \n" );
		return EXIT_FAILURE;
	}

	fclose( out_fp );
	return EXIT_SUCCESS;
}

/*===========================================================================*/
/**
 *  @brief Write output JPEG image. 
 *
 *  @param  jpeg_filename [in] Filename 
 *  @param  image         [in] Image to be written
 *  @param  width         [in]  Image width
 *  @param  height        [in]  Image height
 *  @param  pixel_size    [in]  Pixel size
 */
/*===========================================================================*/
int write_jpeg_image \
		( char*  jpeg_filename, \
		  float* image, \
		  unsigned int  width, \
		  unsigned int  height, \
		  unsigned int  pixel_size )
{
	FILE* out_fp;
	int i, j;

	float* float_image_ptr;
	JSAMPARRAY jpeg_image;

	// JPEG Object
	// Error Handlers
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
 
	
	// Default value for the Error Handler
    cinfo.err = jpeg_std_error( &jerr );
 
	// Initialization of JPEG Object
    jpeg_create_compress( &cinfo );
 
	if ( (out_fp = fopen( jpeg_filename, "wb")) == NULL ) 
	{
		fprintf( stderr, "<<< ERROR >>> Cannot open \"%s\" for writing \n", \
			     jpeg_filename );
		exit( EXIT_FAILURE );
	}

    jpeg_stdio_dest( &cinfo, out_fp );
 
    cinfo.image_width      = width;
    cinfo.image_height     = height;
    cinfo.input_components = 3;
    cinfo.in_color_space   = JCS_RGB;
    jpeg_set_defaults( &cinfo );
    jpeg_set_quality( &cinfo, 100, TRUE );
 
	// Start Image Compression
    jpeg_start_compress( &cinfo, TRUE );
 
	// Set Image Data
	float_image_ptr = image;

    jpeg_image = (JSAMPARRAY)malloc( sizeof(JSAMPROW) * cinfo.image_height );
    for ( i = 0; i < cinfo.image_height; i++ ) {
        jpeg_image[ i ] = (JSAMPROW)malloc( sizeof(JSAMPLE) * cinfo.image_width * cinfo.input_components );
        for ( j = 0; j < cinfo.image_width; j++ ) {
            jpeg_image[ i ][ ( j * 3 ) + 0] = (int)( *float_image_ptr++ * 255 );
            jpeg_image[ i ][ ( j * 3 ) + 1] = (int)( *float_image_ptr++ * 255 );
            jpeg_image[ i ][ ( j * 3 ) + 2] = (int)( *float_image_ptr++ * 255 );
			float_image_ptr++;
        }
    }

	// Write Image Data
    jpeg_write_scanlines( &cinfo, jpeg_image, cinfo.image_height );
 
	// End Image Compression
    jpeg_finish_compress( &cinfo );
 
	// Destroy JPEG Object
    jpeg_destroy_compress( &cinfo );
 
    for ( i = 0; i < cinfo.image_height; i++ ) {
        free( jpeg_image[ i ]);
    }

    free( jpeg_image );
    fclose( out_fp );

	return ( EXIT_SUCCESS );
}


