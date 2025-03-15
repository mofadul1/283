#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>
#include <getopt.h>
#include "dshlib.h"
#include "rshlib.h"

#define MODE_LCLI   0  // Local client
#define MODE_SCLI   1  // Socket client
#define MODE_SSVR   2  // Socket server

typedef struct cmd_args {
    int mode;
    char ip[16];   // e.g., 192.168.100.101\0
    int port;
    int threaded_server;
} cmd_args_t;

void print_usage(const char *progname) {
    printf("Usage: %s [-c | -s] [-i IP] [-p PORT] [-x] [-h]\n", progname);
    printf("  Default is to run %s in local mode\n", progname);
    printf("  -c            Run as client\n");
    printf("  -s            Run as server\n");
    printf("  -i IP         Set IP/Interface address (only valid with -c or -s)\n");
    printf("  -p PORT       Set port number (only valid with -c or -s)\n");
    printf("  -x            Enable threaded mode (only valid with -s)\n");
    printf("  -h            Show this help message\n");
    exit(0);
}

void parse_args(int argc, char *argv[], cmd_args_t *cargs) {
    int opt;
    memset(cargs, 0, sizeof(cmd_args_t));
    cargs->mode = MODE_LCLI;
    cargs->port = RDSH_DEF_PORT;

    while ((opt = getopt(argc, argv, "csi:p:xh")) != -1) {
        switch (opt) {
            case 'c': cargs->mode = MODE_SCLI; strcpy(cargs->ip, RDSH_DEF_CLI_CONNECT); break;
            case 's': cargs->mode = MODE_SSVR; strcpy(cargs->ip, RDSH_DEF_SVR_INTFACE); break;
            case 'i': strcpy(cargs->ip, optarg); break;
            case 'p': cargs->port = atoi(optarg); break;
            case 'x': cargs->threaded_server = 1; break;
            case 'h': print_usage(argv[0]); break;
            default: print_usage(argv[0]);
        }
    }
}

int main(int argc, char *argv[]) {
    cmd_args_t cargs;
    parse_args(argc, argv, &cargs);

    switch (cargs.mode) {
        case MODE_LCLI: return exec_local_cmd_loop();
        case MODE_SCLI: return exec_remote_cmd_loop(cargs.ip, cargs.port);
        case MODE_SSVR: return start_server(cargs.ip, cargs.port, cargs.threaded_server);
        default: fprintf(stderr, "Invalid mode\n"); exit(EXIT_FAILURE);
    }
}

