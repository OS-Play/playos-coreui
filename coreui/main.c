#define _POSIX_C_SOURCE 200112L
#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "server.h"



int main(int argc, char *argv[]) {
    wlr_log_init(WLR_DEBUG, NULL);
    char *startup_cmd = NULL;

    int c;
    while ((c = getopt(argc, argv, "s:h")) != -1) {
        switch (c) {
        case 's':
            startup_cmd = optarg;
            break;
        default:
            printf("Usage: %s [-s startup command]\n", argv[0]);
            return 0;
        }
    }
    if (optind < argc) {
        printf("Usage: %s [-s startup command]\n", argv[0]);
        return 0;
    }

    struct playos_server *server;
    server = playos_server_create();
    if (!server) {
        wlr_log(WLR_ERROR, "Failed to create playos_server.");
        return -1;
    }
	server->startup_cmd = startup_cmd;

    if (playos_server_init(server)) {
        wlr_log(WLR_ERROR, "Failed to init playos_server.");
        goto END;
    }

    playos_server_run(server);

END:
    playos_server_destroy(server);

    return 0;
}