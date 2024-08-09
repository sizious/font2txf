#include "charset.h"

bool load_charcodes_file( const std::string& filename )
{
    FILE* fp;
    fp = fopen( filename.c_str(), "r" );
    if ( fp == NULL )
        return false;

    for (;;)
    {
        unsigned int i;
        if ( fscanf( fp, " %x", &i ) != 1 )
            break;
        g_char_codes.insert( g_char_codes.end(), i );
    }

    fclose( fp );

    LOG( "loaded ", g_char_codes.size(), " character(s) from charset text file: \"", filename, "\"" );

    return ( g_char_codes.size() > 0 );
}
