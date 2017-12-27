#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __linux__
#include <linux/limits.h>
#define MAX_PATH PATH_MAX
#endif

#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;

ssize_t getline(char **lineptr, size_t *n, FILE *stream) {
    char *bufptr = NULL;
    char *p = bufptr;
    ssize_t size;
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
#endif

static void copy_to_clipboard(char* str) {
#ifdef _WIN32
    const size_t len = strlen(str) + 1;
    HGLOBAL hMem =  GlobalAlloc(GMEM_MOVEABLE, len);
    memcpy(GlobalLock(hMem), str, len);
    GlobalUnlock(hMem);
    OpenClipboard(0);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();
#endif
}

int main(int argc, char** argv) {
    char* ACCOUNT_DIR = "Z:\\Accounts";
    char* PASSWORD_PREFIX = "password: ";
    size_t PASSWORD_PREFIX_LEN = strlen(PASSWORD_PREFIX);

    if (argc != 2) {
        fprintf(stderr, "Error: you must provide an account name.\n");
        return EXIT_FAILURE;
    }

    char* account = argv[1];
    char* account_file = (char*)malloc(MAX_PATH);
    snprintf(account_file, MAX_PATH - 1, "%s\\%s.txt", ACCOUNT_DIR, account);
    fprintf(stdout, "Trying to open %s\n", account_file);

    FILE* fp = fopen(account_file, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: account \"%s\" not found.\n", account);
        return EXIT_FAILURE;
    }

    ssize_t read;
    size_t len = 0;
    char* line = NULL;
    char* password = NULL;
    while ((read = getline(&line, &len, fp)) != -1) {
        if (strncmp(line, PASSWORD_PREFIX, PASSWORD_PREFIX_LEN) != 0) {
            continue;
        }

        password = line + PASSWORD_PREFIX_LEN;
        break; // stop when first password found
    }

    fclose(fp);

    if (!password || !strlen(password)) {
        fprintf(stderr, "Account file does not contain a password.\n");
        return EXIT_FAILURE;
    }

    password[strlen(password) - 1] = '\0'; // remove trailing newline added by getline
    copy_to_clipboard(password);

    return EXIT_SUCCESS;
}
