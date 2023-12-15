#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

static void (*snd_msg)(char msg[1024], int port);
static void *library;

void load_library_client(char *library_name) {
    library = dlopen(library_name, RTLD_LAZY);

    if (!library) {
        printf("faux\n");
        exit(1);
    }

    snd_msg = dlsym(library, "sndmsg");
}

void unload_library_client() {
    if (!library)
        dlclose(library);
}

int sndmsg(char msg[1024], int port){
    snd_msg(msg, port);
    return (int) strlen(msg);
}