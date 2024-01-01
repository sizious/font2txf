/*============================================================================
//
// $Id: ttf2txf.cpp,v 1.11 2001/10/20 23:46:37 karl Exp $
//
// Converts TrueType, Type 1 and OpenType fonts to TXF texture font format.
// Uses FreeType 2.x.
//
//==========================================================================*/


/*
 TODO:
    * Pack glyphs more efficiently.
    * Save as bitmap.
*/

#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>
#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "txfbuild.h"
#include "charset.h"
#include "display.h"

/* Characters to include in the TXF */
std::vector<wchar_t> g_char_codes;

/* Verbose switch */
bool g_verbose = true;

/* TXF data */
FT_Bitmap g_txf;

/* Default characters to include in the TXF if nothing specified */
char _default_codes[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890abcdefghijklmnopqrstuvwxyz?.;,!*:\"/+-|'@#$%^&<>()[]{}_";


void usage()
{
    printf( "%s version %s (%s)\n\n", PROGRAM_NAME, PROGRAM_VERSION, __DATE__ );
    printf( "Usage: %s [options] <fontfile.ttf/otf>\n\n", program_name_get() );
    printf( "Options:\n" );
    printf( "  -w <width>          Texture width  (default %d)\n", DEFAULT_FONT_WIDTH );
    printf( "  -h <height>         Texture height (default %d)\n", DEFAULT_FONT_HEIGHT );
/*
    printf( "  -b                  Create bitmap texture\n" );
*/
    printf( "  -f <filename.txt>   File containing character codes to convert\n");
    printf( "  -c <string>         Characters to convert\n" );
    printf( "  -g <gap>            Space between glyphs (default %d)\n", DEFAULT_FONT_GAP );
    printf( "  -s <size>           Font point size (default %d)\n", DEFAULT_FONT_SIZE );
    printf( "  -o <filename.txf>   Output file for textured font\n" );
    printf( "  -q                  Quiet; no output\n" );

    finalize();
}


int main( int argc, char* argv[] )
{
    TexFontWriter fontw;
    int i;
    int gap = DEFAULT_FONT_GAP;
    int size = DEFAULT_FONT_SIZE;
    bool asBitmap = false;
	bool codesFromCmd = false;
    char* infile = 0;
    char outfile[ FILENAME_MAX ];
    std::string codesfile;
    char* codes = _default_codes;

    initialize( argc, argv );

    if( argc < 2 )
    {        
        usage();       
        return 0;
    }

    outfile[ 0 ] = '\0';

    fontw.format     = TexFontWriter::TXF_FORMAT_BYTE;
    fontw.tex_width  = DEFAULT_FONT_WIDTH;
    fontw.tex_height = DEFAULT_FONT_HEIGHT;

    /* Options parsing */
    for( i = 1; i < argc; i++ )
    {
        if( *argv[i] == '-' )
        {
            char* cp = ( argv[ i ] + 1 );

            if( *cp == 'w' )
            {
                i++;
                if( i >= argc )
                    break;
                fontw.tex_width = atoi( argv[ i ] );
            }
            else if( *cp == 'h' )
            {
                i++;
                if( i >= argc )
                    break;
                fontw.tex_height = atoi( argv[ i ] );
            }
            else if( *cp == 'c' )
            {
                i++;
                if( i >= argc )
                    break;
                codes = argv[ i ];
                codesFromCmd = true;
#ifdef _DEBUG
                printf( "codes: %s\n", codes );
#endif
            }
/*
            else if( *cp == 'b' )
            {
                asBitmap = true;
            }
*/
            else if( *cp == 'g' )
            {
                i++;
                if( i >= argc )
                    break;
                gap = atoi( argv[ i ] );
            }
            else if( *cp == 's' )
            {
                i++;
                if( i >= argc )
                    break;
                size = atoi( argv[ i ] );
            }
            else if( *cp == 'o' )
            {
                i++;
                if( i >= argc )
                    break;
                strcpy( outfile, argv[ i ] );
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
                codesfile = argv[ i ];
            }
        }
        else
        {
            infile = argv[ i ];
        }
    }
	
    /* Options "-c" and "-f" can't be mixed */
    if ( codesFromCmd && !codesfile.empty() )
	{
		printf("no------------------");
		usage();
		exit( -2 );
	}	

    /* Check if a input font has been passed */
    if( ! infile )
    {
        usage();
        exit( -1 );
    }

    /* Set outfile to base infile and append ".txf" */
    if( outfile[ 0 ] == '\0' )
    {
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
	{
        finalize();
        return -1;
	}

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

#ifdef _DEBUG
    fontw.dump_to_console();
#endif

//	func("1", 2, 3);
	
#ifdef DISPLAY
    do_preview_txf( argc, argv );
#else
    finalize();
#endif

    return 0;
}


/*EOF*/
