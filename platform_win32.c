#include "platform.h"

#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>

char* get_default_account_dir() {
    static char buf[FILENAME_MAX];
    const char* home = getenv("USERPROFILE");
    const char* dir ="\\docs\\Accounts\\";
    if (strlen(home) + strlen(dir) + 1 > FILENAME_MAX) {
        fprintf(stderr, "USERPROFILE too long\n");
        exit(EXIT_FAILURE);
    }
    strcpy(buf, home);
    strcat(buf, dir);
    return buf;
}

ssize_t getline(char **lineptr, size_t *n, FILE *stream) {
    char *bufptr = NULL;
    char *p = bufptr;
    int64_t size;
    int c;

    if (lineptr == NULL) {
        return -1;
    }
    if (stream == NULL) {
        return -1;
    }
    if (n == NULL) {
        return -1;
    }
    bufptr = *lineptr;
    size = *n;

    c = fgetc(stream);
    if (c == EOF) {
        return -1;
    }
    if (bufptr == NULL) {
        bufptr = malloc(128);
        if (bufptr == NULL) {
            return -1;
        }
        size = 128;
    }
    p = bufptr;
    while(c != EOF) {
        if ((p - bufptr) > (size - 1)) {
            size = size + 128;
            bufptr = realloc(bufptr, size);
            if (bufptr == NULL) {
                return -1;
            }
        }
        *p++ = c;
        if (c == '\n') {
            break;
        }
        c = fgetc(stream);
    }

    *p++ = '\0';
    *lineptr = bufptr;
    *n = size;

    return p - bufptr - 1;
}

void copy_to_clipboard(const char* str) {
    const size_t len = strlen(str) + 1;
    HGLOBAL hMem =  GlobalAlloc(GMEM_MOVEABLE, len);
    memcpy(GlobalLock(hMem), str, len);
    GlobalUnlock(hMem);
    OpenClipboard(0);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();
}
