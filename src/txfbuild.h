#ifndef __TXFBUILD_H__
#define __TXFBUILD_H__

#include "global.h"
#include "txfwrite.h"

/* Build the TXF (textured font).
 * Returns number of glyphs added or zero if fails.
 * If glyphs < 0, it means conversion happened with errors/warnings. */
int build_txf(TexFontWriter& fontw,
         const char* file,
         const std::vector<wchar_t>& codes,
         FT_Bitmap* img,
         int psize,
         int gap,
         bool asBitmap);

#endif /* __TXFBUILD_H__ */
