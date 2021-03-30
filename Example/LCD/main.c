
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <windows.h>


HINSTANCE xe_hInstance = 0;

#define ShowPixView
//#define USE_Transparent_PixView
#include "Xternal/Xternal.h"
#include "XE/XEGI.h"

#include <ft2build.h>
//#include <freetype.h>
#include "freetype/freetype.h"
#include "freetype/ftlcdfil.h"


#define WIDTH   640
#define HEIGHT  480

unsigned char image[HEIGHT][WIDTH];
uint32_t pixels[HEIGHT][WIDTH];

void
draw_bitmap( FT_Bitmap*  bitmap,
             FT_Int      _x,
             FT_Int      _y)
{
	FT_Int  i, j, p, q;
	FT_Int  x_max = _x + bitmap->width;
	FT_Int  y_max = _y + bitmap->rows;

	printf("\nbitmap->pixel_mode: %d \n", bitmap->pixel_mode  );
	printf("\nFT_PIXEL_MODE_MONO: %d \n", FT_PIXEL_MODE_MONO  );
	printf("\n bitmap->pitch  %d \n", bitmap->pitch  );
	printf("\n bitmap->width  %d \n", bitmap->width  );
	printf("\n bitmap->rows  %d \n", bitmap->rows  );

	for ( int y = 0; y < bitmap->rows; y++ ){
		 for ( int x = 0; x < bitmap->width; x+=3 ){
			if(bitmap->buffer != 0 ){
				uint8_t r = bitmap->buffer[y*bitmap->pitch + x+0];
				uint8_t g = bitmap->buffer[y*bitmap->pitch + x+1];
				uint8_t b = bitmap->buffer[y*bitmap->pitch + x+2];
				pixels[_y + y][_x +x/3 ] |=  (r<<16) | (g<<8) | b;
			}
		}
	}
}

int WINAPI 
	WinMain (HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPSTR _lpCmdLine, int _nShowCmd)
{
	dbg_printl("-== FreeType ==- ");
	setbuf(stdout, NULL);//Required to see every printf
	setbuf(stderr, NULL);//Required to see every printf


	FT_Library    library;
	FT_Face       face;

	FT_GlyphSlot  slot;
	FT_Matrix     matrix;                 // transformation matrix
	FT_Vector     pen;                    // untransformed origin
	FT_Error      error;

	char*         filename;
	char*         text;

	double        angle;
	int           target_height;
	int           n, num_chars;


	//filename      = argv[1];                           // first argument
	//text          = argv[2];                           // second argument

	filename = (char*)"c:/windows/fonts/arial.ttf";
	text = (char*)"GZE Embed FreeType!";


	num_chars     = strlen( text );
	angle         = ( 25.0 / 360 ) * 3.14159 * 2;      // use 25 degrees
	target_height = HEIGHT;

	error = FT_Init_FreeType( &library );              // initialize library
	// error handling omitted

	error = FT_New_Face( library, filename, 0, &face );// create face object
	// error handling omitted

	// use 50pt at 100dpi
	error = FT_Set_Char_Size( face, 50 * 64, 0,
							100, 0 );                //set character size
	// error handling omitted
	slot = face->glyph;

	//set up matrix
	matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
	matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
	matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
	matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );

	// the pen position in 26.6 cartesian space coordinates;
	// start at (300,200) relative to the upper left corner
	pen.x = 30 * 64;
	pen.y = ( target_height - 200 ) * 64 - 12000;

	//FT_Library_SetLcdFilter( library, FT_LCD_FILTER_DEFAULT);
	FT_Library_SetLcdFilter( library, FT_LCD_FILTER_LIGHT);

	for ( n = 0; n < num_chars; n++ )
	{
		// set transformation
		FT_Set_Transform( face, &matrix, &pen );

		error = FT_Load_Char( face, text[n], FT_LOAD_RENDER | FT_LOAD_TARGET_LCD );
		if ( error )
		  continue;                 // ignore errors

		//now, draw to our target surface (convert position)
		draw_bitmap( &slot->bitmap,
					 slot->bitmap_left,
					 target_height - slot->bitmap_top );

		// increment pen position
		pen.x += slot->advance.x;
		pen.y += slot->advance.y;
	}
  
	int idx = Create_context((ContextInf){.width=WIDTH, .height=HEIGHT});
	Blit_context(idx, (uint32_t*)pixels, WIDTH);
  
	while(1){
		Sleep(1);
	}
	
	FT_Done_Face    ( face );
	FT_Done_FreeType( library );

	return 0;
}
