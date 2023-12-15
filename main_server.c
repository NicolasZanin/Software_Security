#include "server.h"
#include <stdio.h>
#include <string.h>

int main(void) {
    load_library_server("libserver.so");
    int temp = startserver(2000);
    printf("Port : %d\n", temp);
    
    while(1) {
        char message[1024];
        int test = getmsg(message);

        // Si le message est FINI arrête le server
        if (strcmp("FINI", message) == 0)
            break;
        
        // Affiche la taille du message et le message
        printf("%d : %s\n", test, message);
    }

    stopserver();
    unload_library_server();
    return 0;
}