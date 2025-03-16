#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <fcntl.h>

//INCLUDES for extra credit
//#include <signal.h>
//#include <pthread.h>
//-------------------------

#include "dshlib.h"
#include "rshlib.h"


int start_server(char *ifaces, int port, int is_threaded) {
	(void)is_threaded;
	int svr_socket = boot_server(ifaces, port);
    if (svr_socket < 0) return svr_socket;

    int rc = process_cli_requests(svr_socket);
    stop_server(svr_socket);
    return rc;
}

int boot_server(char *ifaces, int port) {
    struct sockaddr_in addr;
    int svr_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (svr_socket < 0) return ERR_RDSH_COMMUNICATION;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ifaces, &addr.sin_addr);

    if (bind(svr_socket, (struct sockaddr*)&addr, sizeof(addr)) < 0 || listen(svr_socket, 20) < 0) {
        close(svr_socket);
        return ERR_RDSH_COMMUNICATION;
    }

    return svr_socket;
}

int process_cli_requests(int svr_socket) {
    while (1) {
        int cli_socket = accept(svr_socket, NULL, NULL);
        if (cli_socket < 0) return ERR_RDSH_COMMUNICATION;

        int rc = exec_client_requests(cli_socket);
        if (rc == OK_EXIT) break;
    }
    return OK;
}

int exec_client_requests(int cli_socket) {
    char *io_buff = malloc(RDSH_COMM_BUFF_SZ);
    if (!io_buff) return ERR_RDSH_SERVER;

    while (1) {
        int received = recv(cli_socket, io_buff, RDSH_COMM_BUFF_SZ, 0);
        if (received <= 0) break;

        io_buff[received - 1] = '\0';

        if (strcmp(io_buff, "exit") == 0) break;

        system(io_buff);
        send_message_eof(cli_socket);
    }

    free(io_buff);
    return OK;
}

int stop_server(int svr_socket) {
    return close(svr_socket);
}

int send_message_eof(int cli_socket) {
    int send_len = (int)sizeof(RDSH_EOF_CHAR);
    int sent_len = send(cli_socket, &RDSH_EOF_CHAR, send_len, 0);

    if (sent_len != send_len) {
        return ERR_RDSH_COMMUNICATION;
    }
    return OK;
}

int send_message_string(int cli_socket, char *buff) {
    int sent_len = send(cli_socket, buff, strlen(buff), 0);
    if (sent_len < 0) {
        return ERR_RDSH_COMMUNICATION;
    }

    return send_message_eof(cli_socket); // Ensure the EOF character is sent
}

