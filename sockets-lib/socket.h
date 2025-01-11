#ifndef SOCKET
#define SOCKET
int passive_socket_init(const int port);
int passive_socket_wait_for_client(int passiveSocket);
void passive_socket_destroy(int socket);
 
int connect_to_server(const char * name, const int port);
void active_socket_destroy(int socket);

#endif // !SOCKET
