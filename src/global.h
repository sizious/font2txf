#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <ft2build.h>
#include FT_FREETYPE_H

#include <vector>

#include "utils.hpp"

#define PROGRAM_NAME "font2txf"

#ifndef PROGRAM_VERSION
#define PROGRAM_VERSION "(undefined)"
#endif

#define DEFAULT_FONT_GAP 1
#define DEFAULT_FONT_SIZE 20
#define DEFAULT_FONT_HEIGHT 256
#define DEFAULT_FONT_WIDTH 256

extern std::vector<wchar_t> g_char_codes;

extern bool g_verbose;

extern FT_Bitmap g_txf;

void initialize( int argc, char* argv[] );
void finalize();

#endif /* __GLOBAL_H__ */
