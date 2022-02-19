#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <sys/socket.h>
#include <sys/uio.h>

#include "anet.h"
#include "server.h"

#define MAX_ACCEPTS_PER_CALL 1000

void accept_tcp_handler(ae_event_loop *el, int fd, void *privdata, int mask) {
    int cport, cfd, max = MAX_ACCEPTS_PER_CALL;
    char cip[NET_IP_STR_LEN];
    UNUSED(el);
    UNUSED(mask);
    UNUSED(privdata);

    while (max--) {
        cfd = anet_tcp_accept(server.neterr, fd, cip, sizeof(cip), &cport);
        if (cfd == ANET_ERR) {
            if (errno != EWOULDBLOCK)
                printf("Accepting client connection: %s", server.neterr);
            return;
        }

        printf("accepted %s:%d\n", cip, cport);
        // serverLog(LL_VERBOSE, "Accepted %s:%d", cip, cport);
        // acceptCommonHandler(connCreateAcceptedSocket(cfd), 0, cip);
    }
}