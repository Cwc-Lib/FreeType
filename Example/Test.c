
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

/*
for (Tindex y = 0; y < bitmap.rows; y++)
{
    Tindex row = rowStartPos + y;
    for (Tindex x = 0; x < bitmap.width; x++)
    {
        Tindex col = colStartPos + x;
        uint8_t r = bitmap.buffer[y * bitmap.pitch + x*3];
        uint8_t g = bitmap.buffer[y * bitmap.pitch + x*3 + 1];
        uint8_t b = bitmap.buffer[y * bitmap.pitch + x*3 + 2];
        img(col, row) = pixel{r, g, b, 255};
        // img and pixel are placeholders to simplify the code
    }
}
*/

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
			
			//if(_y + y < HEIGHT &&_x +x < WIDTH){
				pixels[_y + y][_x +x/3 ] |=  (r<<16) | (g<<8) | b;
			//}
		}
	}
  }

/*
  for ( i = x, p = 0; i < x_max; i++, p++ )
  {
    for ( j = y, q = 0; j < y_max; j++, q++ )
    {
      if ( i < 0      || j < 0       ||
           i >= WIDTH || j >= HEIGHT )
        continue;

	  unsigned char _gray = bitmap->buffer[q * bitmap->width + p];
	  
      //image[j][i] |= _gray;
	  //pixels[j][i] |= _gray | (_gray<<8)| (_gray<<16); //0xAARRGGGBB
	  //pixels[j][i] |= bitmap->buffer[q * bitmap->width + p] ; //0xAARRGGGBB
	  //pixels[j][i] |= _gray ; //0xAARRGGGBB
	//  pixels[j][i] |= 0xFF000000 | _gray | (_gray<<8)| (_gray<<16);  //0xAARRGGGBB //|= because glyph overlap
	  
	  
	   uint8_t r = bitmap->buffer[q * bitmap->pitch + p*3];
        uint8_t g = bitmap->buffer[q * bitmap->pitch + p*3 + 1];
        uint8_t b = bitmap->buffer[q * bitmap->pitch + p*3 + 2];
	  
	    pixels[j][i] |= (r<<16) | (g<<8) | b;
    }
  }
  */
  
}


void show_image( void )
{
  int  i, j;


  for ( i = 0; i < HEIGHT; i++ )
  {
    for ( j = 0; j < WIDTH; j++ )
      putchar( image[i][j] == 0 ? '`'
                                : image[i][j] < 128 ? '+'
                                                    : '*' );
    putchar( '\n' );
  }
}




//int main( int     argc,  char**  argv){

int WINAPI 
	WinMain (HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPSTR _lpCmdLine, int _nShowCmd)
{
	dbg_printl("-== FreeType ==- ");
	setbuf(stdout, NULL);//Required to see every printf
	setbuf(stderr, NULL);//Required to see every printf
	
	xe_hInstance = _hInstance;


	  
 setbuf(stdout, NULL);
 setbuf(stderr, NULL);
 
		  
		  

		  
		  
		  
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

// FT_PIXEL_MODE_LCD
    //load glyph image into the slot (erase previous one)
	//FT_RENDER_MODE_LCD
	//FT_LCD_FILTER_DEFAULT 
    error = FT_Load_Char( face, text[n], FT_LOAD_RENDER | FT_LOAD_TARGET_LCD );
   // error = FT_Load_Char( face, text[n], FT_LOAD_RENDER  );
  //  error = FT_Load_Char( face, text[n], FT_LOAD_RENDER  );
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
  
  //	int idx = Create_context((ContextInf){.width=WIDTH/4, .height=HEIGHT/4});
	//Blit_context(idx, (uint32_t*)image, WIDTH/4*3);
	  
	int idx = Create_context((ContextInf){.width=WIDTH, .height=HEIGHT});
	Blit_context(idx, (uint32_t*)pixels, WIDTH);
	  

//pixels
 // show_image();
  
	while(1){
		Sleep(1);
	}
	
  FT_Done_Face    ( face );
  FT_Done_FreeType( library );

  return 0;
}
