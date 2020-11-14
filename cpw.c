#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "platform.h"

#define OPTPARSE_IMPLEMENTATION
#define OPTPARSE_API static
#include "optparse.h"

static void parse_args(int argc, char** argv, char** dir, char** key, char** account) {
    int option;
    struct optparse options;
    optparse_init(&options, argv);
    while ((option = optparse(&options, "d:k:")) != -1) {
        switch (option) {
        case 'd':
            *dir = options.optarg;
            break;
        case 'k':
            *key = options.optarg;
            break;
        case '?':
            fprintf(stderr, "%s: %s\n", argv[0], options.errmsg);
            exit(EXIT_FAILURE);
        }
    }

    *account = optparse_arg(&options);
    if (!*account) {
        fprintf(stderr, "Error: you must provide an account name.\n");
        exit(EXIT_FAILURE);
    }

    // check for further args
    if (optparse_arg(&options)) {
        fprintf(stderr, "Error: you must specify exactly one account name.\n");
        exit(EXIT_FAILURE);
    }
}

static void build_path(char* fname, size_t size, const char* dir, const char* account) {
    size_t dirlen = strlen(dir);
    if (size < dirlen + 1 + strlen(account) + 4 + 1) {
        fprintf(stderr, "Path too long\n");
        exit(EXIT_FAILURE);
    }

    memset(fname, '\0', size);
    strcpy(fname, dir);
    fname[dirlen] = PATH_SEPARATOR;
    strcat(fname, account);
    strcat(fname, ".txt");
}

static int read_value(char* fname, char* key, char* buf, size_t buf_size) {
    FILE* fp = fopen(fname, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error opening file \"%s\"\n", fname);
        return EXIT_FAILURE;
    }

    int found = 0;
    int64_t read;
    size_t len = 0;
    char* line = NULL;
    size_t prefix_len = strlen(key) + 2;
    char* prefix = calloc(1, prefix_len + 1);
    strcpy(prefix, key);
    strcat(prefix, ": ");

    while ((read = getline(&line, &len, fp)) != -1) {
        if (strncmp(line, prefix, prefix_len) != 0) {
            continue;
        }

        found = 1;
        strncpy(buf, line + prefix_len, buf_size);

        // remove trailing newline added by getline
        size_t last = strlen(buf) - 1;
        while (last >= 0 && (buf[last] == '\n' || buf[last] == '\r')) {
            buf[last] = '\0';
            --last;
        }

        // stop when first password found
        break;
    }

    if (line) {
        free(line);
    }
    free(prefix);

    fclose(fp);

    return found ? EXIT_SUCCESS : EXIT_FAILURE;
}

int main(int argc, char** argv) {
    int res;
    char fname[FILENAME_MAX];
    char value[4096];
    char* dir = get_default_account_dir();
    char* key = "password";
    char* account = NULL;

    parse_args(argc, argv, &dir, &key, &account);

    build_path(fname, FILENAME_MAX, dir, account);

    fprintf(stdout, "Reading \"%s\" from %s\n", key, fname);

    res = read_value(fname, key, value, 4096);
    if (res) {
        fprintf(stderr, "Error while reading \"%s\" from file \"%s\"\n", key, fname);
        return EXIT_FAILURE;
    }

    if (!strlen(value)) {
        fprintf(stderr, "Account file does not contain the requested key \"%s\".\n", key);
        return EXIT_FAILURE;
    }

    copy_to_clipboard(value);

    return EXIT_SUCCESS;
}
