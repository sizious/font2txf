#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <vector>

#include <GL/glut.h>

#include "utils.h"
#include "buildtxf.h"
#include "charset.h"

void do_preview_txf( int argc, char* argv[] );
		 
#endif /* __DISPLAY_H__ */
