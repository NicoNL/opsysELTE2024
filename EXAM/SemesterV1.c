#include <stdio.h>
#include <stdlib.h> // exit  atoi
#include <string.h>
#include <stdbool.h>
#include <unistd.h>   // fork
#include <sys/wait.h> // pid info
#include <signal.h>   //Signals
#include <time.h>     // random
#include <errno.h>    //perror
#include <sys/stat.h> // Permissions
#include <fcntl.h>    // O_WRONLY AND O_RONLY
#include <sys/msg.h>  // Msg

#define MAX_BUFFER 1020

void handler1(int num);
void handler2(int num);

int main(int argc, char *argv[])
{
    int pipeID[2];
    if (pipe(pipeID) == -1)
    {
        perror("Error while creating the pipe\n");
        exit(1);
    }

    signal(SIGUSR1, handler1);
    signal(SIGUSR2, handler2);

    int student = fork();

    if (student < 0)
    {
        perror("The fork for the student was not possible\n");
        exit(1);
    }
    if (student > 0)
    {
        int leader = fork();
        if (leader < 0)
        {
            perror("The fork for the student was not possible\n");
            exit(1);
        }
        if (leader > 0) // Neptun
        {
            printf("N-Neptun is now waiting for the signals\n");
            pause();
            pause();
            printf("N-Neptun has acknowledge the successful log in of the student and the subject leader\n");

            int l;
            char studentInfo[MAX_BUFFER];
            close(pipeID[1]);

            read(pipeID[0], &l, sizeof(l));
            read(pipeID[0], studentInfo, l);
            
            close(pipeID[0]);
            int Status;
            wait(&Status); // Waiting for children to terminate
        }
        else // Subject Leader
        {
            sleep(1);
            printf("SL-The Subject leader has logged in on Neptun\n");
            kill(getppid(), SIGUSR1);
        }
    }
    else // Student
    {
        sleep(2);
        printf("s-The Student has logged in on Neptun\n");
        kill(getppid(), SIGUSR2);
        // Thesis title, student name, Year, Supervisor name
        int l = 42;
        char studentInfo[42] = "BSC Nicolas Nino Lozano 2025 Hurray Fradi";

        close(pipeID[0]);

        write(pipeID[1], &l, sizeof(l));
        write(pipeID[1], studentInfo, l);

        close(pipeID[1]);
    }

    return 0;
}

void handler1(int num)
{
    printf("The leader sent a signal with number %d was sent to Neptun\n");
}
void handler2(int num)
{
    printf("The student sent a signal with number %d was sent to Neptun\n");
}