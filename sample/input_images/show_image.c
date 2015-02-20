/**********************************************************/
/**
 * 234Compositor - Image data merging library
 *
 * Copyright (c) 2013-2015 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 **/
/**********************************************************/

// @file   show_image.c

// @brief  Test program for 234Compositor
//         GLUT-Based Raw Image Viewer
//         cc -o show_image show_image.c -lglut -lGLU

// @author Jorji Nonaka (jorji@riken.jp)


#define KEY_ESC 27

// Black Transparent Background Image 
#define BLACK_TRANSPARENT 0x00000000  

#define PIXEL_32BIT   4
#define PIXEL_56BIT   7
#define PIXEL_64BIT   8	
#define PIXEL_88BIT  11	
#define PIXEL_96BIT  12	
#define PIXEL_128BIT 16
#define PIXEL_160BIT 20

// OpenGL Include Header
#include <GL/glut.h>  // -lglut

#include <stdbool.h> // true, false
#include <stdlib.h>  // exit
#include <string.h>  // malloc, memset, strcpy, strcat, strrchr
#include <stdio.h>   // printf, fprintf, sprintf, fwrite, FILE

// ====================================================================

typedef unsigned char BYTE;

// ====================================================================
//							GLOBAL VARIABLES
// ====================================================================

BYTE *input_byte_image,   *input_byte_image_ptr;   // Input (BYTE Pixel Image)
BYTE *display_byte_image, *display_byte_image_ptr; // Display (BYTE Pixel Image)

float *input_float_image,   *input_float_image_ptr;   // Input (FLOAT Pixel Image)
float *display_float_image, *display_float_image_ptr; // Display (FLOAT Pixel Image)

BYTE  *flip_byte_image;  // BYTE RGBA Image
float *flip_float_image;  // FLOAT RGBA Image

uint width; 	// Image width
uint height;	// Image height 

uint image_resolution;	// Image resolution (width * height)
uint image_type;		// Pixel Type (PIXEL_32BIT; PIXEL_64BIT; PIXEL_128BIT; PIXEL_160BIT )

uint input_image_size;		// Input Data ( Image resolution * Pixel Type )
uint display_image_size;	// Display Data : Input Data without Z Data

_Bool flip_image_flag;
_Bool swap_flag;

float float_val1, float_val2, float_val3, float_val4;
BYTE  byte_val1;

float *float_a_ptr;
float *float_az_ptr;

uint i;

// ====================================================================
//							FUNCTION PROTOTYPES
// ====================================================================
void 	Reshape_Window ( int width, int height );
void 	Keyboard_Press ( unsigned char key, int x, int y );
void 	Mouse_Click ( int button, int state, int x, int y );
void 	Display_BYTE ( void );
void 	Display_FLOAT ( void );
void 	Show_RGBA_Image_BYTE ( void );	
void 	Show_RGBA_Image_FLOAT ( void );	
void 	Flip_Image_BYTE ( void );
void 	Flip_Image_FLOAT ( void );

// ====================================================================
//							SWAP
// ====================================================================
float FloatSwap( float value )
{
  union
  {
    float f;
    unsigned char bit[4];
  } data1, data2;
 
  data1.f    = value;
  data2.bit[0] = data1.bit[3];
  data2.bit[1] = data1.bit[2];
  data2.bit[2] = data1.bit[1];
  data2.bit[3] = data1.bit[0];
  return data2.f;
}

// ====================================================================
//							MAIN PROGRAM
// ====================================================================
int main( int argc, char *argv[] )
{
	char *filename;
	FILE *fp;

	uint j;
	uint offset_up, offset_down;
	
	flip_image_flag = false;
	swap_flag = false;
	
	if ( argc == 5 )	
	{
		filename   = (char *)argv[1];
		width      = (uint)atoi(argv[2]);
		height     = (uint)atoi(argv[3]);
		image_type = (uint)atoi(argv[4]);
	}
	else if ( argc == 6 )	
	{
		filename   = (char *)argv[1];
		width      = (uint)atoi(argv[2]);
		height     = (uint)atoi(argv[3]);
		image_type = (uint)atoi(argv[4]);

		if ( atoi(argv[5]) == 1 ) // Swap Endian
			swap_flag = true;
	}
	else if ( argc == 7 )	
	{
		filename   = (char *)argv[1];
		width      = (uint)atoi(argv[2]);
		height     = (uint)atoi(argv[3]);
		image_type = (uint)atoi(argv[4]);

		if ( atoi(argv[5]) == 1 ) // Swap Endian
			swap_flag = true;

		if ( atoi(argv[6]) == 1 ) // Flip image
			flip_image_flag = true;
	}
	else {
		printf("\n Usage: %s Filename Width Height Image_Type [Swap_Endian (0 or 1) Flip_Image (0 or 1)]\n\n", argv[0]);
		printf("          Image Type: 0 ( PIXEL_32BIT  ) \n");
		printf("                      1 ( PIXEL_64BIT  ) \n");
		printf("                      2 ( PIXEL_128BIT ) \n");
		printf("                      3 ( PIXEL_160BIT ) \n");
		return ( EXIT_FAILURE );
	}
	
	// ====================================================================

	if ( ( image_type < 0 ) || ( image_type > 3 ) ){
		printf("\n Invalid Image Type \n");
		printf("          Image Type: 0 ( PIXEL_32BIT  ) \n");
		printf("                      1 ( PIXEL_64BIT  ) \n");
		printf("                      2 ( PIXEL_128BIT ) \n");
		printf("                      3 ( PIXEL_160BIT ) \n");
		return ( EXIT_FAILURE );
	}

	// ====================================================================

    if ( ( fp = fopen( filename, "rb")) == NULL ) {
		printf("<<< ERROR >>> Cannot open file: %s \n", filename );
		return 1;
    }

	image_resolution = width * height;

	// ====================================================================

	// PIXEL_32BIT ; PIXEL_56BIT ; PIXEL_64BIT ; PIXEL_88BIT 
	if (( image_type == 0 ) || ( image_type == 1 ))
	{
		if ( image_type == 0 ) { // PIXEL_32BIT
			input_image_size   = image_resolution * PIXEL_32BIT ;
		}
		else if ( image_type == 1 ) { // PIXEL_64BIT
			input_image_size   = image_resolution * PIXEL_64BIT ;
		}

		display_image_size = image_resolution * PIXEL_32BIT ;

		input_byte_image = (GLubyte *)malloc ( input_image_size * sizeof(GLubyte) );
		memset( input_byte_image, 0, input_image_size * sizeof(GLubyte) );

		display_byte_image = (GLubyte *)malloc ( display_image_size * sizeof(GLubyte) );
		memset( display_byte_image, 0, display_image_size * sizeof(GLubyte) );
	
		flip_byte_image = (GLubyte *)malloc ( display_image_size * sizeof(GLubyte) );
		memset( flip_byte_image, 0, display_image_size * sizeof(GLubyte) );

		if ( fread ( input_byte_image, input_image_size, 1, fp ) != 1 ) {
			perror("<<< ERROR >>> Cannot read image \n" );
			exit( EXIT_FAILURE );
		}
		
		if ( image_type == 0 ) { // PIXEL_32BIT

			if ( flip_image_flag == true ) 
			{
				// =======================================
				// 				FLIP IMAGE
				// =======================================
				for ( j = 0; j < height; j++ ) {
					for ( i = 0; i < width; i++ ) {
			
						offset_up   = ( j * width + i ) * 4; // 4 elements
						offset_down = (( height - 1 - j ) * width + i ) * 4; // 4 elements

						display_byte_image[ offset_up     ] = input_byte_image[ offset_down     ]; // R 
						display_byte_image[ offset_up + 1 ] = input_byte_image[ offset_down + 1 ]; // G
						display_byte_image[ offset_up + 2 ] = input_byte_image[ offset_down + 2 ]; // B
						display_byte_image[ offset_up + 3 ] = input_byte_image[ offset_down + 3 ]; // A
					}
				}
			}
			else {
				// =======================================
				// 				NORMAL IMAGE
				// =======================================

				for ( i = 0; i < ( image_resolution * 4 ); i += 4 ) {
					display_byte_image[ i     ] = (BYTE)input_byte_image[ i     ]; // R
					display_byte_image[ i + 1 ] = (BYTE)input_byte_image[ i + 1 ]; // G
					display_byte_image[ i + 2 ] = (BYTE)input_byte_image[ i + 2 ]; // B
					display_byte_image[ i + 3 ] = (BYTE)input_byte_image[ i + 3 ]; // A
				}
			}
		}
		else if ( image_type == 1 ) { // PIXEL_64BIT 

				// =======================================
				// 			Image Tye: 	RGBAZ64 
				// =======================================

				j = 0;
				for ( i = 0; i < ( image_resolution * 8 ); i += 8 ) {
					display_byte_image[ j     ] = (BYTE)input_byte_image[ i     ]; // R
					display_byte_image[ j + 1 ] = (BYTE)input_byte_image[ i + 1 ]; // G
					display_byte_image[ j + 2 ] = (BYTE)input_byte_image[ i + 2 ]; // B
					display_byte_image[ j + 3 ] = (BYTE)input_byte_image[ i + 3 ]; // A
					j += 4;
				}

				if ( flip_image_flag == true ) 
				{
					// =======================================
					// 				FLIP IMAGE
					// =======================================
					memcpy ( input_byte_image, display_byte_image, display_image_size );

					for ( j = 0; j < height; j++ ) {
						for ( i = 0; i < width; i++ ) {
			
							offset_up   = ( j * width + i ) * 4; // 4 elements
							offset_down = (( height - 1 - j ) * width + i ) * 4; // 4 elements

							display_byte_image[ offset_up     ] = input_byte_image[ offset_down     ]; // R
							display_byte_image[ offset_up + 1 ] = input_byte_image[ offset_down + 1 ]; // G
							display_byte_image[ offset_up + 2 ] = input_byte_image[ offset_down + 2 ]; // B
							display_byte_image[ offset_up + 3 ] = input_byte_image[ offset_down + 3 ]; // A
						}
					}
				}
			}

			// ====================================================================
			glutInit ( &argc, argv );
			glutInitWindowPosition ( 0, 0 );
			glutInitWindowSize ( width, height );
			glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			glutInitDisplayMode ( GLUT_SINGLE | GLUT_RGBA | GLUT_ALPHA);
			glutCreateWindow ( "RGBA Image Viewer" );
	
			glutIdleFunc ( NULL );
			glutReshapeFunc ( Reshape_Window );  // Reshape call back 
			glutKeyboardFunc ( Keyboard_Press ); // Keyboard pressing call back 
			glutMouseFunc ( Mouse_Click ); 		 // Mouse click call back
			glutDisplayFunc ( Display_BYTE );

			glutMainLoop();
			// ====================================================================
		}
		else if (( image_type == 2 ) || ( image_type == 3 )) // PIXEL_128BIT of PIXEL_160BIT 
		{

			// ====================================================================
		
			if ( image_type == 2 ) { // PIXEL_128BIT
				input_image_size   = image_resolution * PIXEL_128BIT ;
			}
			else { // PIXEL_160BIT 
				input_image_size   = image_resolution * PIXEL_160BIT ;
			}

			display_image_size = image_resolution * PIXEL_128BIT ;

			input_float_image = (GLfloat *)malloc ( input_image_size * sizeof(GLfloat) );
			memset( input_float_image, 0, input_image_size * sizeof(GLfloat) );

			display_float_image = (GLfloat *)malloc ( display_image_size * sizeof(GLfloat) );
			memset( display_float_image, 0, display_image_size * sizeof(GLfloat) );
	
			flip_float_image = (GLfloat *)malloc ( display_image_size * sizeof(GLfloat) );
			memset( flip_float_image, 0, display_image_size * sizeof(GLfloat) );

			fread ( input_float_image, input_image_size, 1, fp );
		
			if ( image_type == 2 ) { // PIXEL_128BIT

				// =======================================
				// 			Image Tye: 	RGBA128 
				// =======================================

				if ( swap_flag == true ) 
				{
					// =======================================
					// 			BIG <-> LITTLE ENDIAN
					// =======================================

					if ( flip_image_flag == true ) 
					{
						// =======================================
						// 				FLIP IMAGE
						// =======================================
						for ( j = 0; j < height; j++ ) {
							for ( i = 0; i < width; i++ ) {
			
								offset_up   = ( j * width + i ) * 4; // 4 Eelements
								offset_down = (( height - 1 - j ) * width + i ) * 4; // 4 Eelements
		
								float_val1 = input_float_image[ offset_down     ];
								float_val2 = input_float_image[ offset_down + 1 ];
								float_val3 = input_float_image[ offset_down + 2 ];
								float_val4 = input_float_image[ offset_down + 3 ];

								float_val1 = FloatSwap( float_val1 );
								float_val2 = FloatSwap( float_val2 );
								float_val3 = FloatSwap( float_val3 );
								float_val4 = FloatSwap( float_val4 );

								display_float_image[ offset_up     ] = float_val1;
								display_float_image[ offset_up + 1 ] = float_val2;
								display_float_image[ offset_up + 2 ] = float_val3;
								display_float_image[ offset_up + 3 ] = float_val4;
							}
						}
					}
					else {

							// =======================================
							// 				NORMAL IMAGE
							// =======================================
							for ( i = 0; i < ( image_resolution * 4 ); i += 4 ) 
							{
								float_val1 = (float)input_float_image[ i     ];
								float_val2 = (float)input_float_image[ i + 1 ];
								float_val3 = (float)input_float_image[ i + 2 ];
								float_val4 = (float)input_float_image[ i + 3 ];

								float_val1 = FloatSwap( float_val1 );
								float_val2 = FloatSwap( float_val2 );
								float_val3 = FloatSwap( float_val3 );
								float_val4 = FloatSwap( float_val4 );

								display_float_image[ i     ] = (float)float_val1;
								display_float_image[ i + 1 ] = (float)float_val2;
								display_float_image[ i + 2 ] = (float)float_val3;
								display_float_image[ i + 3 ] = (float)float_val4;
							}		
					}
				}
				else // swap_flag == false  
				{

					if ( flip_image_flag == true ) 
					{
						// =======================================
						// 				FLIP IMAGE
						// =======================================
						for ( j = 0; j < height; j++ ) {
							for ( i = 0; i < width; i++ ) {
			
								offset_up   = ( j * width + i ) * 4; // 4 Eelements
								offset_down = (( height - 1 - j ) * width + i ) * 4;  // 4 Eelements
		
								display_float_image[ offset_up     ] = input_float_image[ offset_down     ];
								display_float_image[ offset_up + 1 ] = input_float_image[ offset_down + 1 ];
								display_float_image[ offset_up + 2 ] = input_float_image[ offset_down + 2 ];
								display_float_image[ offset_up + 3 ] = input_float_image[ offset_down + 3 ];
							}
						}
					}
					else {
							printf("PIXEL: R(%f) G(%f) B(%f) A(%f) \n", \
								input_float_image[0], input_float_image[1], input_float_image[2], input_float_image[3] );

							// =======================================
							// 				NORMAL IMAGE
							// =======================================
							for ( i = 0; i < ( image_resolution * 4 ); i += 4 ) {
								display_float_image[ i     ] = input_float_image[ i     ];
								display_float_image[ i + 1 ] = input_float_image[ i + 1 ];
								display_float_image[ i + 2 ] = input_float_image[ i + 2 ];
								display_float_image[ i + 3 ] = input_float_image[ i + 3 ];
							}

					}
				}	

			}
			else if ( image_type == 3 ) { // PIXEL_160BIT 

				input_float_image_ptr   = (float *)input_float_image;
				display_float_image_ptr = (float *)display_float_image;

				// =======================================
				// 			Image Tye: 	RGBAZ160 
				// =======================================
		
				if ( swap_flag == true ) 
				{
					// =======================================
					// 			BIG <-> LITTLE ENDIAN
					// =======================================
					j = 0;
					for ( i = 0; i < input_image_size; i += 5 ) {

						float_val1 = (float)input_float_image_ptr[ i     ];
						float_val2 = (float)input_float_image_ptr[ i + 1 ];
						float_val3 = (float)input_float_image_ptr[ i + 2 ];
						float_val4 = (float)input_float_image_ptr[ i + 3 ];

						float_val1 = FloatSwap( float_val1 );
						float_val2 = FloatSwap( float_val2 );
						float_val3 = FloatSwap( float_val3 );
						float_val4 = FloatSwap( float_val4 );

						// display_float_image_ptr: RGBA 4 elements
						// input_float_image_ptr  : RGBAZ 5 elements
						display_float_image_ptr[ j     ] = (float)float_val1;
						display_float_image_ptr[ j + 1 ] = (float)float_val2;
						display_float_image_ptr[ j + 2 ] = (float)float_val3;
						display_float_image_ptr[ j + 3 ] = (float)float_val4;
						j += 4;
					}
				}
				else 
				{
					// =======================================
					// 			Image Tye: 	RGBAZ160 
					// =======================================

					j = 0;
					for ( i = 0; i < input_image_size; i += 5 ) {

						// display_float_image_ptr: RGBA 4 elements
						// input_float_image_ptr  : RGBAZ 5 elements
						display_float_image_ptr[ j     ] = (float)input_float_image_ptr[ i     ];
						display_float_image_ptr[ j + 1 ] = (float)input_float_image_ptr[ i + 1 ];
						display_float_image_ptr[ j + 2 ] = (float)input_float_image_ptr[ i + 2 ];
						display_float_image_ptr[ j + 3 ] = (float)input_float_image_ptr[ i + 3 ];
						j += 4;
					}
				}

				// =======================================
				// 					FLIP IMAGE
				// =======================================
				if ( flip_image_flag == true ) 
				{
					memcpy ( input_float_image, display_float_image, display_image_size );

					for ( j = 0; j < height; j++ ) {
						for ( i = 0; i < width; i++ ) {
			
							offset_up   = ( j * width + i ) * 4; // 4 Eelements
							offset_down = (( height - 1 - j ) * width + i ) * 4; // 4 Eelements

							display_float_image[ offset_up     ] = input_float_image[ offset_down     ];
							display_float_image[ offset_up + 1 ] = input_float_image[ offset_down + 1 ];
							display_float_image[ offset_up + 2 ] = input_float_image[ offset_down + 2 ];
							display_float_image[ offset_up + 3 ] = input_float_image[ offset_down + 3 ];
						}
					}
				}
			}
	
			// ====================================================================
			glutInit ( &argc, argv );
			glutInitWindowPosition ( 0, 0 );
			glutInitWindowSize ( width, height );
			glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			glutInitDisplayMode ( GLUT_SINGLE | GLUT_RGBA | GLUT_ALPHA);
			glutCreateWindow ( "RGBA FLOAT Image Viewer" );
	
			glutIdleFunc ( NULL );
			glutReshapeFunc ( Reshape_Window );  // Reshape call back 
			glutKeyboardFunc ( Keyboard_Press ); // Keyboard pressing call back 
			glutMouseFunc ( Mouse_Click ); 		 // Mouse click call back
			glutDisplayFunc ( Display_FLOAT );
		
			glutMainLoop();
		}

// ===========================================================

	if ( input_byte_image )
		free( input_byte_image );
	if ( input_float_image )
		free( input_float_image );

	if ( display_byte_image )
		free( display_byte_image );
	if ( display_float_image )
		free( display_float_image );

	fclose(fp);

	return ( 0 );
}

// ===========================================================
void Display_BYTE ( void )
{
	glClear ( GL_COLOR_BUFFER_BIT );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glDrawPixels( width, height, GL_RGBA, GL_UNSIGNED_BYTE, display_byte_image );			
	glFlush();

}

// ===========================================================
void Display_FLOAT ( void )
{
	glClear ( GL_COLOR_BUFFER_BIT );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glDrawPixels( width, height, GL_RGBA, GL_FLOAT, display_float_image );	
	glFlush();

}

// ===========================================================
void Reshape_Window ( int width, int height )
{
	glViewport ( 0, 0, (GLsizei) width, (GLsizei) height );
	glMatrixMode ( GL_PROJECTION );
	glLoadIdentity();
	if ( width <= height )
		gluOrtho2D ( 0.0, 1.0, 0.0, 1.0 * (GLfloat) height / (GLfloat) width );
	else
		gluOrtho2D ( 0.0, 1.0 * (GLfloat) height / (GLfloat) width, 0.0, 1.0  );
}

// ===========================================================
void Keyboard_Press ( unsigned char key, int x, int y )
{
	switch ( key ) {
		case KEY_ESC : // ESCAPE Key
			exit ( EXIT_SUCCESS );
			break;
		case 'q' :
			exit ( EXIT_SUCCESS );
			break;
		default :
			break;
	}
}

// ===========================================================
void Show_RGBA_Image_BYTE ( void )
{
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glDrawPixels( width, height, GL_RGBA, GL_FLOAT, display_byte_image );			
}

// ===========================================================
void Show_RGBA_Image_FLOAT ( void )
{
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glDrawPixels( width, height, GL_RGBA, GL_FLOAT, display_float_image );			

}

// ===========================================================
void Mouse_Click ( int button, int state, int x, int y )
{
	if ( state == GLUT_DOWN ) {
		switch ( button ) {
			case GLUT_LEFT_BUTTON :
			 	if (( image_type == 0 ) || ( image_type == 1 )) { 
					// PIXEL_32BIT of PIXEL_64BIT 
					Show_RGBA_Image_BYTE ( ) ;
				}
				else { 
					// PIXEL_128BIT of PIXEL_160BIT 
					Show_RGBA_Image_FLOAT ( ) ;
				}
				break;
			case GLUT_MIDDLE_BUTTON :
				break;
			case GLUT_RIGHT_BUTTON :
				break;
		}
	}
}


