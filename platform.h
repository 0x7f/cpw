#ifndef __CPW_PLATFORM_H__
#define __CPW_PLATFORM_H__

#include <stdint.h>
#include <stdio.h>

#ifdef _WIN32
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

typedef long ssize_t;
ssize_t getline(char** line, size_t *n, FILE*);

char* get_default_account_dir();
void copy_to_clipboard(const char*);

#endif
