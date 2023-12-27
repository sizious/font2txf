#ifndef __FNTWRITE_H__
#define __FNTWRITE_H__

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <vector>

#include "global.h"
#include "utils.h"

struct TexGlyphInfo
{
    unsigned short c;       /* Potentially support 16-bit glyphs. */
    unsigned char width;
    unsigned char height;
    signed char xoffset;
    signed char yoffset;
    signed char advance;
    char dummy;           /* Space holder for alignment reasons. */
    short x;
    short y;
};


struct TexFontWriter
{
    enum eFormat
    {
        TXF_FORMAT_BYTE   = 0,
        TXF_FORMAT_BITMAP = 1
    };

    TexFontWriter() : tgi(0) {}
    ~TexFontWriter();

    void setGlyphCount( int );
    void write( const char* filename );

    int format;
    int tex_width;
    int tex_height;
    int max_ascent;
    int max_descent;
    int num_glyphs;

    //int min_glyph;
    //int range;

    unsigned char* teximage;
    TexGlyphInfo* tgi;
};


#endif /* __FNTWRITE_H__ */
