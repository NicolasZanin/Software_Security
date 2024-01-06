#include "client.h"
#include "server.h"
#include "client_server.h"
#include <string.h>

int sendmessage(char msg[1024], int port){
    sndmsg(msg, port);
    return strlen(msg);
}

int start_server(int port) {
    startserver(port);
    return port;
}

int stop_server() {
    stopserver();
    return 0;
}

int getmessage(char msg_read[1024]) {
    getmsg(msg_read);
    return strlen(msg_read);
}