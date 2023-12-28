#include "global.h"

void initialize( int argc, char* argv[] )
{
	program_name_initialize( argv[0] );
}

void finalize()
{
	free( g_txf.buffer );
	
	program_name_finalize();
}
