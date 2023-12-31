#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <ft2build.h>
#include FT_FREETYPE_H

#include <vector>

#include "utils.h"

#define PROGRAM_NAME "font2txf"

extern std::vector<wchar_t> g_char_codes;

extern bool g_verbose;

extern FT_Bitmap g_txf;

void initialize( int argc, char* argv[] );
void finalize();

#endif /* __GLOBAL_H__ */
