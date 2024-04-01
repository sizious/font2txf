#ifndef __PREVIEW_H__
#define __PREVIEW_H__

#ifdef DISPLAY

#include <ft2build.h>
#include FT_FREETYPE_H
#include <GL/glut.h>

#include "global.h"

/* Display converted, output txf file */
void do_preview_txf( int argc, char* argv[] );

#endif

#endif /* __PREVIEW_H__ */
