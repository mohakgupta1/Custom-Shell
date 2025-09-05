#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>

#define MAX_TOKENS 128
#define MAX_CMD_LENGTH 1024
#define MAX_HISTORY 100

typedef enum {
    TOKEN_WORD,
    TOKEN_SEMICOLON,
    TOKEN_PIPE,      // Added for pipeline support
    TOKEN_AMPERSAND, // For background jobs
    TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    char *value;
} Token;

typedef struct Command {
    char **args;
    struct Command *next;
    struct Command *pipe_to; // Added for pipeline support
    int background;          // 1 if background job (&), 0 otherwise
} Command;

Token *tokenize_input(const char *input, int *token_count);
Command *parse_tokens(Token *tokens, int token_count);
void execute_command_chain(Command *cmd);
void free_command_chain(Command *cmd);
void init_history(void);
void add_to_history(const char *line);
char *get_history_entry(int index);
void free_history(void);
char *read_line_with_history(void);
void print_history(void);

// Optionally declare the signal handler
void handle_sigint(int sig);
void handle_sigchld(int sig); // Add this declaration

#endif
