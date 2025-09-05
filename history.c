#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "shell.h"

static char *history[MAX_HISTORY];
static int history_count = 0;

void init_history(void) {
    for (int i = 0; i < MAX_HISTORY; i++) {
        history[i] = NULL;
    }
    history_count = 0;
}

void add_to_history(const char *cmd) {
    if (!cmd || !*cmd) return;
    if (history_count < MAX_HISTORY) {
        history[history_count++] = strdup(cmd);
    } else {
        free(history[0]);
        for (int i = 1; i < MAX_HISTORY; i++)
            history[i - 1] = history[i];
        history[MAX_HISTORY - 1] = strdup(cmd);
    }
    add_history(cmd); 
}

void print_history(void) {
    for (int i = 0; i < history_count; i++) {
        printf("%d %s\n", i + 1, history[i]);
    }
}

char *get_history_entry(int index) {
    if (index < 0 || index >= history_count) {
        return NULL;
    }
    return history[index];
}

void free_history(void) {
    for (int i = 0; i < history_count; i++) {
        free(history[i]);
        history[i] = NULL;
    }
    history_count = 0;
}

char *read_line_with_history(void) {
    char *line = readline("myshell> ");
    if (line && *line) {
        add_to_history(line);
    }
    return line;
}