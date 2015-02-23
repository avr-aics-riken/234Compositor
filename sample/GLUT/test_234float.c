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

// MPI Library
#ifndef _MPI_INCLUDE
	#include <mpi.h>
	#define _MPI_INCLUDE
#endif

int write_rgbaf_image ( char*, float*, unsigned int, unsigned int , unsigned int );

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
		sprintf( image_filename, "output_%dx%d.rgba128", (int)width, (int)height );
		write_rgbaf_image( image_filename, rgba_image, width, height, RGBA128 );
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


