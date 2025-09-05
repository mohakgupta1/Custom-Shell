#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include "shell.h"

void handle_sigint(int sig) {
    (void)sig;
    rl_replace_line("", 0);
    printf("\n");
    rl_on_new_line();
    rl_redisplay();
}

int main() {
    init_history();
    char *input;

    // Initialize readline
    rl_bind_key('\t', rl_complete);

    // Set up SIGINT handler
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);

    // Set up SIGCHLD handler for background jobs
    struct sigaction sa_chld;
    sa_chld.sa_handler = handle_sigchld;
    sigemptyset(&sa_chld.sa_mask);
    sa_chld.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa_chld, NULL);

    while (1) {
        // Use readline instead of fgets
        input = read_line_with_history();
        
        // Handle EOF (Ctrl+D)
        if (!input) {
            printf("\n");
            break;
        }

        // Skip empty lines
        if (!*input) {
            free(input);
            continue;
        }

        int token_count = 0;
        Token *tokens = tokenize_input(input, &token_count);
        Command *cmd_chain = parse_tokens(tokens, token_count);
        execute_command_chain(cmd_chain);
        free_command_chain(cmd_chain);

        for (int i = 0; i < token_count; i++) {
            if (tokens[i].value) free(tokens[i].value);
        }
        free(tokens);
        free(input);
    }

    free_history();
    return 0;
}
