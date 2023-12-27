#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <vector>

//#include "global.h"
#include "utils.h"

#include "txfbuild.h"


#define FT_PIXELS(x)  (x >> 6)


void txf_dump_image( unsigned char* buf, int pitch, int w, int h, bool fit )
{
    int x, y;
    
    printf( "print_img:  pitch=%d  w=%d h=%d  fit=%s\n", pitch, w, h, bool_to_str( fit ) );
	
    /* Fit on 80 column terminal. */
    if( fit && w > 39 )
        w = 39;

    for( y = 0; y < h; y++ )
    {
        for( x = 0; x < w; x++ )
            printf( "%02x", (int) *(buf + x) );
        printf( "\n" );
        buf += pitch;
    }
    printf( "\n" );
}


void dump_char_maps( FT_Face face )
{
    FT_CharMap charmap;
    int n;

    printf( "  CharMaps %d [\n", face->num_charmaps );
    for( n = 0; n < face->num_charmaps; n++ )
    {
        long enc;
        charmap = face->charmaps[ n ];
        enc = charmap->encoding;
        printf( "    %lx (%c%c%c%c)\n", enc,
                 (char) ((enc >> 24) & 0xff),
                 (char) ((enc >> 16) & 0xff),
                 (char) ((enc >> 8) & 0xff),
                 (char) (enc & 0xff) );
    }
    printf( "  ]\n" );
}


void blit_glyph_to_bitmap( FT_Bitmap* src, FT_Bitmap* dst, int x, int y )
{
    unsigned char* s;
    unsigned char* d;
    unsigned char* dend;
    int r;

    s = src->buffer;
    d = dst->buffer + (y * dst->pitch) + x;
    dend = dst->buffer + (dst->rows * dst->pitch);

    r = src->rows;
    while( r && (d < dend) )
    {
        memcpy( d, s, src->width );
        s += src->pitch;
        d += dst->pitch;
        r--;
    }
}


static FT_Error render_glyph( FT_Bitmap* img, FT_GlyphSlot glyph,
                              int x_offset, int y_offset, bool antialias )
{
    /* first, render the glyph into an intermediate buffer */
    if( glyph->format != ft_glyph_format_bitmap )
    {
        FT_Error error = FT_Render_Glyph( glyph,
                antialias ? ft_render_mode_normal : ft_render_mode_mono );
        if( error )
            return error;
    }
 
#ifdef DEBUG
    printf( "KR offset %dx%d\n", x_offset, y_offset );
    printf( "KR left/top %d %d\n", glyph->bitmap_left, glyph->bitmap_top );
    printf( "KR metrics %ldx%ld %ldx%ld\n", 
            FT_PIXELS(glyph->metrics.width),
            FT_PIXELS(glyph->metrics.height),
            FT_PIXELS(glyph->metrics.horiBearingX),
            FT_PIXELS(glyph->metrics.horiBearingY) );
#endif

    /* Then, blit the image to the target surface */
    blit_glyph_to_bitmap( &glyph->bitmap, img,
                          x_offset + glyph->bitmap_left,
                          y_offset - glyph->bitmap_top );

    return 0;
}


/**
  Returns number of glyphs added or zero if fails.
*/

int build_txf( TexFontWriter& fontw,
               const char* file,
               const std::vector<wchar_t>& codes,
               FT_Bitmap* img,
               int psize,
               int gap,
               bool asBitmap )
{
    FT_Library library;
    FT_Face face;
    FT_Error error;
    FT_GlyphSlot glyph;
    FT_Size size;
    FT_F26Dot6 start_x;
    FT_F26Dot6 step_y;
    FT_F26Dot6 x, y;
    int count = 0;
    int fail = 0;


    error = FT_Init_FreeType( &library );
    if( error )
    {
        return 0;
    }


    error = FT_New_Face( library, file, 0, &face );
    if( error )
    {
        return 0;
    }


    if( g_verbose )
    {
        printf( "FT_Face [\n" );
        printf( "  family_name: \"%s\"\n", face->family_name );
        printf( "  style_name:  \"%s\"\n",  face->style_name );
        printf( "  num_glyphs:  %ld\n", face->num_glyphs );
        dump_char_maps( face );
        printf( "]\n" );
    }


    error = FT_Set_Pixel_Sizes( face, psize, psize );
    if( error )
    {
        return 0;
    }


    glyph = face->glyph;
    size  = face->size;


    fontw.setGlyphCount( face->num_glyphs );
    //fontw.max_ascent  = size->metrics.y_ppem;
    // fontw.max_ascent  = FT_PIXELS(face->ascender);
    // fontw.max_descent = -FT_PIXELS(face->descender);
    fontw.max_ascent  = FT_PIXELS( (int) (face->ascender * (float) psize / 30.0f) );
    fontw.max_descent = -FT_PIXELS( (int) (face->descender * (float) psize / 30.0f) );


    /* Clear bitmap */
    memset( img->buffer, 0, img->rows * img->pitch );


    step_y = size->metrics.y_ppem + gap;


    start_x = gap;
    x = start_x;
    y = step_y;


    for (unsigned int i = 0; i < codes.size(); i++)
    {
        int glyph_index = FT_Get_Char_Index(face, codes[i]);
        if( glyph_index == 0 )
        {
            printf( "Code 0x%x is undefined\n", (int) codes[i]);
            continue;
        }

        error = FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT );
        if( ! error )
        {
            unsigned int nextX = x + FT_PIXELS(glyph->metrics.horiAdvance) + gap;

            if( nextX > img->width )
            {
                x  = start_x;
                y += step_y;

                if( (unsigned int) y >= img->rows )
                {
                    fprintf( stderr, "Texture too small for %dpt %s\n",
                             psize, file);
                    break;
                }

                nextX = x + FT_PIXELS(glyph->metrics.horiAdvance) + gap;
            }

            render_glyph( img, glyph, x, y, ! asBitmap );

            TexGlyphInfo& tgi = fontw.tgi[ count ];
            count++;

            tgi.c       = codes[i];
            tgi.width   = FT_PIXELS(glyph->metrics.width);
            tgi.height  = FT_PIXELS(glyph->metrics.height);
            tgi.xoffset = FT_PIXELS(glyph->metrics.horiBearingX);
            tgi.yoffset = FT_PIXELS(glyph->metrics.horiBearingY) - tgi.height;
            tgi.advance = FT_PIXELS(glyph->metrics.horiAdvance);
            tgi.x       = x + tgi.xoffset;
            tgi.y       = fontw.tex_height - y + tgi.yoffset;

#ifdef DEBUG
            printf("char: \"%c\"  code: %04x  size=%dx%d\n", tgi.c, tgi.c, tgi.width, tgi.height);
#endif
			
            x = nextX;
        }
        else
        {
            fail++;
        }
    }

    if( fail )
        printf( "Failed to load %d glyphs.\n", fail );


    FT_Done_Face( face );
    FT_Done_FreeType( library );

    return count;
}
