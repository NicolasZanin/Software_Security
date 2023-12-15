#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

static void (*start_server)(int port);
static void (*stop_server)();
static void (*get_msg)(char message[1024]);

static void *library;

void load_library_server(char *library_name) {
    library = dlopen(library_name, RTLD_LAZY);

    if (!library) {
        printf("faux\n");
        exit(1);
    }

    start_server = dlsym(library, "startserver");
    stop_server = dlsym(library, "stopserver");
    get_msg = dlsym(library, "getmsg");
}

void unload_library_server() {
    if (!library)
        dlclose(library);
}

int startserver(int port) {
    start_server(port);
    return port;
}

int stopserver() {
    stop_server();
    return 0;
}

int getmsg(char msg_read[1024]) {
    get_msg(msg_read);
    return strlen(msg_read);
}