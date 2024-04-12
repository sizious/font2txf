/*============================================================================
//
// $Id: ttf2txf.cpp,v 1.11 2001/10/20 23:46:37 karl Exp $
//
// Converts TrueType, Type 1 and OpenType fonts to TXF texture font format.
// Uses FreeType 2.x.
//
//==========================================================================*/


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


/* Display the header... */
void header()
{
    std::cout << PROGRAM_NAME << " version " << PROGRAM_VERSION << " (" << __DATE__ << ")\n\n";
}

/* Display help/usage of this tool. */
void usage()
{
    header();

    std::cout << "Converts a TrueType/OpenType font file <fontfile.ttf/otf> to a texture mapped\n";
    std::cout << "font (TXF), the font format created by Mark J. Kilgard for the OpenGL Utility\n";
    std::cout << "Toolkit (GLUT).  This tool is a heavily modified version of \"ttf2txf\"\n";
    std::cout << "originally written by Chris Laurel for the Celestia project.\n\n";    

    std::cout << "Usage: " << program_name_get() << " [options] <fontfile.ttf/otf>\n\n";

    std::cout << "Default character set (see `-c` or `-f` options below): " << "\n";
    std::cout << "  " << DEFAULT_CHARCODES << "\n\n";
//    std::cout << "... where " << DEFAULT_CHARCODES_POS1_AZU_SHORT << ": " << DEFAULT_CHARCODES_POS1_AZU << "\n";
//    std::cout << "      and " << DEFAULT_CHARCODES_POS3_AZL_SHORT << ": " << DEFAULT_CHARCODES_POS3_AZL << "\n\n";

    std::cout << "Options:\n";
    std::cout << "  -w <width>         Texture width (default: " << DEFAULT_FONT_WIDTH << ")\n";
    std::cout << "  -e <height>        Texture height (default: " << DEFAULT_FONT_HEIGHT << ")\n";
    std::cout << "                     This option may also be mapped to `-h` for compatibility\n";
#if 0 /* Disabled for now */
    std::cout << "  -b                 Create bitmap texture\n";
#endif
    std::cout << "  -c <string>        Characters to convert; read directly from command-line\n";
    std::cout << "                     Using this option will override default character set\n";    
    std::cout << "                     Note: This option cannot be mixed with `-f`\n";
    std::cout << "  -f <filename.txt>  Text file containing the character codes to convert\n";
    std::cout << "                     Using this option will override default character set\n";
    std::cout << "                     Note: This option cannot be mixed with `-c`\n";
    std::cout << "  -g <gap>           Space between glyphs (default: " << DEFAULT_FONT_GAP << ")\n";
    std::cout << "  -s <size>          Font point size (default: " << DEFAULT_FONT_SIZE << ")\n";
    std::cout << "  -o <filename.txf>  Output file for textured font (default: <fontfile>.txf)\n";
    std::cout << "  -q                 Quiet; no output\n";
#ifdef DISPLAY
    std::cout << "  -p                 Preview; display the txf output at the end of the process\n";
#endif
	std::cout << "  -h                 Usage information (you're looking at it)";

    std::cout << std::endl;
}


/* Entry point */
int main( int argc, char* argv[] )
{
    TexFontWriter fontw;
    int i;
    int gap = DEFAULT_FONT_GAP;
    int size = DEFAULT_FONT_SIZE;
    bool asBitmap = false;
	bool codes_from_cmd = false;
    bool h_switch = false;
    char* infile = 0;
    char outfile[ FILENAME_MAX ];
    std::string codesfile;
    char* codes = g_default_char_codes;
#ifdef DISPLAY
    bool preview_txf = false;
#endif    

    if( !initialize( argc, argv ) )
    {
        return EXIT_FAILURE;
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

    /* Simple options parsing */
    for( i = 1; i < argc; i++ )
    {
        if( *argv[i] == '-' )
        {
            char* cp = ( argv[ i ] + 1 );
            
            if( *cp == 'w' )
            {
                /* Width */                
                i++;
                if( i >= argc )
                    break;
                fontw.tex_width = atoi( argv[ i ] );
            }
            else if( *cp == 'e' || *cp == 'h' )
            {
                /* Height ("e" but could be "h" for compatibility) */
                
                /* Handle compatibility */
                h_switch = true;
                
                i++;
                if( i >= argc )
                    break;
                fontw.tex_height = atoi( argv[ i ] ); 

                /* Reevaluate h_switch; if tex_height > 0, then finally it's "height" */
                h_switch = ( ! fontw.tex_height );
            }
            else if( *cp == 'c' )
            {
                /* Characters to convert (from command-line) */
                i++;
                if( i >= argc )
                    break;
                codes = argv[ i ];
                codes_from_cmd = true;
                DEBUG( "setting up codes: \"", codes, "\"" );
            }
#if 0
            else if( *cp == 'b' )
            {
                /* Bitmap texture */
                asBitmap = true;
            }
#endif
            else if( *cp == 'g' )
            {
                /* Spaces between glyphs (gap) */
                i++;
                if( i >= argc )
                    break;
                gap = atoi( argv[ i ] );
            }
            else if( *cp == 's' )
            {
                /* Font point size */
                i++;
                if( i >= argc )
                    break;
                size = atoi( argv[ i ] );
            }
            else if( *cp == 'o' )
            {
                /* Output txf filename */
                i++;
                if( i >= argc )
                    break;
                strcpy( outfile, argv[ i ] );
            }
            else if( *cp == 'q' )
            {
                /* Disable verbosity */
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
                preview_txf = true;      
            }
#endif            
        }
        else
        {
            /* Input ttf/otf file */
            infile = argv[ i ];
        }
    }

    /* Display help info, if no input file is provided and "-h" is used "alone" (without "-w") */
    if( h_switch && ! infile )
    {
        usage();
        return EXIT_SUCCESS;
    }

    if ( g_verbose )
    {
        header();
    }

    /* Check if a input font has been passed */
    if( ! infile )
    {
        FATAL( "unspecified input font file" );        
        return EXIT_FAILURE;
    }

    /* Check if input file is provided */
    if( ! file_exists(infile) )
    {        
        FATAL( "input file not found" );
        return EXIT_FAILURE;
    }
	
    /* Options "-c" and "-f" can't be mixed */
    if( codes_from_cmd && ! codesfile.empty() )
	{
		ERR( "unable to use '-c' and '-f' options at the same time" );
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
            FATAL( "cannot load specified character codes file" );
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
        FATAL( "there is no glyphs in this font" );
        return EXIT_FAILURE;
	}


    fontw.display_info();

    fontw.tex_image = g_txf.buffer;
    fontw.write( outfile );

#ifdef _DEBUG
#ifdef _DEBUG_FONT_DUMP_TO_CONSOLE
    fontw.dump_to_console();
#endif // _DEBUG_FONT_DUMP_TO_CONSOLE
#endif // _DEBUG

#ifdef DISPLAY
    if ( preview_txf )
    {
        LOG( "displaying preview..." );
        do_preview_txf( argc, argv );
    }
#endif

    return EXIT_SUCCESS;
}


/*EOF*/
