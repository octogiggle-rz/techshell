# CSC 222 - TechShell

**Name:** Shreyasri Rijal

## About My Project

For my CSC 222 project, I created a simple shell using C. The idea was to make a program where someone can type commands and have them run, just like in a Linux terminal.

My shell can run commands like `ls`, `pwd`, and `echo`. I also added `cd` so users can move between folders and `exit` to close the program. It can also read from files and save output to files using `<` and `>`.

## How to Run My Program

First, clone the project from GitHub:

```bash
git clone https://github.com/octogiggle-rz/techshell.git
cd techshell
```

Then compile the C file:

```bash
gcc -o techshell techshell.c
```

After that, run the program:

```bash
./techshell
```

## Some Commands to Try

These are a few commands I used to test my shell:

```text
pwd
ls
echo Hello
mkdir test
cd test
cd ..
echo Hello > test.txt
cat test.txt
cat < test.txt
exit
```

## What My Program Can Do

- Show the current folder in the prompt.
- Run commands and their arguments.
- Change folders using `cd`.
- Close the shell using `exit`.
- Read input from a file using `<`.
- Save output to a file using `>`.
- Handle filenames with spaces and commands with quotes.
- Show an error message if something goes wrong.

## What I Didn't Add

I didn't include some of the more advanced shell features, like pipes (`|`), running commands in the background (`&`), or joining commands with `&&`. I focused on getting the main features from the assignment working first.

## Project Files

- `techshell.c` - Contains the code for my shell.
- `readme.md` - Explains what my program does and how to run it.