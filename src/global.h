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

/* Default Charset Codes */
#define DEFAULT_CHARCODES_POS0_SPC " "
#define DEFAULT_CHARCODES_POS1_AZU "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define DEFAULT_CHARCODES_POS2_NUM "1234567890"
#define DEFAULT_CHARCODES_POS3_AZL "abcdefghijklmnopqrstuvwxyz"
#define DEFAULT_CHARCODES_POS4_SYM "?.;,!*:\"/+-|'@#$%^&<>()[]{}_"

/* Default Charset Codes for the help screen */
#define DEFAULT_CHARCODES "(space)A..Z1..9a..z" DEFAULT_CHARCODES_POS4_SYM

extern char g_default_char_codes[];

extern std::vector<wchar_t> g_char_codes;

extern bool g_verbose;

extern FT_Bitmap g_txf;

extern Console console;

bool initialize( int argc, char* argv[] );

#endif /* __GLOBAL_H__ */
