#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#include <Shlobj.h>
#include <Shlwapi.h>
#include <Windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Error: you must provide a container name.\n");
        return EXIT_FAILURE;
    }

    const char *container = argv[1];

    WCHAR pathBuf[MAX_PATH];
    PWSTR path = &pathBuf[0];
    HRESULT res = SHGetKnownFolderPath(&FOLDERID_Documents, 0, NULL, &path);
    if (FAILED(res)) {
        fprintf(stderr, "Error while determining documents dir\n");
        return EXIT_FAILURE;
    }

    WCHAR containerPathBuf[MAX_PATH];
    PWSTR containerPath = &containerPathBuf[0];
    swprintf(containerPath, MAX_PATH - 1, L"Container\\%hs.tc", container);
    PathAppendW(path, containerPath);

    WCHAR cmdBuf[MAX_PATH + 32];
    PWSTR cmd = &cmdBuf[0];
    swprintf(cmd, MAX_PATH + 31, L"truecrypt /q /lz /v %s", path);

    int rc = _wsystem(cmd);
    if (rc < 0) {
        fprintf(stderr, "Error while mounting container file %s\n", container);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
