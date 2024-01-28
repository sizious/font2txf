#include <assert.h>
#include "global.h"

/* Characters to include in the TXF */
std::vector<wchar_t> g_char_codes;

/* Verbose switch */
bool g_verbose = true;

/* TXF data */
FT_Bitmap g_txf;

/* Console */
Console console;

/* Perform all clean-up when the program exits. */
void finalize()
{
    console.debug( "finalize fired!" );

    if( g_txf.buffer )
    {        
        free( g_txf.buffer );
        g_txf.buffer = nullptr;
    }
}


/* Initialize some global variables and stuff. */
bool initialize( int argc, char* argv[] )
{
    assert( argc );
    program_name_initialize( argv[0] );

    if ( std::atexit( finalize ) )
    {
        console.fatal( "initialize: atexit() registration failed!" );
        return false;
    }

    return true;
}
