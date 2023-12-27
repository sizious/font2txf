#include "utils.h"

char* g_program_name;

void program_name_initialize(char* argv0)
{
    char* result = basename( argv0 );
    char* buf = strrchr( result, '.' );
    if ( buf != NULL )
	{
        int offset = buf - result;
        result[offset] = '\0';
    }
    g_program_name = strdup( result );
}

char* program_name_get()
{
    return g_program_name;
}

void program_name_finalize()
{
    free( g_program_name );	
}

const char* bool_to_str( bool b )
{
    return ( b ? "true" : "false" );
}
