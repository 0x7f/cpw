#include "platform.h"

#include <stdlib.h>

#include <linux/limits.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xmu/Atoms.h>

char* get_default_account_dir(void) {
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

static size_t format_to_byte(int format) {
    switch (format) {
        case 8:
            return sizeof(char);
        case 16:
            return sizeof(short);
        case 32:
            return sizeof(long);
    }
    return 0;
}

void copy_to_clipboard(const char* str) {
    Display* display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Unable to open Display\n");
        exit(EXIT_FAILURE);
    }

    Atom clipboard_atom = XA_CLIPBOARD(display);
    Window window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, 1, 1, 0, 0, 0);
    int select_result = XSelectInput(display, window, PropertyChangeMask);
    Atom result_atom = XInternAtom(display, "FOR_GET_RESULT", False);
    int convert_result = XConvertSelection(display, clipboard_atom, XA_UTF8_STRING(display), result_atom, window, CurrentTime);

    XEvent event;
    int f = 0;
    for (int i = 0; i < 10; i++) {
        int result = XNextEvent(display, &event);
        if (event.type != SelectionNotify) {
            printf("event.type != SelectionNotify. type = %d.\n", event.type);
        } else {
            f = 1;
            break;
        }
    }

    if (!f)
        return;

    if (event.xselection.property == None) {
        printf("xselection.property == None\n");
        return;
    }

    Atom actual_type;
    int actual_format;
    unsigned long property_size, property_items;
    unsigned char* buffer;

    int get_property_result = XGetWindowProperty(display, window, result_atom, 0, 0, False, AnyPropertyType, &actual_type, &actual_format, &property_size, &property_items, &buffer);
    XFree(buffer);
    char* actual_type_name = XGetAtomName(display, actual_type);
    printf("actual_type_name = %s\n", actual_type_name);
    XFree(actual_type_name);
    printf("format = %d, property_size = %lu, property_items = %lu\n", actual_format, property_size, property_items);

    get_property_result = XGetWindowProperty(display, window, result_atom, 0, (long)property_items, False, AnyPropertyType, &actual_type, &actual_format, &property_size, &property_items, &buffer);

    printf("format = %d, property_size = %lu, property_items = %lu\n", actual_format, property_size, property_items);

    int delete_result = XDeleteProperty(display, window, result_atom);
    size_t property_buffer_size = property_items * format_to_byte(actual_format);
    printf("%s\n", buffer);
    XFree(buffer);
    XCloseDisplay(display);
}
