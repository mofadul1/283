#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

int exec_local_cmd_loop() {
    char cmd_buff[SH_CMD_MAX];
    command_list_t clist;
    
    while (1) {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }
        
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';
        
        if (strcmp(cmd_buff, EXIT_CMD) == 0) {
            printf("exiting...\n");
            return OK_EXIT;
        }
        
        if (build_cmd_list(cmd_buff, &clist) == OK) {
            execute_pipeline(&clist);
            wait(NULL);
        }
    }
    return OK;
}

int execute_pipeline(command_list_t *clist) {
    int num_cmds = clist->num;
    int pipes[num_cmds - 1][2];
    pid_t pids[num_cmds];

    // Create pipes
    for (int i = 0; i < num_cmds - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return ERR_EXEC_CMD;
        }
    }

    // Fork processes
    for (int i = 0; i < num_cmds; i++) {
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork");
            return ERR_EXEC_CMD;
        }

        if (pids[i] == 0) { // Child process
            // Redirect input from previous pipe if not first command
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }

            // Redirect output to next pipe if not last command
            if (i < num_cmds - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            // Close all pipes in child
            for (int j = 0; j < num_cmds - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            // Execute command
            fflush(stdout);
            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            perror("execvp");
            exit(ERR_EXEC_CMD);
        }
    }

    // Close all pipes in parent
    for (int i = 0; i < num_cmds - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Wait for all child processes to finish
    for (int i = 0; i < num_cmds; i++) {
        waitpid(pids[i], NULL, 0);
    }

    return OK;
}


int build_cmd_list(char *cmd_line, command_list_t *clist) {
    char *token;
    int count = 0;

    memset(clist, 0, sizeof(command_list_t));

    token = strtok(cmd_line, PIPE_STRING);
    while (token != NULL) {
        if (count >= CMD_MAX) {
            printf("%s\n", CMD_ERR_PIPE_LIMIT);
            return ERR_TOO_MANY_COMMANDS;
        }

        clist->commands[count]._cmd_buffer = strdup(token);
        if (clist->commands[count]._cmd_buffer == NULL) {
            return ERR_MEMORY;
        }

        clist->commands[count].argv[0] = strtok(token, " ");
        int i = 1;
        while ((clist->commands[count].argv[i] = strtok(NULL, " ")) != NULL) {
            i++;
        }
        clist->commands[count].argc = i;
        count++;

        token = strtok(NULL, PIPE_STRING);
    }

    clist->num = count;
    return OK;
}

