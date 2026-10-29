#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define NUM_DEPT 3
#define MSG_SIZE 256

static volatile sig_atomic_t interrupted = 0;

void handle_sigint(int sig)
{
    (void)sig;
    interrupted = 1;
    const char msg[] =
        "\n[Admin] Interrupt detected (Ctrl+C)! Announcement system shutting down.\n"
        "[Admin] Closing all department connections...\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

void department(const char *name, int read_fd)
{
    char buf[MSG_SIZE];
    ssize_t n;

    signal(SIGINT, SIG_IGN);

    while ((n = read(read_fd, buf, MSG_SIZE - 1)) > 0) {
        buf[n] = '\0';
        printf("[%s - PID %d] Announcement received: %s",
               name, getpid(), buf);
        fflush(stdout);
    }
    printf("[%s - PID %d] Disconnected.\n", name, getpid());
    close(read_fd);
    exit(0);
}

int main(void)
{
    const char *dept[NUM_DEPT] = {"Library", "Finance", "Student Affairs"};
    int fd[NUM_DEPT][2];
    pid_t pid[NUM_DEPT];
    char msg[MSG_SIZE];
    int i, j;

    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    printf("=== UiTM Announcement System ===\n");
    printf("[Admin] Creating %d departments...\n", NUM_DEPT);

    for (i = 0; i < NUM_DEPT; i++) {
        if (pipe(fd[i]) == -1) {
            perror("pipe");
            exit(1);
        }

        pid[i] = fork();
        if (pid[i] < 0) {
            perror("fork");
            exit(1);
        }

        if (pid[i] == 0) {
            for (j = 0; j <= i; j++)
                close(fd[j][1]);
            department(dept[i], fd[i][0]);
        }

        close(fd[i][0]);
        printf("[Admin] Department %s (PID %d) is online\n", dept[i], pid[i]);
    }

    while (!interrupted) {
        printf("\nEnter announcement (Ctrl+C to exit): ");
        fflush(stdout);

        if (fgets(msg, MSG_SIZE, stdin) == NULL)
            break;
        if (strcmp(msg, "\n") == 0)
            continue;

        printf("\n[Admin] Sending announcement to all departments...\n");
        for (i = 0; i < NUM_DEPT; i++)
            write(fd[i][1], msg, strlen(msg));

        usleep(200000);
    }

    for (i = 0; i < NUM_DEPT; i++)
        close(fd[i][1]);
    for (i = 0; i < NUM_DEPT; i++)
        waitpid(pid[i], NULL, 0);

    printf("[Admin] System closed safely.\n");
    return 0;
}
