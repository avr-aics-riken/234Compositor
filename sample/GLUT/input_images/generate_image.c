/**********************************************************/
/**
 * 234Compositor - Image data merging library
 *
 * Copyright (c) 2013-2015 Advanced Institute for Computational Science, RIKEN.
 * All rights reserved.
 *
 **/
/**********************************************************/

// @file   generate_image.c

// @brief  Test program for 234Compositor (Float Image)
//         GLUT-Based Raw Image Generator
//         cc -o generate_image generate_image.c -lglut -lGLU

// @author Jorji Nonaka (jorji@riken.jp)


#define ID_RGBA32	  0	// ID for RGBA   32-bit
#define ID_RGBAZ64	1	// ID for RGBAZ  64-bit	
#define ID_RGBA128	2	// ID for RGBA  128-bit	
#define ID_RGBAZ160	3	// ID for RGBAZ 160-bit	

#define RGBA32		4	  //  4 Bytes ( Byte  RGBA : 4 * 1 Byte  )
#define RGBAZ64		8	  //  8 Bytes ( Byte  RGBA Float Z: 4 * 1 Byte + 1 * 4 Bytes )
#define RGBA128		16	// 16 Bytes ( Float RGBA : 4 * 4 Bytes )
#define RGBAZ160	20	// 20 Bytes ( Float RGBAZ: 5 * 4 Bytes )

#define MAX_NNODES  2048

#define KEY_ESC      27
#define MAX_LENGTH   255

// ======================================
//		    TYPEDEF DECLARATIONS
// ======================================
typedef unsigned char BYTE;	// Unsigned Char
// =======================================

#include <GL/glut.h>

#include <string.h>  // malloc, memset, strcpy, strcat, strrchr
#include <math.h>    // ceil
#include <stdio.h>   // printf, fprintf, sprintf, fwrite, FILE
#include <stdlib.h>  // atoi, free 
#include <stdbool.h> // true, false	

// =======================================
//			GLOBAL VARIABLES
// =======================================
_Bool loop_flag;
_Bool swap_flag;

int my_rank;	
int nnodes;
int image_type;
int pixel_size;
int max_nnodes;
int swap_endian;
int   alpha;
float alpha_f;

int width;
int height;

long image_resolution;
long image_size;

char *filename;
char *node_num;

float float_val1, float_val2, float_val3, float_val4, float_val5;
// =======================================

static GLfloat angle = 0.0;

// Pre-defined colors
GLfloat color1[]={ 1.0, 0.0, 0.0, 1.0 },
    	color2[]={ 0.0, 1.0, 0.0, 1.0 },
        color3[]={ 0.0, 0.0, 1.0, 1.0 },  
		color4[]={ 1.0, 0.0, 1.0, 1.0 },  
		color5[]={ 1.0, 1.0, 0.0, 1.0 },  
		color6[]={ 0.0, 1.0, 1.0, 1.0 };  
// =======================================

void initialize(void);
void display( void );
void reshape(int w,int h);
void keyboard(unsigned char key, int x, int y);
void animate(void);

// =======================================

// =======================================
//			INITIALIZE FUNCTION
// =======================================
void initialize(void)
{
	glDisable(GL_DEPTH_TEST); 
	glDisable(GL_BLEND);
}

// =======================================
//			DISPLAY FUNCTION
// =======================================
void display( void )
{
	int rand_color;

	typedef GLfloat point3[3];
	
	// =======================================
    point3 xy_plane[4]={ { -1.25, -0.5, 0.0 },   // X-Y plane
                         { -1.25,  0.5, 0.0 },
                         {  1.25,  0.5, 0.0 },
                         {  1.25, -0.5, 0.0 } };
	// =======================================

	glClearColor( 0.0, 0.0, 0.0, 0.1 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glPushMatrix( );

    glRotatef( angle, 0.0, 0.0, 1.0 );  

	rand_color = rand( ) % 6 + 1;
	if ( rand_color == 1 ) {
		glColor4fv(color1);		
	}
	else if ( rand_color == 2 ) {
		glColor4fv(color2);		
	}
	else if ( rand_color == 3 ) {
		glColor4fv(color3);		
	}
	else if ( rand_color == 4 ) {
		glColor4fv(color4);		
	}
	else if ( rand_color == 5 ) {
		glColor4fv(color5);		
	}
	else if ( rand_color == 6 ) {
		glColor4fv(color6);		
	}
	
    glBegin( GL_QUADS );
      glVertex3fv( xy_plane[0] );
      glVertex3fv( xy_plane[1] );
      glVertex3fv( xy_plane[2] ) ;
      glVertex3fv( xy_plane[3] );
    glEnd();
	
	glPopMatrix();  
    glutSwapBuffers();
 }

// =======================================
//			RESHAPE FUNCTION
// =======================================
void reshape( int new_width, int new_height )
{
	glViewport( 0, 0, (GLsizei)new_width, (GLsizei)new_height );
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    gluPerspective(60, 1, 1, 1000);	// setup a perspective projection
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt( 1.8, 1.8, 1.8, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );
	//           eye point   center of view      up
}

// =======================================
//		SWAP (BIG<->LITTLE ENDIAN)
// =======================================
float FloatSwap( float f )
{
  union
  {
    float f;
    unsigned char b[4];
  } dat1, dat2;
 
  dat1.f = f;
  dat2.b[0] = dat1.b[3];
  dat2.b[1] = dat1.b[2];
  dat2.b[2] = dat1.b[1];
  dat2.b[3] = dat1.b[0];
  return dat2.f;
}

// =======================================
//	WRITE RGBA IMAGE (WIDTH, HEIGHT)
// =======================================
void write_rgba_image( void )
{
	BYTE* image;
	BYTE* image_ptr;

	BYTE*  rgbaz_image;
	BYTE*  rgbaz_ptr;

   	float* float_ptr;

	float* rgbazf_image;
	float* rgbazf_ptr;
	float* imagef_ptr;

	FILE* out_fp;
	
	long i, j;
	
	printf("%d x %d (%ld Pixels): IMAGE TYPE: %d ; PIXEL SIZE: %d \n", width, height, image_resolution, image_type, pixel_size );

	image_size  = image_resolution * pixel_size;
	
	if ( ( image = (BYTE *)malloc ( sizeof(BYTE) * image_size )) == NULL ) { 
   		printf( "<<< ERROR >> Cannot allocate memory for image data \n" );
		exit( EXIT_FAILURE );	
	}
	memset( image, 0x00, sizeof(BYTE) * image_size );
	
	if ( ( node_num = (char *)malloc ( sizeof(BYTE) * MAX_LENGTH )) == NULL ) { 
   		printf( "<<< ERROR >> Cannot allocate memory for image data \n" );
		exit( EXIT_FAILURE );	
	}
	memset( node_num, 0x00, sizeof(char) * MAX_LENGTH );

	if ( image_type == ID_RGBAZ64 ) {
		if ( ( rgbaz_image = (BYTE *)malloc ( sizeof(BYTE) * image_size )) == NULL ) { 
   			printf( "<<< ERROR >> Cannot allocate memory for image data \n" );
			exit( EXIT_FAILURE );	
		}
		memset( rgbaz_image, 0x00, sizeof(BYTE) * image_size );
	} 
	else if ( image_type == ID_RGBAZ160 ) {
		if ( ( rgbazf_image = (float *)malloc ( sizeof(BYTE) * image_size )) == NULL ) { 
   			printf( "<<< ERROR >> Cannot allocate memory for image data \n" );
			exit( EXIT_FAILURE );	
		}
		memset( rgbazf_image, 0x00, sizeof(BYTE) * image_size );
	} 

	// =======================================
	for ( nnodes = 0; nnodes < max_nnodes; nnodes++ ) {
	
		filename[0] = '\0';		
	
		if ( my_rank < 10 ) {
			filename = strcat ( filename, "000" );
			sprintf( node_num, "%d", my_rank );
		}
		else if (( my_rank > 9 ) && ( my_rank < 100 )) 
		{
			filename = strcat ( filename, "00" );
			sprintf( node_num, "%d", my_rank );
		}
		else if (( my_rank > 99 ) && ( my_rank < 1000 )) 
		{
			filename = strcat ( filename, "0" );
			sprintf( node_num, "%d", my_rank );
		}
		else if (( my_rank > 999 ) && ( my_rank < 10000 )) 
		{
			sprintf( node_num, "%d", my_rank );
		}

		filename = strcat ( filename, node_num );

		switch ( image_type )
		{
			case ID_RGBA32 :
				filename = strcat ( filename, ".rgba32\0" ); break;
			case ID_RGBAZ64 :
				filename = strcat ( filename, ".rgbaz64\0" ); break;
			case ID_RGBA128 :
				filename = strcat ( filename, ".rgba128\0" ); break;
			case ID_RGBAZ160 :
				filename = strcat ( filename, ".rgbaz160\0" ); break;
		}

		printf("FILENAME: %s \n", filename );
	
		if ( (out_fp = fopen( filename, "wb")) == NULL ) {
			printf("<<< ERROR >>> Cannot open file for writing \n" );
			exit( EXIT_FAILURE );	
		}

		// =======================================
		if (( image_type == ID_RGBA32 ) || ( image_type == ID_RGBAZ64 ))
		{
			glReadPixels ( 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image );

			image_ptr = (BYTE *)image;

			if ( image_type == ID_RGBA32 ) 
			{
				// =======================================
				// 			Image Tye: 	RGBA32
				// =======================================
				for ( i = 0; i < image_size; i += 4 ) {
				
					if (( image_ptr[ i ] == 0 ) && ( image_ptr[ i + 1 ] == 0 ) && ( image_ptr[ i + 2 ] == 0 )) {
						image_ptr[ i + 3 ] = (BYTE)0; // R = G = B= 0
					}
					else {		
						image_ptr[ i + 3 ] = (BYTE)alpha;
					}	
				}

				if ( fwrite( image, image_size, 1, out_fp ) != 1 ) {
					perror("<<< ERROR >>> Cannot write image \n" );
					exit( EXIT_FAILURE );
				}
				fclose( out_fp );
			}
			else
			{
				// =======================================
				// 			Image Tye: 	RGBAZ64
				// =======================================
				rgbaz_ptr = (BYTE *)rgbaz_image;
				float_ptr = (float*)rgbaz_image;

				for ( i = 0; i < image_size/2; i += 4 ) {
		
					j = i * 2;

					rgbaz_ptr[ j     ] = (BYTE)image_ptr[ i     ]; // R
					rgbaz_ptr[ j + 1 ] = (BYTE)image_ptr[ i + 1 ]; // G
					rgbaz_ptr[ j + 2 ] = (BYTE)image_ptr[ i + 2 ]; // B

					// Z
					if (( rgbaz_ptr[ j ] == 0 ) && ( rgbaz_ptr[ j + 1 ] == 0 ) && ( rgbaz_ptr[ j + 2 ] == 0 )) {
						rgbaz_ptr[ j + 3 ] = 0; // R = G = B= 0
					}
					else {		
						rgbaz_ptr[ j + 3 ] = alpha;
					}	

					float_val1 = (float  )(( my_rank + 1 ) * 0.1 ); // Z

					if ( swap_flag == true ) {
						float_val1 = FloatSwap( float_val1 );
					}

					float_ptr    = (float *)&rgbaz_ptr[ j + 4 ];
					float_ptr[0] = (float  )float_val1;
				}

				if ( fwrite( rgbaz_image, image_size, 1, out_fp ) != 1 ) {
					perror("<<< ERROR >>> Cannot write image \n" );
					exit( EXIT_FAILURE );
				}
				fclose( out_fp );
			}
		}	
		else if (( image_type == ID_RGBA128 ) || ( image_type == ID_RGBAZ160 ))		
		{
			glReadPixels ( 0, 0, width, height, GL_RGBA, GL_FLOAT, image );

			imagef_ptr = (float *)image;

			if ( image_type == ID_RGBA128 ) 
			{
				// =======================================
				// 			Image Tye: 	RGBA128
				// =======================================
				for ( i = 0; i < ( image_resolution * 4 ); i += 4 ) {

					float_val1 = (float)imagef_ptr[ i     ];
					float_val2 = (float)imagef_ptr[ i + 1 ];
					float_val3 = (float)imagef_ptr[ i + 2 ];

					if (( float_val1 == 0.0 ) && ( float_val2 == 0.0 ) && ( float_val3 == 0.0 )) { 
						imagef_ptr[ i + 3 ] = (float)0.0; // R = G = B= 0
					}
					else {		
						imagef_ptr[ i + 3 ] = (float)alpha_f;
					}	
				}

				// =======================================
				// 		SWAP (BIG <-> LITTLE ENDIAN)
				// =======================================
				if ( swap_flag == true ) {

					for ( i = 0; i < ( image_resolution * 4 ); i += 4 ) 
					{
						float_val1 = (float)imagef_ptr[ i     ];
						float_val2 = (float)imagef_ptr[ i + 1 ];
						float_val3 = (float)imagef_ptr[ i + 2 ];
						float_val4 = (float)imagef_ptr[ i + 3 ];

						float_val1 = FloatSwap( float_val1 );
						float_val2 = FloatSwap( float_val2 );
						float_val3 = FloatSwap( float_val3 );
						float_val4 = FloatSwap( float_val4 );

						imagef_ptr[ i     ] = (float)float_val1;
						imagef_ptr[ i + 1 ] = (float)float_val2;
						imagef_ptr[ i + 2 ] = (float)float_val3;
						imagef_ptr[ i + 3 ] = (float)float_val4;
					}
				}

				if ( fwrite( image, image_size, 1, out_fp ) != 1 ) {
					perror("<<< ERROR >>> Cannot write image \n" );
					exit( EXIT_FAILURE );
				}
				fclose( out_fp );
			}
			else  
			{
				// =======================================
				// 			Image Tye: 	RGBAZ160
				// =======================================
				rgbazf_ptr = (float *)rgbazf_image;
				j = 0;

				for ( i = 0; i < ( image_resolution * 4 ); i += 4 ) {
				
					rgbazf_ptr[ j     ] = (float)imagef_ptr[ i     ]; // R
					rgbazf_ptr[ j + 1 ] = (float)imagef_ptr[ i + 1 ]; // G
					rgbazf_ptr[ j + 2 ] = (float)imagef_ptr[ i + 2 ]; // B

					if (( rgbazf_ptr[ j ] == 0.0 ) && ( rgbazf_ptr[ j + 1 ] == 0.0 ) && ( rgbazf_ptr[ j + 2 ] == 0.0 )) {
						rgbazf_ptr[ j + 3 ] = (float)0.0; // R = G = B= 0
					}
					else {		
						rgbazf_ptr[ j + 3 ] = (float)alpha_f;
					}	

					rgbazf_ptr[ j + 4 ] = (float)(( my_rank + 1 ) * 0.1 );

					j += 5;	
				}
				
				// =======================================
				// 		SWAP (BIG <-> LITTLE ENDIAN)
				// =======================================
				if ( swap_flag == true ) {

					for ( i = 0; i < ( image_resolution * 5 ); i += 5 ) 
					{
						float_val1 = (float)rgbazf_ptr[ i     ];
						float_val2 = (float)rgbazf_ptr[ i + 1 ];
						float_val3 = (float)rgbazf_ptr[ i + 2 ];
						float_val4 = (float)rgbazf_ptr[ i + 3 ];
						float_val5 = (float)rgbazf_ptr[ i + 4 ];

						float_val1 = FloatSwap( float_val1 );
						float_val2 = FloatSwap( float_val2 );
						float_val3 = FloatSwap( float_val3 );
						float_val4 = FloatSwap( float_val4 );
						float_val5 = FloatSwap( float_val5 );

						rgbazf_ptr[ i     ] = (float)float_val1;
						rgbazf_ptr[ i + 1 ] = (float)float_val2;
						rgbazf_ptr[ i + 2 ] = (float)float_val3;
						rgbazf_ptr[ i + 3 ] = (float)float_val4;
						rgbazf_ptr[ i + 4 ] = (float)float_val5;
					}
				}
				
				if ( fwrite( rgbazf_image, image_size, 1, out_fp ) != 1 ) {
					perror("<<< ERROR >>> Cannot write image \n" );
					exit( EXIT_FAILURE );
				}
				fclose( out_fp );
			}

		}
	
			my_rank ++;
			angle -= 5;

			glTranslatef( 0.01, 0.01, 0.0 );
			glScalef( 1.0, 1.0, 1.0 );
		
			display();
	}

	exit( EXIT_SUCCESS );

}

// =======================================
//			KEYBOARD FUNCTION
// =======================================
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		case KEY_ESC :
           exit(0); break;
		case 'q' :
			exit(0); break;
		case 'w' :
			if ( loop_flag == false ) {
				write_rgba_image( ); 
			}
			break;			
	}
}

// =======================================
//				MAIN FUNCTION
// =======================================
int main ( int argc, char** argv )
{
	my_rank = 0;
	
	loop_flag  = false;
	swap_flag  = false;
	
	if ( argc == 5 ) {
		width  = atoi(argv[1]);
		height = atoi(argv[2]);
		image_type = atoi(argv[3]);
		max_nnodes = atoi(argv[4]);
		if ( (image_type == 0) || (image_type == 1) ){
			alpha = 127; // Default Alpha = 50%
		}
		else {
			alpha_f = 0.5;  // Default Alpha = 50%
		}
	}
	else if ( argc == 6 ) {
		width  = atoi(argv[1]);
		height = atoi(argv[2]);
		image_type = atoi(argv[3]);
		max_nnodes = atoi(argv[4]);
		if ( (image_type == 0) || (image_type == 1) ){
			alpha = atoi(argv[5]);
		}
		else {
			alpha_f = atof(argv[5]);
		}
	}
	else if ( argc == 7 ) {
		width  = atoi(argv[1]);
		height = atoi(argv[2]);
		image_type  = atoi(argv[3]);
		max_nnodes  = atoi(argv[4]);
		swap_endian = atoi(argv[6]);
		if ( (image_type == 0) || (image_type == 1) ){
			alpha = atoi(argv[5]);
		}
		else {
			alpha_f = atof(argv[5]);
		}
	}
	else { 
		printf ("\n Usage: %s Width Height Image_Type Num_Images (Alpha: 0-255) (Swap-Endian: 0 or 1)\n\n", argv[0] );
					printf("          Image_Type: 0 ( PIXEL_32BIT  ) \n");
					printf("                      1 ( PIXEL_64BIT  ) \n");
					printf("                      2 ( PIXEL_128BIT ) \n");
					printf("                      3 ( PIXEL_160BIT ) \n\n");
					printf(" OUTPUT: XXXX.rgbxx images \n\n");
		exit( EXIT_FAILURE );
	}		

	// =======================================
	image_resolution = width * height;
	
    if (( image_type < 0 ) || ( image_type > 3 )){
		printf("\n <<< Invalid Image Type >>> \n");
		printf("          Image_Type: 0 ( PIXEL_32BIT  ) \n");
		printf("                      1 ( PIXEL_64BIT  ) \n");
		printf("                      2 ( PIXEL_128BIT ) \n");
		printf("                      3 ( PIXEL_160BIT ) \n\n");
		exit( EXIT_FAILURE );
	}

    if ( max_nnodes < 0 ){
		printf("\n Number of images is too small \n");
		exit( EXIT_FAILURE );
	}
    else if ( max_nnodes > MAX_NNODES ){
		printf("\n Number of images is too large \n");
		exit( EXIT_FAILURE );
	}

    if ( swap_endian == 1 ){
		swap_flag = true;
	}

	switch ( image_type )
	{
		case 0 :
           pixel_size = RGBA32; break;
		case 1 :
           pixel_size = RGBAZ64; break;
		case 2 :
           pixel_size = RGBA128; break;
		case 3 :
           pixel_size = RGBAZ160; break;
	}

	filename = (char *)malloc( sizeof(char) * MAX_LENGTH ); 	
	memset( filename, 0, sizeof(char) * MAX_LENGTH );

	printf ("\n Press \'w\' to WRITE the images as files. \n" );
	printf (" Press \'q\' to QUIT  the program \n\n" );

	// =======================================
	glutInit(&argc,argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(width,height);
	glutInitWindowPosition( 70, 70 );
	glutCreateWindow("Generate RAW images");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard); 
	
	initialize();
	glutMainLoop();
	
	return 0;
}
