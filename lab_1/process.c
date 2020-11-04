// HUST OS Lab 1 Process
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <error.h>
#include <string.h>
#include <signal.h>
// #include <csignal>

#define BUFFER_SIZE 200
#define MAX_TIME 9
int fd[2];
int count = 0;
char wbuffer[BUFFER_SIZE];
char rbuffer[BUFFER_SIZE];
pid_t pid1, pid2;

void parentSignalHandler(int);
void childSignalHandler_1(int);
void childSignalHandler_2(int);

int main() {
    memset(wbuffer, 0, sizeof(wbuffer));
    memset(rbuffer, 0, sizeof(rbuffer));
    if(pipe(fd) != 0) {
        perror("create pipe error\n");
        exit(-1);
    }

    if((pid1 = fork()) < 0) {
        perror("fork error\n");
        exit(-1);
    }
    if(pid1 == 0) {
        // child process 1
        int readfd = fd[0];
        int writefd = fd[1];
        int read_ret;
        if(signal(SIGINT, childSignalHandler_1) == SIG_ERR) {
            perror("[child1]set signal handler error\n");
            exit(-1);
        }
        if(signal(SIGUSR1, childSignalHandler_1) == SIG_ERR) {
            perror("[child1]set signal handler error\n");
            exit(-1);
        }
        if(signal(SIGUSR2, childSignalHandler_1) == SIG_ERR) {
            perror("[child1]set signal handler error\n");
            exit(-1);
        }
        close(writefd);
        while (count < 1000)
        {
            if(count < MAX_TIME) {
                if((read_ret = read(readfd, rbuffer, sizeof(rbuffer))) == -1) {
                    perror("[child1]read pipe error\n");
                    exit(-1);
                } else if (read_ret == 0) {
                    perror("[child1]the write port of pipe close\n");
                    exit(-1);
                }
                printf("child process %d read {%s} from pipe\n", getpid(), rbuffer);
            }
        }
        close(readfd);
    } else {
        if((pid2 = fork()) < 0) {
            perror("fork error\n");
            exit(-1);
        }
        if(pid2 == 0) {
            // child process 2
            int readfd = fd[0];
            int writefd = fd[1];
            int read_ret;
            if(signal(SIGINT, childSignalHandler_2) == SIG_ERR) {
                perror("[child2]set signal handler error\n");
                exit(-1);
            }
            if(signal(SIGUSR1, childSignalHandler_2) == SIG_ERR) {
                perror("[child2]set signal handler error\n");
                exit(-1);
            }
            if(signal(SIGUSR2, childSignalHandler_2) == SIG_ERR) {
                perror("[child2]set signal handler error\n");
                exit(-1);
            }
            close(writefd);
            while (count < 1000)
            {
                if(count < MAX_TIME) {
                    if((read_ret = read(readfd, rbuffer, sizeof(rbuffer))) == -1) {
                        perror("[child2]read pipe error\n");
                        exit(-1);
                    } else if (read_ret == 0) {
                        perror("[child2]the write port of pipe close\n");
                        exit(-1);
                    }
                    printf("child process %d read {%s} from pipe\n", getpid(), rbuffer);
                }
            }
            close(readfd);
        } else {
            // parent process
            int readfd = fd[0];
            int writefd = fd[1];
            if(signal(SIGINT, parentSignalHandler) == SIG_ERR) {
                perror("[parent]set signal handler error\n");
            }
            if(signal(SIGUSR1, parentSignalHandler) == SIG_ERR) {
                perror("[parent]set signal handler error\n");
            }
            if(signal(SIGUSR2, parentSignalHandler) == SIG_ERR) {
                perror("[parent]set signal handler error\n");
            }
            close(readfd);
            while (count < 1000)
            {
                count++;
                if(count < MAX_TIME) {
                    char *string_1 = "I send you ";
                    char string_2[2];
                    string_2[0] = (char)(count + 48);
                    string_2[1] = '\0';
                    char *string_3 = " times.";
                    // printf("[log]size of string_4: %d\n", strlen(string_1) + strlen(string_2) + strlen(string_3));
                    char *string_4 = (char*)malloc(strlen(string_1) + strlen(string_2) + strlen(string_3));
                    sprintf(string_4, "%s%s%s", string_1, string_2, string_3);
                    memmove(wbuffer, string_4, strlen(string_4));
                    write(writefd, wbuffer, strlen(wbuffer));
                    // printf("[log]send message: %s\n", wbuffer);
                    // printf("[parent]send message %d times\n", count);
                }
                sleep(1);
            }
            close(writefd);
            pid_t exit_pid_1 = wait(NULL);
            printf("child process %d exit\n", exit_pid_1);
            pid_t exit_pid_2 = wait(NULL);
            printf("child process %d exit\n", exit_pid_2);
            printf("parent process exit\n");
        }
    }
}

void parentSignalHandler(int signum) {
    if(signum == 2) {
        // SIGINT
        printf("[SignalHandler]receive SIGINT, send SIGUSR.\n");
        // raise(SIGUSR1);
        // raise(SIGUSR2);
        kill(pid1, SIGUSR1);
        kill(pid2, SIGUSR2);
        pid_t exit_pid_1 = wait(NULL);
        printf("child process %d exit\n", exit_pid_1);
        pid_t exit_pid_2 = wait(NULL);
        printf("child process %d exit\n", exit_pid_2);
        printf("parent process exit with message count: %d\n", count);
        exit(signum);
}
    else if(signum == 10) {
        // SIGUSR1
        // printf("[SignalHandler]receive SIGUSR1, exit.\n");
        // exit(signum);
    }
    else if(signum == 12) {
        // SIGUSR2
        // printf("[SignalHandler]receive SIGUSR2, exit.\n");
        // exit(signum);
    }
    else {
        // unimplemet!
    }
}

void childSignalHandler_1(int signum) {
    if(signum == 2) {
        // SIGINT
        // printf("[SignalHandler]receive SIGINT, send SIGUSR.\n");
        // raise(10);
        // raise(12);
        // pid_t exit_pid_1 = wait(NULL);
        // printf("child process %d exit\n", exit_pid_1);
        // pid_t exit_pid_2 = wait(NULL);
        // printf("child process %d exit\n", exit_pid_2);
        // printf("parent process exit\n");
        // exit(signum);
    }
    else if(signum == 10) {
        // SIGUSR1
        printf("[SignalHandler]receive SIGUSR1, exit.\n");
        exit(signum);
    }
    else if(signum == 12) {
        // SIGUSR2
        // printf("[SignalHandler]receive SIGUSR2, exit.\n");
        // exit(signum);
    }
    else {
        // unimplemet!
    }
}

void childSignalHandler_2(int signum) {
    if(signum == 2) {
        // SIGINT
        // printf("[SignalHandler]receive SIGINT, send SIGUSR.\n");
        // raise(10);
        // raise(12);
        // pid_t exit_pid_1 = wait(NULL);
        // printf("child process %d exit\n", exit_pid_1);
        // pid_t exit_pid_2 = wait(NULL);
        // printf("child process %d exit\n", exit_pid_2);
        // printf("parent process exit\n");
        // exit(signum);
    }
    else if(signum == 10) {
        // SIGUSR1
        // printf("[SignalHandler]receive SIGUSR1, exit.\n");
        exit(signum);
    }
    else if(signum == 12) {
        // SIGUSR2
        printf("[SignalHandler]receive SIGUSR2, exit.\n");
        exit(signum);
    }
    else {
        // unimplemet!
    }
}