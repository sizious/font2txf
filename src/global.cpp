#include <assert.h>

#include "global.h"

void initialize( int argc, char* argv[] )
{
    assert( argc );
    program_name_initialize( argv[0] );

    if ( std::atexit( finalize ) )
    {
//        std::cerr << "Registration failed!" << std::endl;
//        return EXIT_FAILURE;
    }
}

void finalize()
{
    free( g_txf.buffer );

    program_name_finalize();
}
