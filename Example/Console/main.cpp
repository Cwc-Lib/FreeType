 /*  -== Freetype LCD example ==-
 *
 * Simple use of Freetype in console
 *
 *  @autors
 *   - Maeiky
 *  
 * Copyright (c) 2021 - V·Liance
 *
 * The contents of this file are subject to the Apache License Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * If a copy of the Apache License Version 2.0 was not distributed with this file,
 * You can obtain one at https://www.apache.org/licenses/LICENSE-2.0.html
 *
* Description:
* 
* Some utils function to view pixels from memory, 
* we need to create window to view visually the memory
*
*/

#include <stdio.h>
#include <string.h>
#include <math.h>


#include <ft2build.h>
#include "freetype/freetype.h"

#define WIDTH   640
#define HEIGHT  480


unsigned char image[HEIGHT][WIDTH];


void
draw_bitmap( FT_Bitmap*  bitmap,
             FT_Int      x,
             FT_Int      y)
{
  FT_Int  i, j, p, q;
  FT_Int  x_max = x + bitmap->width;
  FT_Int  y_max = y + bitmap->rows;


  for ( i = x, p = 0; i < x_max; i++, p++ )
  {
    for ( j = y, q = 0; j < y_max; j++, q++ )
    {
      if ( i < 0      || j < 0       ||
           i >= WIDTH || j >= HEIGHT )
        continue;

      image[j][i] |= bitmap->buffer[q * bitmap->width + p];
    }
  }
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

int main( int     argc,
      char**  argv){
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

  for ( n = 0; n < num_chars; n++ )
  {
    // set transformation
    FT_Set_Transform( face, &matrix, &pen );

    //load glyph image into the slot (erase previous one)
    error = FT_Load_Char( face, text[n], FT_LOAD_RENDER );
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

  show_image();

  FT_Done_Face    ( face );
  FT_Done_FreeType( library );

  return 0;
}
