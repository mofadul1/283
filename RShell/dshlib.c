#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

int exec_local_cmd_loop()
{
    char cmd_line[SH_CMD_MAX];
    cmd_buff_t cmd_buff;
    command_list_t cmd_list;
    int status;

    while (1)
    {
        printf("%s", SH_PROMPT);
        fflush(stdout);

        if (fgets(cmd_line, sizeof(cmd_line), stdin) == NULL)
        {
            printf("\n");
            break;
        }

        cmd_line[strcspn(cmd_line, "\n")] = '\0';

        if (strlen(cmd_line) == 0)
        {
            printf("%s", CMD_WARN_NO_CMD);
            continue;
        }

        if (strcmp(cmd_line, EXIT_CMD) == 0)
        {
            break;
        }

        if (strchr(cmd_line, PIPE_CHAR) != NULL)
        {
            if ((status = build_cmd_list(cmd_line, &cmd_list)) != OK)
            {
                if (status == ERR_TOO_MANY_COMMANDS)
                {
                    printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
                }
                else if (status == WARN_NO_CMDS)
                {
                    printf("%s", CMD_WARN_NO_CMD);
                }
                else
                {
                    printf("Error building command list\n");
                }
                free_cmd_list(&cmd_list);
                continue;
            }

            if ((status = execute_pipeline(&cmd_list)) != OK)
            {
                if (status != OK_EXIT)
                {
                    fprintf(stderr, CMD_ERR_EXECUTE, cmd_line);
                }
                else
                {
                    free_cmd_list(&cmd_list);
                    return OK;
                }
            }

            free_cmd_list(&cmd_list);
        }
        else
        {
            if ((status = alloc_cmd_buff(&cmd_buff)) != OK)
            {
                printf("Memory allocation error\n");
                continue;
            }

            if ((status = build_cmd_buff(cmd_line, &cmd_buff)) != OK)
            {
                if (status == WARN_NO_CMDS)
                {
                    printf("%s", CMD_WARN_NO_CMD);
                }
                free_cmd_buff(&cmd_buff);
                continue;
            }

            Built_In_Cmds bi_status = exec_built_in_cmd(&cmd_buff);
            if (bi_status == BI_EXECUTED)
            {
                free_cmd_buff(&cmd_buff);
                continue;
            }
            else if (bi_status == BI_CMD_EXIT)
            {
                free_cmd_buff(&cmd_buff);
                break;
            }

            if ((status = exec_cmd(&cmd_buff)) != OK)
            {
                fprintf(stderr, CMD_ERR_EXECUTE, cmd_line);
            }

            free_cmd_buff(&cmd_buff);
        }
    }

    return OK;
}

int execute_pipeline(command_list_t *clist)
{
    if (clist->num == 0)
    {
        fprintf(stderr, CMD_WARN_NO_CMD);
        return WARN_NO_CMDS;
    }

    int pipes[CMD_MAX - 1][2];
    pid_t pids[CMD_MAX];

    for (int i = 0; i < clist->num - 1; i++)
    {
        if (pipe(pipes[i]) == -1)
        {
            perror("pipe");
            return ERR_MEMORY;
        }
    }

    for (int i = 0; i < clist->num; i++)
    {
        pids[i] = fork();

        if (pids[i] == -1)
        {
            perror("fork");
            return ERR_MEMORY;
        }

        if (pids[i] == 0)
        {
            if (i > 0)
            {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }

            if (i < clist->num - 1)
            {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            for (int j = 0; j < clist->num - 1; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            fflush(stdout);
            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            perror("execvp");
            exit(ERR_EXEC_CMD);
        }
    }

    for (int i = 0; i < clist->num - 1; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < clist->num; i++)
    {
        waitpid(pids[i], NULL, 0);
    }

    return OK;
}

Built_In_Cmds match_command(const char *input)
{
    if (strcmp(input, EXIT_CMD) == 0)
    {
        return BI_CMD_EXIT;
    }
    else if (strcmp(input, "cd") == 0)
    {
        return BI_CMD_CD;
    }
    return BI_NOT_BI;
}

int build_cmd_list(char *cmd_line, command_list_t *clist) {
    clist->num = 0;
    char *cmd_copy = strdup(cmd_line);
    if (cmd_copy == NULL) {
        return ERR_MEMORY;
    }

    char *saveptr;
    char *token = strtok_r(cmd_copy, PIPE_STRING, &saveptr);

    while (token != NULL && clist->num < CMD_MAX) {
        while (isspace((unsigned char)*token)) token++; // Trim leading spaces

        char *end = token + strlen(token) - 1;
        while (end > token && isspace((unsigned char)*end)) *end-- = '\0'; // Trim trailing spaces

        if (strlen(token) == 0) {
            token = strtok_r(NULL, PIPE_STRING, &saveptr);
            continue;
        }

        if (alloc_cmd_buff(&clist->commands[clist->num]) != OK) {
            free(cmd_copy);
            return ERR_MEMORY;
        }

        build_cmd_buff(token, &clist->commands[clist->num]);
        clist->num++;
        token = strtok_r(NULL, PIPE_STRING, &saveptr);
    }

    free(cmd_copy);

    if (clist->num == 0) {
        return WARN_NO_CMDS;
    }
    return OK;
}

int free_cmd_list(command_list_t *clist) {
    for (int i = 0; i < clist->num; i++) {
        free_cmd_buff(&clist->commands[i]);
    }
    clist->num = 0;
    return OK;
}

int alloc_cmd_buff(cmd_buff_t *cmd_buff) {
    cmd_buff->_cmd_buffer = (char *)malloc(SH_CMD_MAX * sizeof(char));
    if (cmd_buff->_cmd_buffer == NULL) {
        return ERR_MEMORY;
    }
    cmd_buff->argc = 0;
    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        cmd_buff->argv[i] = NULL;
    }
    return OK;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    memset(cmd_buff, 0, sizeof(*cmd_buff));
    cmd_buff->_cmd_buffer = strdup(cmd_line);

    char *token = strtok(cmd_buff->_cmd_buffer, " ");
    int argc = 0;

    while (token) {
        if (argc >= CMD_ARGV_MAX - 1) {
            fprintf(stderr, "error: too many arguments\n");
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        cmd_buff->argv[argc++] = token;
        token = strtok(NULL, " ");
    }

    cmd_buff->argv[argc] = NULL;
    cmd_buff->argc = argc;

    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff) {
    if (cmd_buff->_cmd_buffer != NULL) {
        free(cmd_buff->_cmd_buffer);
        cmd_buff->_cmd_buffer = NULL;
    }
    cmd_buff->argc = 0;
    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        cmd_buff->argv[i] = NULL;
    }
    return OK;
}

int exec_cmd(cmd_buff_t *cmd) {
    pid_t pid;
    int status;

    pid = fork();
    if (pid < 0) {
        perror("fork");
        return ERR_EXEC_CMD;
    } else if (pid == 0) {
        if (execvp(cmd->argv[0], cmd->argv) < 0) {
            perror("execvp");
            exit(ERR_EXEC_CMD);
        }
    } else {
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } else {
            return ERR_EXEC_CMD;
        }
    }
    return OK;
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    Built_In_Cmds cmd_type = match_command(cmd->argv[0]);

    switch (cmd_type) {
    case BI_CMD_CD:
        if (cmd->argc > 1) {
            if (chdir(cmd->argv[1]) != 0) {
                perror("cd");
            }
        } else {
            char *home = getenv("HOME");
            if (home && chdir(home) != 0) {
                perror("cd");
            }
        }
        return BI_EXECUTED;

    case BI_CMD_EXIT:
        return BI_CMD_EXIT;

    default:
        return BI_NOT_BI;
    }
}
