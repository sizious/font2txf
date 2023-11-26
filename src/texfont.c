
/* Copyright (c) Mark J. Kilgard, 1997. */


/* This program is freely distributable without licensing fees  and is
   provided without guarantee or warrantee expressed or  implied. This
   program is -not- in the public domain. */

/*
   Modified by Karl Robillard (Aug. 4, 2001)
   Now Requires GL_VERSION_1_1 or later.
*/


#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/glu.h>
#include "texfont.h"


int txfUseLuminanceAlpha = 1;


/* byte swap a 32-bit value */
#define SWAPL(x, n) { \
                 n = ((char*) (x))[0];\
                 ((char*) (x))[0] = ((char*) (x))[3];\
                 ((char*) (x))[3] = n;\
                 n = ((char*) (x))[1];\
                 ((char*) (x))[1] = ((char*) (x))[2];\
                 ((char*) (x))[2] = n; }

/* byte swap a short */
#define SWAPS(x, n) { \
                 n = ((char*) (x))[0];\
                 ((char*) (x))[0] = ((char*) (x))[1];\
                 ((char*) (x))[1] = n; }


static TexGlyphVertexInfo* getTCVI( TexFont* txf, int c )
{
    TexGlyphVertexInfo* tgvi;

    /* Automatically substitute uppercase letters with lowercase if not
    uppercase available (and vice versa). */
    if( (c >= txf->min_glyph) && (c < txf->min_glyph + txf->range) )
    {
        tgvi = txf->lut[c - txf->min_glyph];
        if( tgvi )
            return tgvi;

        if( islower(c) )
        {
            c = toupper(c);
            if( (c >= txf->min_glyph) && (c < txf->min_glyph + txf->range) )
            {
                return txf->lut[c - txf->min_glyph];
            }
        }
        else if( isupper(c) )
        {
            c = tolower(c);
            if( (c >= txf->min_glyph) && (c < txf->min_glyph + txf->range) )
            {
                return txf->lut[c - txf->min_glyph];
            }
        }
    }

    fprintf( stderr,
    "texfont: tried to access unavailable font character \"%c\" (%d)\n",
    isprint(c) ? c : ' ', c );
    abort();
    /* NOTREACHED */
}


static char* lastError;


char* txfErrorString(void)
{
    return lastError;
}


TexFont* txfLoadFont( const char* filename )
{
    TexFont* txf;
    FILE* file;
    GLfloat w, h, xstep, ystep;
    char fileid[4];
    char tmp;
    GLubyte* texbitmap;
    int min_glyph, max_glyph;
    int endianness, swap, format, stride, width, height;
    int i, j, got;

#define EXPECT(n) if (got != n) { lastError = "premature end of file."; goto error; }

#define MEMORY(p) if( p == NULL ) { lastError = "out of memory."; goto error; }


    /* Ensure external file format sizes. */
    assert(sizeof(int) == 4);
    assert(sizeof(TexGlyphInfo) == 12);

    txf = NULL;

    file = fopen( filename, "rb" );
    if( file == NULL )
    {
        lastError = "file open failed.";
        return NULL;
    }

    got = fread( fileid, 1, 4, file );
    if( got != 4 || strncmp(fileid, "\377txf", 4) )
    {
        lastError = "not a texture font file.";
        goto error;
    }

    got = fread(&endianness, sizeof(int), 1, file);
    if( got == 1 && endianness == 0x12345678 )
    {
        swap = 0;
    }
    else if( got == 1 && endianness == 0x78563412 )
    {
        swap = 1;
    }
    else
    {
        lastError = "not a texture font file.";
        goto error;
    }


    txf = (TexFont*) malloc( sizeof(TexFont) );
    MEMORY( txf )

    txf->tex_id    = 0;

    /* For easy cleanup in error case. */
    txf->tgi       = NULL;
    txf->tgvi      = NULL;
    txf->lut       = NULL;
    txf->tex_image = NULL;


    got = fread( &format, sizeof(int), 1, file );
    EXPECT(1);
    got = fread( &txf->tex_width, sizeof(int), 1, file );
    EXPECT(1);
    got = fread( &txf->tex_height, sizeof(int), 1, file );
    EXPECT(1);
    got = fread( &txf->max_ascent, sizeof(int), 1, file );
    EXPECT(1);
    got = fread( &txf->max_descent, sizeof(int), 1, file );
    EXPECT(1);
    got = fread( &txf->num_glyphs, sizeof(int), 1, file );
    EXPECT(1);

    if( swap )
    {
        SWAPL( &format, tmp );
        SWAPL( &txf->tex_width, tmp );
        SWAPL( &txf->tex_height, tmp );
        SWAPL( &txf->max_ascent, tmp );
        SWAPL( &txf->max_descent, tmp );
        SWAPL( &txf->num_glyphs, tmp );
    }

    txf->tgi = (TexGlyphInfo*) malloc(txf->num_glyphs * sizeof(TexGlyphInfo));
    MEMORY( txf->tgi )

    got = fread(txf->tgi, sizeof(TexGlyphInfo), txf->num_glyphs, file);
    EXPECT(txf->num_glyphs);

    if( swap ) 
    {
        for( i = 0; i < txf->num_glyphs; i++ )
        {
            SWAPS( &txf->tgi[i].c, tmp );
            SWAPS( &txf->tgi[i].x, tmp );
            SWAPS( &txf->tgi[i].y, tmp );
        }
    }

    txf->tgvi = (TexGlyphVertexInfo*)
                malloc( txf->num_glyphs * sizeof(TexGlyphVertexInfo) );
    MEMORY( txf->tgvi )

    w = txf->tex_width;
    h = txf->tex_height;
#if 0
    // This adjustment causes the bottom texel row to dissappear on my
    // sysem (NVidia Linux GL drivers) - KR.
    xstep = 0.5f / w;
    ystep = 0.5f / h;
#else
    xstep = 0.0f;
    ystep = 0.0f;
#endif

    for( i = 0; i < txf->num_glyphs; i++ )
    {
        TexGlyphInfo* tgi;
        TexGlyphVertexInfo* vi;

        tgi = &txf->tgi[i];
        vi  = &txf->tgvi[i];

        vi->min_s = tgi->x / w + xstep;
        vi->min_t = tgi->y / h + ystep;
        vi->max_s = (tgi->x + tgi->width) / w + xstep;
        vi->max_t = (tgi->y + tgi->height) / h + ystep;

        vi->v0[0] = tgi->xoffset;
        vi->v0[1] = tgi->yoffset;
        vi->v1[0] = tgi->xoffset + tgi->width;
        vi->v1[1] = tgi->yoffset;
        vi->v2[0] = tgi->xoffset + tgi->width;
        vi->v2[1] = tgi->yoffset + tgi->height;
        vi->v3[0] = tgi->xoffset;
        vi->v3[1] = tgi->yoffset + tgi->height;

        vi->advance = tgi->advance;
    }

    min_glyph = txf->tgi[0].c;
    max_glyph = txf->tgi[0].c;
    for( i = 1; i < txf->num_glyphs; i++ )
    {
        if( txf->tgi[i].c < min_glyph )
            min_glyph = txf->tgi[i].c;
        if( txf->tgi[i].c > max_glyph )
            max_glyph = txf->tgi[i].c;
    }

    txf->min_glyph = min_glyph;
    txf->range = max_glyph - min_glyph + 1;

    txf->lut = (TexGlyphVertexInfo **)
               calloc( txf->range, sizeof(TexGlyphVertexInfo*) );
    MEMORY( txf->lut )

    for( i = 0; i < txf->num_glyphs; i++ )
    {
        txf->lut[txf->tgi[i].c - txf->min_glyph] = &txf->tgvi[i];
    }

    switch( format )
    {
    case TXF_FORMAT_BYTE:
        if( txfUseLuminanceAlpha )
        {
            GLubyte* orig;
            int tsize = txf->tex_width * txf->tex_height;

            orig = (GLubyte*) malloc( tsize );
            MEMORY( orig )

            got = fread( orig, 1, tsize, file );
            EXPECT( tsize )

            txf->tex_image = (GLubyte*) malloc( 2 * tsize );
            MEMORY( txf->tex_image )

            {
                GLubyte* src = orig;
                GLubyte* dst = txf->tex_image;
                for( i = 0; i < tsize; i++ )
                {
                    *dst++ = *src;
                    *dst++ = *src++;
                }
            }

            free( orig );
        }
        else
        {
            int tsize = txf->tex_width * txf->tex_height;

            txf->tex_image = (GLubyte*) malloc( tsize );
            MEMORY( txf->tex_image )

            got = fread( txf->tex_image, 1, tsize, file );
            EXPECT( tsize )
        }
        break;

    case TXF_FORMAT_BITMAP:
        width  = txf->tex_width;
        height = txf->tex_height;
        stride = (width + 7) >> 3;

        texbitmap = (GLubyte*) malloc( stride * height );
        MEMORY( texbitmap )

        got = fread( texbitmap, 1, stride * height, file );
        EXPECT( stride * height );

        if( txfUseLuminanceAlpha )
        {
            txf->tex_image = (GLubyte*) calloc(width * height * 2, 1);
            MEMORY( txf->tex_image )

            for( i = 0; i < height; i++ )
            {
                for( j = 0; j < width; j++ )
                {
                    if( texbitmap[i * stride + (j >> 3)] & (1 << (j & 7)) )
                    {
                        txf->tex_image[(i * width + j) * 2]     = 255;
                        txf->tex_image[(i * width + j) * 2 + 1] = 255;
                    }
                }
            }
        }
        else
        {
            txf->tex_image = (GLubyte*) calloc( width * height, 1 );
            MEMORY( txf->tex_image )

            for( i = 0; i < height; i++ )
            {
                for( j = 0; j < width; j++ )
                {
                    if( texbitmap[i * stride + (j >> 3)] & (1 << (j & 7)) )
                        txf->tex_image[i * width + j] = 255;
                }
            }
        }

        free( texbitmap );
        break;
    }

    fclose( file );
    return txf;

error:

    if( txf )
    {
        if( txf->tgi )
            free( txf->tgi );
        if( txf->tgvi )
            free( txf->tgvi );
        if( txf->lut )
            free( txf->lut );
        if( txf->tex_image )
            free( txf->tex_image );
        free( txf );
    }

    fclose( file );
    return NULL;
}


/**
  If tex_id is zero then glGenTextures() is called.
  Min is the GL_TEXTURE_MIN_FILTER setting.
  Mag is the GL_TEXTURE_MAG_FILTER setting.
  Returns the GL texture id.
*/

GLuint txfEstablishTexture( TexFont* txf, GLuint tex_id, GLboolean mipmap,
                            GLenum magf, GLenum minf )
{
    if( txf->tex_id == 0 )
    {
        if( tex_id == 0 )
        {
            glGenTextures( 1, &txf->tex_id );
        }
        else
        {
            txf->tex_id = tex_id;
        }
    }

    glBindTexture( GL_TEXTURE_2D, txf->tex_id );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minf );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magf );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP ) ;
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP ) ;

#if 0
  /* XXX Indigo2 IMPACT in IRIX 5.3 and 6.2 does not support the GL_INTENSITY
     internal texture format. Sigh. Win32 non-GLX users should disable this
     code. */
  if (txfUseLuminanceAlpha == 0) {
    char *vendor, *renderer, *version;

    renderer = (char *) glGetString(GL_RENDERER);
    vendor = (char *) glGetString(GL_VENDOR);
    if (!strcmp(vendor, "SGI") && !strncmp(renderer, "IMPACT", 6)) {
      version = (char *) glGetString(GL_VERSION);
      if (!strcmp(version, "1.0 Irix 6.2") ||
        !strcmp(version, "1.0 Irix 5.3")) {
        unsigned char *latex;
        int width = txf->tex_width;
        int height = txf->tex_height;
        int i;

        txfUseLuminanceAlpha = 1;
        latex = (unsigned char *) calloc(width * height * 2, 1);
        /* XXX unprotected alloc. */
        for (i = 0; i < height * width; i++) {
          latex[i * 2] = txf->tex_image[i];
          latex[i * 2 + 1] = txf->tex_image[i];
        }
        free(txf->tex_image);
        txf->tex_image = latex;
      }
    }
  }
#endif

    if( txfUseLuminanceAlpha )
    {
        if( mipmap )
        {
            gluBuild2DMipmaps( GL_TEXTURE_2D, GL_LUMINANCE_ALPHA,
                     txf->tex_width, txf->tex_height,
                     GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, txf->tex_image );
        }
        else
        {
            glTexImage2D( GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA,
                      txf->tex_width, txf->tex_height, 0,
                      GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, txf->tex_image );
        }
    }
    else
    {
        /* Use GL_INTENSITY4 as internal texture format since we want to use
           as little texture memory as possible.
         */
        if( mipmap )
        {
            gluBuild2DMipmaps( GL_TEXTURE_2D, GL_INTENSITY4,
                           txf->tex_width, txf->tex_height,
                           GL_LUMINANCE, GL_UNSIGNED_BYTE, txf->tex_image );
        }
        else
        {
            glTexImage2D( GL_TEXTURE_2D, 0, GL_INTENSITY4,
                          txf->tex_width, txf->tex_height, 0,
                          GL_LUMINANCE, GL_UNSIGNED_BYTE, txf->tex_image );
        }
    }

    return txf->tex_id;
}


void txfBindFontTexture( TexFont* txf )
{
    glBindTexture( GL_TEXTURE_2D, txf->tex_id );
}


void txfUnloadFont( TexFont* txf )
{
    if( txf->tex_image )
        free( txf->tex_image );

    free( txf->tgi );
    free( txf->tgvi );
    free( txf->lut );
    free( txf );
}


void txfGetStringMetrics( TexFont* txf, char* string, int len,
                          int* width, int* max_ascent, int* max_descent )
{
    TexGlyphVertexInfo *tgvi;
    int w, i;

    w = 0;
    for( i = 0; i < len; i++ )
    {
        if( string[i] == 27 )
        {
            switch( string[i + 1] )
            {
                case 'M': i += 4;  break;
                case 'T': i += 7;  break;
                case 'L': i += 7;  break;
                case 'F': i += 13; break;
            }
        }
        else
        {
            tgvi = getTCVI( txf, string[i] );
            w += tgvi->advance;
        }
    }

    *width = w;
    *max_ascent  = txf->max_ascent;
    *max_descent = txf->max_descent;
}


void txfRenderGlyph( TexFont* txf, int c )
{
    TexGlyphVertexInfo* tgvi;
    tgvi = getTCVI( txf, c );

    glBegin( GL_QUADS );
    glTexCoord2f( tgvi->min_s, tgvi->min_t );
    glVertex2sv( tgvi->v0 );
    glTexCoord2f( tgvi->max_s, tgvi->min_t );
    glVertex2sv( tgvi->v1 );
    glTexCoord2f( tgvi->max_s, tgvi->max_t );
    glVertex2sv( tgvi->v2 );
    glTexCoord2f( tgvi->min_s, tgvi->max_t );
    glVertex2sv( tgvi->v3 );
    glEnd();

    glTranslatef( tgvi->advance, 0.0, 0.0 );
}


/**
  If len is less than 1 then string is assumed to be null terminated.
*/

void txfRenderString( TexFont* txf, const char* string, int len )
{
    if( len )
    {
        while( len-- )
            txfRenderGlyph( txf, *string++ );
    }
    else
    {
        while( *string )
            txfRenderGlyph( txf, *string++ );
    }
}


enum
{
    MONO,
    TOP_BOTTOM,
    LEFT_RIGHT,
    FOUR
};

void txfRenderFancyString( TexFont* txf, const char* string, int len )
{
  TexGlyphVertexInfo* tgvi;
  GLubyte c[4][3];
  int mode = MONO;
  int i;

  for (i = 0; i < len; i++)
  {
    if (string[i] == 27)
    {
      switch (string[i + 1])
      {
      case 'M':
        mode = MONO;
        glColor3ubv((GLubyte *) & string[i + 2]);
        i += 4;
        break;

      case 'T':
        mode = TOP_BOTTOM;
        memcpy(c, &string[i + 2], 6);
        i += 7;
        break;

      case 'L':
        mode = LEFT_RIGHT;
        memcpy(c, &string[i + 2], 6);
        i += 7;
        break;

      case 'F':
        mode = FOUR;
        memcpy(c, &string[i + 2], 12);
        i += 13;
        break;
      }
    }
    else
    {
      switch( mode )
      {
      case MONO:
        txfRenderGlyph(txf, string[i]);
        break;

      case TOP_BOTTOM:
        tgvi = getTCVI(txf, string[i]);
        glBegin(GL_QUADS);
        glColor3ubv(c[0]);
        glTexCoord2f( tgvi->min_s, tgvi->min_t );
        glVertex2sv(tgvi->v0);
        glTexCoord2f( tgvi->max_s, tgvi->min_t );
        glVertex2sv(tgvi->v1);
        glColor3ubv(c[1]);
        glTexCoord2f( tgvi->max_s, tgvi->max_t );
        glVertex2sv(tgvi->v2);
        glTexCoord2f( tgvi->min_s, tgvi->max_t );
        glVertex2sv(tgvi->v3);
        glEnd();
        glTranslatef(tgvi->advance, 0.0, 0.0);
        break;

      case LEFT_RIGHT:
        tgvi = getTCVI(txf, string[i]);
        glBegin(GL_QUADS);
        glColor3ubv(c[0]);
        glTexCoord2f( tgvi->min_s, tgvi->min_t );
        glVertex2sv(tgvi->v0);
        glColor3ubv(c[1]);
        glTexCoord2f( tgvi->max_s, tgvi->min_t );
        glVertex2sv(tgvi->v1);
        glColor3ubv(c[1]);
        glTexCoord2f( tgvi->max_s, tgvi->max_t );
        glVertex2sv(tgvi->v2);
        glColor3ubv(c[0]);
        glTexCoord2f( tgvi->min_s, tgvi->max_t );
        glVertex2sv(tgvi->v3);
        glEnd();
        glTranslatef(tgvi->advance, 0.0, 0.0);
        break;

      case FOUR:
        tgvi = getTCVI(txf, string[i]);
        glBegin(GL_QUADS);
        glColor3ubv(c[0]);
        glTexCoord2f( tgvi->min_s, tgvi->min_t );
        glVertex2sv(tgvi->v0);
        glColor3ubv(c[1]);
        glTexCoord2f( tgvi->max_s, tgvi->min_t );
        glVertex2sv(tgvi->v1);
        glColor3ubv(c[2]);
        glTexCoord2f( tgvi->max_s, tgvi->max_t );
        glVertex2sv(tgvi->v2);
        glColor3ubv(c[3]);
        glTexCoord2f( tgvi->min_s, tgvi->max_t );
        glVertex2sv(tgvi->v3);
        glEnd();
        glTranslatef(tgvi->advance, 0.0, 0.0);
        break;
      }
    }
  }
}


int txfInFont( TexFont* txf, int c )
{
    /* NOTE: No uppercase/lowercase substituion. */
    if( (c >= txf->min_glyph) && (c < txf->min_glyph + txf->range) )
    {
        if( txf->lut[c - txf->min_glyph] )
            return 1;
    }
    return 0;
}


/* EOF */
