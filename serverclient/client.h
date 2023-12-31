/* send message (maximum size: 1024 bytes) */
int sndmsg(char msg[1024], int port); 

/* load et unload library */
void load_library_client(char *name);
void unload_library_client();