#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    while (*cmd_line == SPACE_CHAR)
    {
        cmd_line++;
    }

    int len = strlen(cmd_line);
    while (len > 0 && cmd_line[len - 1] == SPACE_CHAR)
    {
        cmd_line[len - 1] = '\0';
        len--;
    }

    if (len == 0)
    {
        return WARN_NO_CMDS;
    }

    int cmd_count = 0;
    char *cmd_pointer = NULL;
    char *arg_pointer = NULL;
    char *cmd = strtok_r(cmd_line, PIPE_STRING, &cmd_pointer);

    while (cmd != NULL && cmd_count < CMD_MAX)
    {
        memset(&clist->commands[cmd_count], 0, sizeof(command_t));

        char *arg = strtok_r(cmd, " ", &arg_pointer);

        if (arg != NULL)
        {
            strncpy(clist->commands[cmd_count].exe, arg, EXE_MAX - 1);

            arg = strtok_r(NULL, " ", &arg_pointer);
            while (arg != NULL)
            {
                if (strlen(clist->commands[cmd_count].args) + strlen(arg) + 1 >= ARG_MAX)
                {
                    return ERR_CMD_OR_ARGS_TOO_BIG;
                }

                if (strlen(clist->commands[cmd_count].args) > 0)
                {
                    strcat(clist->commands[cmd_count].args, " ");
                }

                strcat(clist->commands[cmd_count].args, arg);
                arg = strtok_r(NULL, " ", &arg_pointer);
            }
        }

        cmd_count++;
        cmd = strtok_r(NULL, PIPE_STRING, &cmd_pointer);
    }

    if (cmd != NULL && cmd_count >= CMD_MAX)
    {
        return ERR_TOO_MANY_COMMANDS;
    }

    clist->num = cmd_count;
    return OK;
}
