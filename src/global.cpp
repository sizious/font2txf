#include <assert.h>
#include "global.h"


/* Perform all clean-up when the program exits. */
void finalize()
{
    console.debug( "finalize fired!" );

    free( g_txf.buffer );
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
