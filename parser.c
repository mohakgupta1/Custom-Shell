#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"

static Command *parse_pipeline(Token *tokens, int *i, int token_count) {
    Command *head = NULL, *current = NULL;
    while (*i < token_count && tokens[*i].type != TOKEN_EOF &&
           tokens[*i].type != TOKEN_SEMICOLON) {
        Command *cmd = malloc(sizeof(Command));
        cmd->args = malloc(sizeof(char *) * MAX_TOKENS);
        cmd->next = NULL;
        cmd->pipe_to = NULL;
        cmd->background = 0; 

        int argc = 0;
        while (*i < token_count && tokens[*i].type == TOKEN_WORD) {
            cmd->args[argc++] = strdup(tokens[*i].value);
            (*i)++;
        }
        cmd->args[argc] = NULL;

        // Check for background '&'
        if (*i < token_count && tokens[*i].type == TOKEN_AMPERSAND) {
            cmd->background = 1;
            (*i)++;
        }

        if (!head) {
            head = current = cmd;
        } else {
            current->pipe_to = cmd;
            current = cmd;
        }

        if (*i < token_count && tokens[*i].type == TOKEN_PIPE) {
            (*i)++; // skip '|'
        } else {
            break;
        }
    }
    return head;
}

Command *parse_tokens(Token *tokens, int token_count) {
    Command *head = NULL, *current = NULL;
    int i = 0;

    while (i < token_count && tokens[i].type != TOKEN_EOF) {
        Command *pipeline = parse_pipeline(tokens, &i, token_count);

        if (i < token_count && tokens[i].type == TOKEN_SEMICOLON) i++;

        if (!head) {
            head = current = pipeline;
        } else {
            current->next = pipeline;
            // Move to the end of the pipeline
            while (current->pipe_to) current = current->pipe_to;
        }
    }

    return head;
}
