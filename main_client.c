#include <stdio.h>
#include <unistd.h>
#include "client.h"

int main(void) {
    load_library_client("libclient.so");
    
    // Ecris le premier message
    char msg[1024] = "abc";
    int temp = sndmsg(msg, 2000);
    printf("%d\n", temp);
    
    sleep(10);

    // Ecris le deuxieme message
    char msg2[1024] = "FINI";
    temp = sndmsg(msg2, 2000);
    printf("%d\n", temp);

    unload_library_client();
    return 0;
}