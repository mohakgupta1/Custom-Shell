#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "shell.h"

Token *tokenize_input(const char *input, int *token_count) {
    Token *tokens = malloc(sizeof(Token) * MAX_TOKENS);
    *token_count = 0;
    int i = 0, len = strlen(input);

    while (i < len) {
        while (isspace(input[i])) i++;

        if (input[i] == ';') {
            tokens[(*token_count)++] = (Token){TOKEN_SEMICOLON, strdup(";")};
            i++;
        } else if (input[i] == '|') {
            tokens[(*token_count)++] = (Token){TOKEN_PIPE, strdup("|")};
            i++;
        } else if (input[i] == '&') {
            tokens[(*token_count)++] = (Token){TOKEN_AMPERSAND, strdup("&")};
            i++;
        } else if (input[i] != '\0') {
            int start = i;
            while (i < len && !isspace(input[i]) && input[i] != ';' && input[i] != '|') i++;
            int word_len = i - start;
            char *word = strndup(&input[start], word_len);
            tokens[(*token_count)++] = (Token){TOKEN_WORD, word};
        }
    }

    tokens[(*token_count)++] = (Token){TOKEN_EOF, NULL};
    return tokens;
}
