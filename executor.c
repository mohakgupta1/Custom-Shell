#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include "shell.h"

void handle_sigchld(int sig) {
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void execute_pipeline(Command *cmd) {
    int fd[2], in_fd = 0;
    int background = cmd->background;
    pid_t last_pid = -1;

    while (cmd) {
        pipe(fd);
        pid_t pid = fork();

        if (pid == 0) {
            // Child
            if (in_fd != 0) {
                dup2(in_fd, 0);
                close(in_fd);
            }
            if (cmd->pipe_to) {
                dup2(fd[1], 1);
                close(fd[1]);
            }
            close(fd[0]);
            execvp(cmd->args[0], cmd->args);
            perror("execvp");
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            perror("fork");
            return;
        }

        if (in_fd != 0) close(in_fd);
        close(fd[1]);
        in_fd = fd[0];
        last_pid = pid;
        cmd = cmd->pipe_to;
    }

    if (!background) {
        // Foreground: wait for last process in pipeline
        if (last_pid > 0) waitpid(last_pid, NULL, 0);
    } else {
        // Background: print PID, don't wait
        if (last_pid > 0)
            printf("[background pid %d]\n", last_pid);
    }
}

void execute_command_chain(Command *cmd) {
    while (cmd) {
        if (cmd->args[0] == NULL) {
            cmd = cmd->next;
            continue;
        }

        if (strcmp(cmd->args[0], "cd") == 0) {
            if (cmd->args[1] == NULL || chdir(cmd->args[1]) != 0)
                perror("cd");
        } else if (strcmp(cmd->args[0], "exit") == 0) {
            exit(0);
        } else if (strcmp(cmd->args[0], "history") == 0) {
            print_history();
        } else {
            execute_pipeline(cmd);
        }

        cmd = cmd->next;
    }
}

void free_command_chain(Command *cmd) {
    while (cmd) {
        for (int i = 0; cmd->args[i]; i++)
            free(cmd->args[i]);
        free(cmd->args);

        // Free pipeline
        Command *pipe_cmd = cmd->pipe_to;
        while (pipe_cmd) {
            for (int i = 0; pipe_cmd->args[i]; i++)
                free(pipe_cmd->args[i]);
            free(pipe_cmd->args);
            Command *next_pipe = pipe_cmd->pipe_to;
            free(pipe_cmd);
            pipe_cmd = next_pipe;
        }

        Command *next = cmd->next;
        free(cmd);
        cmd = next;
    }
}
