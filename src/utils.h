#ifndef __UTILS_H__
#define __UTILS_H__

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <libgen.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

void program_name_initialize(char *argv0);
char * program_name_get();
void program_name_finalize();

#endif /* __UTILS_H__ */
