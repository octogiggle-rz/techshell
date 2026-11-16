#define _POSIX_C_SOURCE 200809L
/*
 * Name: Shreyasri Rijal
 * CSC 222 - A Simple Shell  Project
 * A small interactive shell with built-in cd/exit, external commands,
 * input/output redirection, quoting, and backslash escaping.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>

typedef struct {
    char **argv;
    size_t argc;
    size_t capacity;
    char *input_file;
    char *output_file;
} Command;

static void error_message(int number) {
    fprintf(stderr, "Error %d (%s)\n", number, strerror(number));
}

static void init_command(Command *cmd) {
    memset(cmd, 0, sizeof(*cmd));
}

static void free_command(Command *cmd) {
    for (size_t i = 0; i < cmd->argc; ++i) free(cmd->argv[i]);
    free(cmd->argv);
    free(cmd->input_file);
    free(cmd->output_file);
    init_command(cmd);
}

static int add_argument(Command *cmd, char *arg) {
    if (cmd->argc + 1 >= cmd->capacity) {
        size_t new_capacity = cmd->capacity ? cmd->capacity * 2 : 8;
        char **new_args = realloc(cmd->argv, new_capacity * sizeof(*new_args));
        if (!new_args) return -1;
        cmd->argv = new_args;
        cmd->capacity = new_capacity;
    }
    cmd->argv[cmd->argc++] = arg;
    cmd->argv[cmd->argc] = NULL;
    return 0;
}

static int append_character(char **buffer, size_t *length, size_t *capacity, char ch) {
    if (*length + 1 >= *capacity) {
        size_t new_capacity = *capacity ? *capacity * 2 : 32;
        char *new_buffer = realloc(*buffer, new_capacity);
        if (!new_buffer) return -1;
        *buffer = new_buffer;
        *capacity = new_capacity;
    }
    (*buffer)[(*length)++] = ch;
    (*buffer)[*length] = '\0';
    return 0;
}

/* Tokenize whitespace, quotes, escaped characters and redirection symbols.
 * Return -1 on invalid input or allocation failure. */
static int next_token(const char **position, char **token, int *is_operator) {
    const char *p = *position;
    while (*p && isspace((unsigned char)*p)) ++p;
    if (!*p) { *position = p; return 0; }

    *is_operator = (*p == '<' || *p == '>');
    if (*is_operator) {
        *token = malloc(2);
        if (!*token) return -1;
        (*token)[0] = *p++;
        (*token)[1] = '\0';
        *position = p;
        return 1;
    }

    char *result = NULL;
    size_t length = 0, capacity = 0;
    char quote = 0;
    while (*p) {
        if (*p == '\\' && p[1]) {
            if (append_character(&result, &length, &capacity, p[1]) < 0) goto failure;
            p += 2;
            continue;
        }
        if (quote) {
            if (*p == quote) { quote = 0; ++p; continue; }
        } else {
            if (*p == '\'' || *p == '"') { quote = *p++; continue; }
            if (isspace((unsigned char)*p) || *p == '<' || *p == '>') break;
        }
        if (append_character(&result, &length, &capacity, *p++) < 0) goto failure;
    }
    if (quote) goto failure;
    if (!result) {
        result = strdup("");
        if (!result) goto failure;
    }
    *token = result;
    *position = p;
    return 1;
failure:
    free(result);
    return -1;
}

static int parse_command(const char *line, Command *cmd) {
    const char *position = line;
    int expect_file = 0; /* 1: input, 2: output */
    for (;;) {
        char *token = NULL;
        int is_operator = 0;
        int result = next_token(&position, &token, &is_operator);
        if (result == 0) break;
        if (result < 0) return -1;
        if (expect_file) {
            if (is_operator) { free(token); return -1; }
            char **destination = expect_file == 1 ? &cmd->input_file : &cmd->output_file;
            free(*destination);
            *destination = token;
            expect_file = 0;
        } else if (is_operator) {
            expect_file = token[0] == '<' ? 1 : 2;
            free(token);
        } else if (add_argument(cmd, token) < 0) {
            free(token);
            return -1;
        }
    }
    return expect_file ? -1 : 0;
}

static void show_prompt(void) {
    char *cwd = getcwd(NULL, 0);
    if (!cwd) {
        error_message(errno);
        fputs("$ ", stdout);
    } else {
        printf("%s$ ", cwd);
        free(cwd);
    }
    fflush(stdout);
}

/* Redirection must be opened before exec; failures prevent execution. */
static int apply_redirection(const Command *cmd) {
    if (cmd->input_file) {
        int fd = open(cmd->input_file, O_RDONLY);
        if (fd < 0) { error_message(errno); return -1; }
        if (dup2(fd, STDIN_FILENO) < 0) {
            int saved = errno;
            close(fd);
            error_message(saved);
            return -1;
        }
        close(fd);
    }
    if (cmd->output_file) {
        int fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fd < 0) { error_message(errno); return -1; }
        if (dup2(fd, STDOUT_FILENO) < 0) {
            int saved = errno;
            close(fd);
            error_message(saved);
            return -1;
        }
        close(fd);
    }
    return 0;
}

static void execute_command(const Command *cmd) {
    if (!cmd->argc) return;

    if (strcmp(cmd->argv[0], "cd") == 0) {
        const char *destination = cmd->argc > 1 ? cmd->argv[1] : getenv("HOME");
        if (!destination) destination = "/";
        if (chdir(destination) < 0) error_message(errno);
        return;
    }
    if (strcmp(cmd->argv[0], "exit") == 0) exit(0);

    pid_t pid = fork();
    if (pid < 0) { error_message(errno); return; }
    if (pid == 0) {
        if (apply_redirection(cmd) < 0) _exit(1);
        execvp(cmd->argv[0], cmd->argv);
        error_message(errno);
        _exit(127);
    }
    int status;
    while (waitpid(pid, &status, 0) < 0) {
        if (errno == EINTR) continue;
        error_message(errno);
        break;
    }
}

int main(void) {
    char *line = NULL;
    size_t line_capacity = 0;
    for (;;) {
        show_prompt();
        ssize_t bytes = getline(&line, &line_capacity, stdin);
        if (bytes < 0) {
            if (ferror(stdin)) error_message(errno);
            break;
        }
        Command command;
        init_command(&command);
        if (parse_command(line, &command) < 0) {
            fprintf(stderr, "Error 22 (%s)\n", strerror(EINVAL));
        } else {
            execute_command(&command);
        }
        free_command(&command);
    }
    free(line);
    return 0;
}
