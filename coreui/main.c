#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "coreui_server.h"


int main(int argc, char *argv[]) {
    wlr_log_init(WLR_DEBUG, NULL);
    char *startup_cmd = NULL;

    // TODO: for vmwgfx drm driver
    setenv("WLR_NO_HARDWARE_CURSORS", "1", 1);

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

    struct coreui_server *server;
    server = coreui_server_create();
    if (!server) {
        wlr_log(WLR_ERROR, "Failed to create coreui_server.");
        return -1;
    }
    server->startup_cmd = startup_cmd;

    if (coreui_server_init(server)) {
        wlr_log(WLR_ERROR, "Failed to init coreui_server.");
        goto END;
    }

    coreui_server_run(server);

END:
    coreui_server_destroy(server);
    return 0;
}