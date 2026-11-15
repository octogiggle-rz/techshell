# CSC 222 — TechShell

**Name:** Shreyasri Rijal

## Description
TechShell is a small command-line shell written in C. It displays the current working directory, accepts commands, and runs external programs in child processes. It also includes the built-in `cd` and `exit` commands, input and output file redirection, and support for quoted arguments and escaped spaces.

## Clone, compile, and run

Clone your GitHub repository (replace `YOUR_USERNAME` with your GitHub username):

```bash
git clone https://github.com/YOUR_USERNAME/techshell.git
cd techshell
```

Compile on a Linux or POSIX-compatible environment with GCC:

```bash
gcc -Wall -Wextra -std=c11 -o techshell techshell.c
```

Run:

```bash
./techshell
```

## Example commands

```text
pwd
mkdir demo
cd demo
/bin/echo "hello world" > message.txt
cat < message.txt
wc -l < message.txt > count.txt
cat count.txt
cd ..
exit
```

## Supported functionality

- Prompt shows the current directory and updates after `cd`.
- Runs programs with arguments, including absolute paths, using `fork()` and `execvp()`.
- `cd` works with relative and absolute paths; `cd` with no argument uses `HOME`.
- `exit` quits the shell.
- `<` redirects standard input and `>` redirects standard output (creating or replacing files).
- Supports both input and output redirection on one command.
- Supports single/double quoted arguments and backslash-escaped spaces.
- Reports system errors in `Error # (MESSAGE)` format.
- Ignores blank lines and continues after failed commands.

## Limitations

Pipelines (`|`), command chaining (`&&`, `;`), background jobs (`&`), variable expansion, and append redirection (`>>`) are not implemented because they are not required by the project rubric. Built-in `cd` and `exit` do not implement I/O redirection. Unmatched quotes and missing redirection filenames produce `Error 22 (Invalid argument)`.

## Submission note

Only submit source files (`techshell.c`, `readme.md`) to GitHub. Do not commit the compiled `techshell` executable.
