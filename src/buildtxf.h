#ifndef __BUILDTXF_H__
#define __BUILDTXF_H__

#include "global.h"
#include "fntwrite.h"

/**
  Returns number of glyphs added or zero if fails.
*/

int build_txf(TexFontWriter& fontw,
         const char* file,
         const std::vector<wchar_t>& codes,
         FT_Bitmap* img,
         int psize,
         int gap,
         bool asBitmap);
		 
void txf_dump_image( unsigned char* buf, int pitch, int w, int h, bool fit = false );
	 
#endif /* __BUILDTXF_H__ */
