/*============================================================================
//
// $Id: ttf2txf.cpp,v 1.11 2001/10/20 23:46:37 karl Exp $
//
// Converts fonts to TXF texture font format.
// Uses Freetype 2.0.
//
//==========================================================================*/


/*
 TODO:
    * Pack glyphs more efficiently.
    * Save as bitmap.
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <vector>

#include "utils.h"
#include "txfbuild.h"
#include "charset.h"
#include "display.h"

std::vector<wchar_t> g_char_codes;

bool g_verbose = true;

FT_Bitmap g_txf;

char _default_codes[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890abcdefghijklmnopqrstuvwxyz?.;,!*:\"/+-|'@#$%^&<>()[]{}_";


void usage()
{
    printf( "%s version %s (%s)\n\n", TTF2TXF_PROGNAME, TTF2TXF_VERSION, __DATE__ );
    printf( "Usage: %s [options] <TrueType Font>\n\n", program_name_get() );
    printf( "Options:\n" );
    printf( "  -w <width>          Texture width  (default 256)\n" );
    printf( "  -h <height>         Texture height (default 256)\n" );
//    printf( "  -b                  Create bitmap texture\n" );
    printf( "  -f <filename.txt>   File containing character codes to convert\n");
    printf( "  -c <string>         Characters to convert\n" );
    printf( "  -g <gap>            Space between glyphs (default 1)\n" );
    printf( "  -s <size>           Font point size (default 20)\n" );
    printf( "  -o <filename.txf>   Output file for textured font\n" );
    printf( "  -q                  Quiet; no output\n" );
}


int main( int argc, char** argv )
{
    TexFontWriter fontw;
    int i;
    int gap = 1;
    int size = 20;
    bool asBitmap = false;
    char* infile = 0;
    char outfile[FILENAME_MAX];
    std::string codesfile;
    char* codes = _default_codes;
    
    if( argc < 2 )
    {
        program_name_initialize( argv[0] );
        usage();
        program_name_finalize();
        return 0;
    }

    outfile[ 0 ] = '\0';

    fontw.format     = TexFontWriter::TXF_FORMAT_BYTE;
    fontw.tex_width  = 256;
    fontw.tex_height = 256;

    for( i = 1; i < argc; i++ )
    {
        if( *argv[i] == '-' )
        {
            char* cp = argv[i] + 1;

            if( *cp == 'w' )
            {
                i++;
                if( i >= argc )
                    break;
                fontw.tex_width = atoi(argv[i]);
            }
            else if( *cp == 'h' )
            {
                i++;
                if( i >= argc )
                    break;
                fontw.tex_height = atoi(argv[i]);
            }
            else if( *cp == 'c' )
            {
                i++;
                if( i >= argc )
                    break;
                codes = argv[i];
                printf("codes: %s\n", codes);
            }
            else if( *cp == 'b' )
            {
                printf( "as bitmap\n" );
                asBitmap = true;
            }
            else if( *cp == 'g' )
            {
                i++;
                if( i >= argc )
                    break;
                gap = atoi(argv[i]);
            }
            else if( *cp == 's' )
            {
                i++;
                if( i >= argc )
                    break;
                size = atoi(argv[i]);
            }
            else if( *cp == 'o' )
            {
                i++;
                if( i >= argc )
                    break;
                strcpy( outfile, argv[i] );
            }
            else if( *cp == 'q' )
            {
                g_verbose = false;
            }
            else if (*cp == 'f')
            {
                i++;
                if (i >= argc)
                    break;
                codesfile = argv[i];
            }
        }
        else
        {
            infile = argv[i];
        }
    }

    if( ! infile )
    {
        usage();
        exit( -1 );
    }

    if( outfile[ 0 ] == '\0' )
    {
        // Set outfile to base infile and append ".txf"

        char* src = infile;
        char* dst = outfile;
        while( *src )
        {
            if( *src == '/' || *src == '\\' )
            {
                // Reset to strip path.
                dst = outfile;
            }
            else if( *src == '.' )
            {
                break;
            }
            else
            {
                *dst++ = *src;
            }
            ++src;
        }
        strcpy( dst, ".txf" ); 
    }

    g_txf.width  = fontw.tex_width;
    g_txf.rows   = fontw.tex_height;
    g_txf.pitch  = g_txf.width;
    g_txf.buffer = (unsigned char*) malloc( g_txf.pitch * g_txf.rows );

    // Populate the list of character codes
    if ( !codesfile.empty() )
    {
        loadCharCodesFile( codesfile );
    }
    else
    {
        int i = 0;
        while ( codes[i] != '\0' )
        {
            g_char_codes.insert(g_char_codes.end(), (wchar_t) codes[i]);
            i++;
        }
    }

    fontw.num_glyphs = build_txf( fontw, infile, g_char_codes, &g_txf, size, gap,
                                  asBitmap );
    if( ! fontw.num_glyphs )
        return -1;

    if( g_verbose )
    {
        printf( "TexFont [\n" );
        printf( "  format:      %d\n", fontw.format );
        printf( "  tex_width:   %d\n", fontw.tex_width );
        printf( "  tex_height:  %d\n", fontw.tex_height );
        printf( "  max_ascent:  %d\n", fontw.max_ascent );
        printf( "  max_descent: %d\n", fontw.max_descent );
        printf( "  num_glyphs:  %d\n", fontw.num_glyphs );
        printf( "]\n" );
    }

    fontw.tex_image = g_txf.buffer;
    fontw.write( outfile );
  
#ifdef DEBUG	
    fontw.dump( 256 );
#endif

#ifdef DISPLAY
    do_preview_txf( argc, argv );
#else
    free( g_txf.buffer );
#endif

    return 0;
}


/*EOF*/
