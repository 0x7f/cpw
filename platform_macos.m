#include "platform.h"

#include <AppKit/AppKit.h>
#include <sys/syslimits.h>

char* get_default_account_dir() {
    static char buf[FILENAME_MAX];
    const char* home = getenv("HOME");
    const char* dir ="/docs/Accounts/";
    if (strlen(home) + strlen(dir) + 1 > FILENAME_MAX) {
        fprintf(stderr, "HOME too long\n");
        exit(EXIT_FAILURE);
    }
    strcpy(buf, home);
    strcat(buf, dir);
    return buf;
}

void copy_to_clipboard(const char* str) {
    NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
    [pasteboard clearContents];
    [pasteboard setString:@(str) forType:NSPasteboardTypeString];
}

