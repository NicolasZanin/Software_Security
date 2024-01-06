/* send message (maximum size: 1024 bytes) */
int sendmessage(char msg[1024], int port);

int start_server(int port);

int stop_server();

int getmessage(char msg_read[1024]);