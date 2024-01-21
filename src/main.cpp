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
#include "preview.h"

/* Characters to include in the TXF */
std::vector<wchar_t> g_char_codes;

/* Verbose switch */
bool g_verbose = true;

/* TXF data */
FT_Bitmap g_txf;

/* Console */
Console console;

/* Default characters to include in the TXF if nothing specified */
char _default_codes[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890abcdefghijklmnopqrstuvwxyz?.;,!*:\"/+-|'@#$%^&<>()[]{}_";


void usage()
{
    std::cout << PROGRAM_NAME << " version " << PROGRAM_VERSION << " (" << __DATE__ << ")\n\n";

    std::cout << "Convert a TrueType or OpenType font file <fontfile.ttf/otf> to a texture\n";
    std::cout << "mapped font (txf), the font format created by Mark J. Kilgard for GLUT.\n\n";

    std::cout << "Usage: " << program_name_get() << " [options] <fontfile.ttf/otf>\n\n";

    std::cout << "Options:\n";
    std::cout << "  -w <width>          Texture width  (default " << DEFAULT_FONT_WIDTH << ")\n";
    std::cout << "  -h <height>         Texture height (default " << DEFAULT_FONT_HEIGHT << ")\n";
/*
    std::cout << "  -b                  Create bitmap texture\n";
*/
    std::cout << "  -f <filename.txt>   File containing character codes to convert\n";
    std::cout << "  -c <string>         Characters to convert\n";
    std::cout << "  -g <gap>            Space between glyphs (default " << DEFAULT_FONT_GAP << ")\n";
    std::cout << "  -s <size>           Font point size (default " << DEFAULT_FONT_SIZE << ")\n";
    std::cout << "  -o <filename.txf>   Output file for textured font\n";
    std::cout << "  -q                  Quiet; no output\n";
#ifdef DISPLAY
    std::cout << "  -p                  Preview; display the output txf at the end of the conversion\n";
#endif
    std::cout << std::endl;
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
#ifdef DISPLAY
    bool preview_txf = false;
#endif    

    if( !initialize( argc, argv ) )
    {
        return EXIT_SUCCESS;
    }

    if( argc < 2 )
    {       
        usage();       
        return EXIT_SUCCESS;
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
                
                console.debug( "setting up codes: \"", codes, "\"" );
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
            else if( *cp == 'f' )
            {
                i++;
                if (i >= argc)
                    break;
                codesfile = argv[ i ];
            }
#ifdef DISPLAY            
            else if( *cp == 'p' )
            {
                i++;
                if (i >= argc)
                    break;
                preview_txf = true;      
            }
#endif            
        }
        else
        {
            infile = argv[ i ];
        }
    }
	
    /* Options "-c" and "-f" can't be mixed */
    if ( codesFromCmd && !codesfile.empty() )
	{
		console.error( "unable to use '-c' and '-f' options at the same time" );
        return EXIT_FAILURE;
	}

    /* Check if a input font has been passed */
    if( ! infile )
    {
        console.fatal( "unspecified input font file" );        
        return EXIT_FAILURE;
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
        if ( !load_charcodes_file( codesfile ) )
        {
            console.error( "cannot load specified character codes file" );
            return EXIT_FAILURE;
        }
    }
    else
    {
        int i = 0;
        while ( codes[i] != '\0' )
        {
            g_char_codes.insert( g_char_codes.end(), (wchar_t) codes[i] );
            i++;
        }
    }

    fontw.num_glyphs = build_txf( fontw, infile, g_char_codes, &g_txf, size, gap,
                                  asBitmap );

    if( ! fontw.num_glyphs )
	{
        console.error( "there is no glyphs in this font" );
        return EXIT_FAILURE;
	}

    if( g_verbose )
    {
        std::cout << "TexFont [\n";
        std::cout << "  format:      " << fontw.format << "\n";
        std::cout << "  tex_width:   " << fontw.tex_width << "\n";
        std::cout << "  tex_height:  " << fontw.tex_height << "\n";
        std::cout << "  max_ascent:  " << fontw.max_ascent << "\n";
        std::cout << "  max_descent: " << fontw.max_descent << "\n";
        std::cout << "  num_glyphs:  " << fontw.num_glyphs << "\n";
        std::cout << "]" << std::endl;
    }

    fontw.tex_image = g_txf.buffer;
    fontw.write( outfile );

#ifdef _DEBUG
#ifdef _FONT_DUMP_TO_CONSOLE
    fontw.dump_to_console();
#endif // _FONT_DUMP_TO_CONSOLE
#endif // _DEBUG

#ifdef DISPLAY
    if ( preview_txf )
    {
        console.log( "displaying preview..." );
        do_preview_txf( argc, argv );
    }
#endif

    return EXIT_SUCCESS;
}


/*EOF*/
