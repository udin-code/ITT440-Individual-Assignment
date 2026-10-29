# UiTM Announcement System

ITT440 Network Programming - Individual Assignment

A C program that uses fork(), pipe() and SIGINT handling. The parent process (Admin) reads an announcement from the keyboard and sends it through pipes to three child processes (Library, Finance and Student Affairs), which display it. Pressing Ctrl+C shows an interrupt message and shuts down safely.

## Compile
gcc -Wall -o announce announce.c

## Run
./announce

## Author
MOHAMAD IRSYAD IZZUDDIN BIN MOHD ISA (2026116193)
