#include "utils.h"

int g_verbose = 0;

char *g_program_name;

void program_name_initialize(char *argv0)
{
  char *result = basename(argv0);
  char *buf = strrchr(result, '.');
  if (buf != NULL) {
    int offset = buf - result;
    result[offset] = '\0';
  }
  g_program_name = strdup(result);
}

char * program_name_get()
{
  return g_program_name;
}

void program_name_finalize()
{
  free(g_program_name);	
}
