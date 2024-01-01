#include <assert.h>

#include "global.h"

void initialize( int argc, char* argv[] )
{
    assert( argc );
    program_name_initialize( argv[0] );

//	func("1xxx", 2, 3);
		
    if ( std::atexit( finalize ) )
    {
        // std::cerr << "Registration failed!\n";
//        return EXIT_FAILURE;
    }
}

void finalize()
{
    free( g_txf.buffer );

    program_name_finalize();
}
