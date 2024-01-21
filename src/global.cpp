#include <assert.h>

#include "global.h"


void finalize()
{
#ifdef _DEBUG
    console.log( "finalize fired!" );
#endif // _DEBUG

    free( g_txf.buffer );
}


bool initialize( int argc, char* argv[] )
{
    assert( argc );
    program_name_initialize( argv[0] );

    if ( std::atexit( finalize ) )
    {
        console.error( "initialize: atexit() registration failed!" );
        return false;
    }

    return true;
}
