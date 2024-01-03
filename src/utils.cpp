#include <filesystem>
#include <libgen.h>

#include "utils.hpp"

std::string g_program_name;
Console g_console;

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

const char* bool_to_str( bool b )
{
    return ( b ? "true" : "false" );
}


void halt()
{
    g_console.log("xxx", "log", 0);
    g_console.warn("xxx", "warn", 1);    
    g_console.error("xxx", "error", 2);    
    g_console.fatal("xxx", "error", 2);
//    std::cerr << "Registration failed!" << std::endl;
//	exit( EXIT_FAILURE );
}
