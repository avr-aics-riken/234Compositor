/**********************************************************/
/**
 * 234Compositor - Image data merging library
 *
 * Copyright (c) 2013-2015 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 **/
/**********************************************************/

// @file   234compositor.h
// @brief  Main routines for 234Compositor
//          2-3-4 Decomposition + Binary-Swap
//          MPI_Gather using MPI Rank in bit-reversed order
// @author Jorji Nonaka (jorji@riken.jp)

#ifndef COMPOSITOR234_H_INCLUDE
#define COMPOSITOR234_H_INCLUDE
#endif

// ======================================
//			HEADER FILES
// ======================================

#include <stdio.h>   // printf
#include <string.h>  // malloc, memset, strcpy, strcat, strrchr
#include <math.h>    // ceil, M_LOG2E
#include <stdio.h>   // printf, fprintf, sprintf, fwrite, FILE
#include <stdlib.h>  // atoi, free 
#ifndef HAVE_STDBOOL_H
#include <stdbool.h> // true, false
#endif

// C99 Compiler
#if (__STDC_VERSION__ >= 199901L)
	#define C99
#endif

// MPI Library
#ifndef _MPI_INCLUDE
	#include <mpi.h>
#endif

// OpenMP Library
#ifdef _OPENMP
   #include <omp.h>
#else
   #define omp_get_thread_num() 0
#endif


// ======================================
//		    TYPEDEF DECLARATIONS
// ======================================

typedef unsigned char BYTE;	// Unsigned Char

// ======================================
//	    CONSTANTS (Image Data ) 
// ======================================

#define MPIXELS_4	  4194304 	//   4096 x 1024 equivalent (  4 Mega Pixels)
#define MPIXELS_8	  8388608 	//   8192 x 1024 equivalent (  8 Mega Pixels)
#define MPIXELS_16	 16777216 	//  16384 x 1024 equivalent ( 16 Mega Pixels)
#define MPIXELS_32	 33554432 	//  32768 x 1024 equivalent ( 32 Mega Pixels)
#define MPIXELS_64 	 67108864 	//  65536 X 1024 equivalent ( 64 Mega Pixels)
#define MPIXELS_128	134217728 	// 131072 X 1024 equivalent (128 Mega Pixels)

#define ID_RGBA32		0	// ID for RGBA   32-bit	
#define ID_RGBAZ64		1	// ID for RGBAZ  64-bit	
#define ID_RGBA128		2	// ID for RGBA  128-bit	
#define ID_RGBAZ160		3	// ID for RGBAZ 160-bit	
#define ID_RGBA56		4	// ID for RGBA   56-bit	
#define ID_RGBAZ88		5	// ID for RGBAZ  88-bit	
#define ID_RGBA64		6	// ID for RGBA   64-bit	
#define ID_RGBAZ96		7	// ID for RGBAZ  96-bit	

#define RGBA32			4	//  4 Bytes ( Byte  RGBA : 4 * 1 Byte  )
#define RGBAZ64			8	//  8 Bytes ( Byte  RGBA Float Z: 4 * 1 Byte + 1 * 4 Bytes )
#define RGBA128			16	// 16 Bytes ( Float RGBA : 4 * 4 Bytes )
#define RGBAZ160		20	// 20 Bytes ( Float RGBAZ: 5 * 4 Bytes )
#define RGBA56			7	//  7 Bytes ( Byte  RGB   Float A : 3 * 1 Byte + 1 * 4 Bytes )
#define RGBAZ88			11	// 11 Bytes ( Byte  RGB   Float AZ: 3 * 1 Byte + 2 * 4 Bytes )
#define RGBA64			8	//  8 Bytes ( Byte  RGBX  Float A : 4 * 1 Byte + 1 * 4 Bytes )
#define RGBAZ96			12	// 12 Bytes ( Byte  RGBX  Float AZ: 4 * 1 Byte + 2 * 4 Bytes )

#define RGB			3	// 3 Components ( R, G and B )
#define RGBA			4	// 4 Components ( R, G, B and A )
#define RGBAZ			5	// 5 Components ( R, G, B, A and Z )
#define RGBA2			8	// Equivalent to 2 RGBA ( 4-byte RGBA + 4-byte Z )

#define LEFT  			0	// Node on the left side  
#define RIGHT 			1	// Node on the right side

#define ALPHA_BtoF	        -1	// Alpha-blending (OLD)
#define ALPHA		 	0	// Alpha-blending
#define DEPTH         		1	// Z-depth Sorting
#define ALPHA_ROI	  	2	// Alpha-blending with ROI 
#define DEPTH_ROI      		3	// Z-depth Sorting with ROI
#define ALPHA_COMPRESS		4	// Alpha-blending with COMPRESSION
#define DEPTH_COMPRESS		5	// Z-depth Sorting with COMPRESSION
// Other Pixel Merging Modes

// ======================================
//	    VARIABLES (Image Data ) 
// ======================================

BYTE *temp_image_rgba32;		// Temporary Image Data (RGBA32 Pixels)
BYTE *temp_image_rgba32_ptr;	// Pointer for the Temporary Image Data (RGBA32 Pixels)

BYTE *temp_image_rgba56;		// Temporary Image Data (RGBA56 Pixels)
BYTE *temp_image_rgba56_ptr;	// Pointer for the Temporary Image Data (RGBA56 Pixels)

BYTE *temp_image_rgba64;		// Temporary Image Data (RGBA64 Pixels)
BYTE *temp_image_rgba64_ptr;	// Pointer for the Temporary Image Data (RGBA64 Pixels)

BYTE *temp_image_rgbaz64;		// Temporary Image Data (RGBAZ64 Pixels)
BYTE *temp_image_rgbaz64_ptr;	// Pointer for the Temporary Image Data (RGBAZ64 Pixels)

BYTE *temp_image_rgbaz88;		// Temporary Image Data (RGBAZ88 Pixels)
BYTE *temp_image_rgbaz88_ptr;	// Pointer for the Temporary Image Data (RGBAZ88 Pixels)

BYTE *temp_image_rgbaz96;		// Temporary Image Data (RGBAZ96 Pixels)
BYTE *temp_image_rgbaz96_ptr;	// Pointer for the Temporary Image Data (RGBAZ96 Pixels)

BYTE *temp_image_byte_ptr;		// Pointer for the Temporary Image Data (BYTE)

float *temp_image_rgba128;		// Temporary Image Data (RGBA128 Pixels)
float *temp_image_rgba128_ptr;	// Pointer for the Temporary Image Data (RGBA128 Pixels)

float *temp_image_rgbaz160;		// Temporary Image Data (RGBAZ160 Pixels)
float *temp_image_rgbaz160_ptr;	// Pointer for the Temporary Image Data (RGBAZ160 Pixels)

float *temp_image_float_ptr;	// Pointer for the Temporary Image Data (FLOAT) 

unsigned int global_width;		// Image width
unsigned int global_height;		// Image height

unsigned int global_num_pixels;	// Number of pixels ( Image size )
unsigned int global_image_size;	// image size ( Number of pixels * image_type )

unsigned int global_mod_pixels; 	// Remainder pixels (Number of pixels mod Number of nodes)
unsigned int global_add_pixels; 	// Added pixels to complete a divisible number of pixels

unsigned int global_image_type;	// Image type ( RGBA32, RGBAZ_64, RGBA128, RGBAZ160 )
unsigned int pixel_ID;		// pixel ID (ID_RGBA32, ID_RGBAZ64, ID_RGBA128, ID_RGBAZ160)

// ======================================
// 	 		CONSTANTS (MPI related)
// ======================================

#define ROOT_NODE	0  	// Master Node's MPI Rank 

#define PAIR_TAG	100	// MPI Communicator Tag
#define SEND_TAG	101	// MPI Communicator Tag
#define RECV_TAG	102	// MPI Communicator Tag

// 3-Node Composition
#define PAIR_02_TAG	103 // MPI Communicator Tag ( Nodes 0 and 2 )
#define PAIR_12_TAG	104 // MPI Communicator Tag ( Nodes 1 and 2 )
#define PAIR_13_TAG	105 // MPI Communicator Tag ( Nodes 1 and 3 )

// ======================================
//	    VARIABLES ( MPI related ) 
// ======================================

MPI_Status  global_status;	// Status object for MPI_Recv
MPI_Request global_isend;	// ISend request parameter
MPI_Request global_irecv;	// IRecv request parameter

_Bool is_power_of_two; 	// Check wether is power-of-two (2^n)

// ======================================
//	    TRADITIONAL GATHERV 
// Final image gathering (MPI_Gatherv)
// ======================================

unsigned int bs_counts;		// Data counts information for MPI_Gatherv				
unsigned int bs_offset;		// Offset information for MPI_Gatherv

int *bs_gatherv_offset;		// List of offset data for MPI_Gatherv
int *bs_gatherv_counts;		// List of data counts information for MPI_Gatherv 
int *bs_gatherv_counts_offset;	// List of data counts information for MPI_Gatherv 

int *bs_gatherv_offset_ptr;		// Pointer for bs_gatherv_offset
int *bs_gatherv_counts_ptr;		// Pointer for bs_gatherv_counts
int *bs_gatherv_counts_offset_ptr;	// Pointer for bs_gatherv_counts

// ======================================
//	    BIT-REVERSAL 
// Final image gathering (MPI_Gather)
// ======================================

MPI_Comm MPI_COMM_BITREV; 	// MPI Communicator (Bit-reversal rank order)

int bitrev_my_rank;		// My Rank (MPI_COMM_BITREV)
int bitrev_nnodes;		// Num Nodes (MPI_COMM_BITREV)

int bitrev_my_group;		// Group(Color) for MPI_Comm_split
//==========================
// Not even image size
//==========================
//unsigned int remaining_pixel_node;	// Node with excess pixel

BYTE* send_byte_pixel_ptr;	// Pointer for remaining pixels (BYTE) to be sent	
BYTE* recv_byte_pixel_ptr;	// Pointer to receive remaining pixels (BYTE) 

float* send_float_pixel_ptr;// Pointer for remaining pixels (float) to be sent	
float* recv_float_pixel_ptr;// Pointer to receive remaining pixels (float) 

// ======================================
//	    2-3-4 Decomposition
// ======================================

unsigned int near_pow2;	// Nearest power of two smaller than total number of nodes

unsigned int ngroups_234;	// Number of groups
unsigned int base_234;	// Base for 2-3-4 Decomposition
unsigned int over_234;	// Nodes over threshold
unsigned int threshold_234;	// Threshold for 2-3-4 Decomposition (3)

//==========================
MPI_Comm MPI_COMM_234; 	// MPI Communicator (Groups of 2, 3 or 4)

int my_rank_234;		// My Rank (Groups of 2, 3 or 4)
int nnodes_234;		// Num Nodes (Groups of 2, 3 or 4)

unsigned int my_group_234;	// Group(Color) for MPI_Comm_split
//==========================

// ======================================
//	    2nd stage Binary-Swap
// ======================================
int *group_bswap;		// List of nodes (2nd stage Binary-Swap) 
int *group_bswap_ptr;	// Pointer for group_bswap

MPI_Group MPI_GROUP_WORLD;		// Group (Entire nodes)
MPI_Group MPI_GROUP_STAGE2_BSWAP;	// Group (2nd stage Binary-Swap)

//==========================
MPI_Comm  MPI_COMM_STAGE2_BSWAP;	// MPI Communicator (2nd stage Binary-Swap)

int stage2_bswap_my_rank;		// My Rank (2nd stage Binary-Swap)
int stage2_bswap_nnodes; ;		// Num Nodes (2nd stage Binary-Swap)

MPI_Comm  MPI_COMM_STAGE2_BITREV;	// MPI Communicator (2nd stage Bit-Reversed Binary-Swap)

int stage2_bitrev_my_rank;		// My Rank (2nd stage Binary-Swap)
int stage2_bitrev_nnodes; ;		// Num Nodes (2nd stage Binary-Swap)
//==========================

// ======================================
//    BIT-REVERSAL  (for MPI_Gather)
// ======================================
MPI_Comm MPI_COMM_MSTEP1_BITREV; // MPI Communicator (Step 1 w/ Bit-reversal rank order)
MPI_Comm MPI_COMM_MSTEP2_BITREV; // MPI Communicator (Step 2 w/ Bit-reversal rank order)

int bitrev_mstep1_my_rank;		// My Rank   (MPI_COMM_MSTEP1_BITREV)
int bitrev_mstep1_nnodes;		// Num Nodes (MPI_COMM_MSTEP1_BITREV)

int bitrev_mstep2_my_rank;		// My Rank   (MPI_COMM_MSTEP2_BITREV)
int bitrev_mstep2_nnodes;		// Num Nodes (MPI_COMM_MSTEP2_BITREV)

int bitrev_mstep1_my_group;		// Group(Color) for MPI_Comm_split
int bitrev_mstep2_my_group;		// Group(Color) for MPI_Comm_split

// ======================================
//	234Composition API
// ======================================
int Init_234Composition  ( unsigned int, unsigned int, unsigned int, unsigned int, unsigned int ); 
				// my_rank, nnodes, width, height, pixel_ID 
// Do image composition
int  Do_234Composition  ( unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, void*, MPI_Comm ); 
				// my_rank, nnodes, width, height, pixel_ID, merge_ID, *my_image_byte, MPI_COMM 

void* Do_234Composition_Ptr ( unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, void*, MPI_Comm ); 
				// my_rank, nnodes, width, height, pixel_ID, merge_ID, *my_image_byte, MPI_COMM 	

int Destroy_234Composition ( unsigned int );
				// pixel_ID )

// Initialize variables and image buffer for 234 Image Compositing 
int Init_234Composition_FLOAT ( unsigned int, unsigned int, unsigned int, unsigned int, unsigned int ); 
				   // my_rank, nnodes, width, height, pixel_ID

//=====================================
// Initialize variables and image buffer for 234 Image Compositing 
int Init_234Composition_BYTE  ( unsigned int, unsigned int, unsigned int, unsigned int, unsigned int ); 
				   // my_rank, nnodes, width, height, pixel_ID 
// Do image composition

int  Do_234Composition_Core_BYTE  ( unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, BYTE*,  MPI_Comm );
					// my_rank, nnodes, width, height, pixel_ID, *my_image_byte,  MPI_COMM 	

int Destroy_234Composition_BYTE ( unsigned int );
					// pixel_ID )

// Initialize variables and image buffer for 234 Image Compositing 
int Init_234Composition_FLOAT ( unsigned int, unsigned int, unsigned int, unsigned int, unsigned int ); 
				   // my_rank, nnodes, width, height, pixel_ID
// Do image composition
int  Do_234Composition_Core_FLOAT ( unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, float*, MPI_Comm );
					// my_rank, nnodes, width, height, pixel_ID, *my_image_float,  MPI_COMM 	

int Destroy_234Composition_FLOAT ( unsigned int );
//=====================================

unsigned int global_my_rank;
unsigned int global_nnodes;

// ========= ALPHA BLENDING LOOK UP TABLE ========= //
#ifdef _LUTBLEND
BYTE LUT_Mult[ 256 * 256 ]; // Product Lookup Table: (255 - Alpha) * Color  
BYTE LUT_Sat [ 512 ];       // Saturation Lookup Table: 255 if Color > 255  
#endif
//=====================================

#ifndef COMPOSITOR234_MISC_H_INCLUDE
	#include "misc.h"
	#define COMPOSITOR234_MISC_H_INCLUDE
#endif

#ifndef COMPOSITOR234_EXCHANGE_H_INCLUDE
	#include "exchange.h"
	#define COMPOSITOR234_EXCHANGE_H_INCLUDE
#endif

#ifndef COMPOSITOR234_MERGE_H_INCLUDE
	#include "merge.h"
	#define COMPOSITOR234_MERGE_H_INCLUDE
#endif


