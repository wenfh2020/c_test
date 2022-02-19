#include "server.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ae.h"
#include "anet.h"

#define PORT 12345
#define SERVER_IP "127.0.0.1"
#define TCP_BACK_LOG 512
#define MAX_CLIENTS 10000

#define CONFIG_MIN_RESERVED_FDS 32
#define CONFIG_FDSET_INCR (CONFIG_MIN_RESERVED_FDS + 96)

struct redis_server server; /* Server global state */

void init_server() {
    server.el = ae_create_event_loop(MAX_CLIENTS + CONFIG_FDSET_INCR);
    if (server.el == NULL) {
        printf("anet_tcp_server fail, err: %s\n", strerror(errno));
        exit(1);
    }

    server.sock_fd = anet_tcp_server(server.neterr, PORT, (char *)SERVER_IP, TCP_BACK_LOG);
    if (server.sock_fd < 0) {
        printf("anet_tcp_server fail, err: %s\n", server.neterr);
        exit(1);
    }
    anet_non_lock(NULL, server.sock_fd);

    ae_create_file_event(server.el, server.sock_fd, AE_READABLE,
                         accept_tcp_handler, NULL);
}

int main(int argc, char **argv) {
    init_server();
    ae_main(server.el);
    ae_delete_event_loop(server.el);
    close(server.sock_fd);
    printf("close fd: %d\n", server.sock_fd);
    return 0;
}
