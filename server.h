int startserver(int port);
int stopserver();

/* read message sent by client */
int getmsg(char msg_read[1024]);

/* load et unload le server */
void load_library_server(char *name);
void unload_library_server();
