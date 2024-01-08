#include "../serverclient/client_server.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc > 1) {
        printf("Message envoyé : %s\n", argv[1]);
        sendmessage(argv[1], 2000);
    }
    return 0;
}

