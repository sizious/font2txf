#include <filesystem>
#include <libgen.h>

#include "utils.hpp"

std::string g_program_name;

void program_name_initialize( char* argv0 )
{
    std::string program_filename(basename( argv0 ) );
    size_t dotchr = program_filename.find_last_of(".");
    g_program_name = ( dotchr == std::string::npos ) ? program_filename : program_filename.substr(0, dotchr);
}

std::string program_name_get()
{
    return g_program_name;
}

std::string bool_to_str( bool b )
{
    return ( b ? "true" : "false" );
}
