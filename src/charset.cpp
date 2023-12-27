#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <vector>

#include "utils.h"
#include "buildtxf.h"

bool loadCharCodesFile(const std::string& filename)
{
    FILE* fp;
    fp = fopen(filename.c_str(), "r");
    if (fp == NULL)
        return false;

    for (;;)
    {
        unsigned int i;
        if (fscanf(fp, " %x", &i) != 1)
            break;
        g_charCodes.insert(g_charCodes.end(), i);
    }

    fclose(fp);

    return true;
}
