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
int receive( int mqueue );
int send( int mqueue );


struct Msg
{
    long priority;
    char question[MAX_BUFFER];
};

int main(int argc, char *argv[])
{
    int fd;
    int pipe1[2];
    if (pipe(pipe1) == -1)
    {
        perror("Error while creating the pipe\n");
        exit(1);
    }
    key_t key = ftok(argv[0], 17042000);
    int msg = msgget(key, 0600 | IPC_CREAT);
    char pipepath[22] = "/tmp/nicolasuniquepipe";
    int pipe2 = mkfifo(pipepath,S_IRUSR | S_IWUSR );
    if(pipe2)
    {
        if(errno == 17)
        {
            printf("Pipe is being re used\n");

        }
        else{
            printf("Error while creating the pipe\n");
        }
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
            sleep(1);
            printf("N-Neptun has acknowledge the successful log in of the student and the subject leader\n");

            int l;
            char studentInfo[MAX_BUFFER];
            close(pipe1[1]);

            read(pipe1[0], &l, sizeof(l));
            read(pipe1[0], studentInfo, l);
            
            close(pipe1[0]);
            studentInfo[l] = '\0';

            fd = open(pipepath, O_WRONLY);
            write(fd, &l, sizeof(l));
            write(fd, studentInfo, l);
            
            close(fd);

            int Status;
            wait(&Status); // Waiting for children to terminate
            unlink(pipepath);
        }
        else // Subject Leader
        {
            sleep(1);
            kill(getppid(), SIGUSR1);
            printf("SL-The Subject leader has logged in on Neptun\n");
            int l;
            char studentInfo[MAX_BUFFER];
            fd = open(pipepath, O_RDONLY);
            read(fd, &l, sizeof(l));
            read(fd, studentInfo, l);

            printf("SL-The Subject leader has received the student information from Neptun\n");
            printf("SL-This is student information: %s\n", studentInfo);
            close(fd);

            sleep(1);
            send(msg);

        }
    }
    else // Student
    {
        sleep(2);
        kill(getppid(), SIGUSR2);
        printf("s-The Student has logged in on Neptun\n");
        // Thesis title, student name, Year, Supervisor name
        int l = 42;
        char studentInfo[42] = "BSC Nicolas Nino Lozano 2025 Hurray Fradi";
        sleep(1);
        close(pipe1[0]);

        write(pipe1[1], &l, sizeof(l));
        write(pipe1[1], studentInfo, l);
        printf("s-The Student sent his information on Neptun\n");
        close(pipe1[1]);

        sleep(2);
        receive(msg);
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



int send( int mqueue ) 
{ 
     const struct Msg m = { 5, "Hurray Fradi!" }; 
     int status; 
     
     status = msgsnd( mqueue, &m, strlen ( m.question ) + 1 , 0 ); 
     if ( status < 0 ) 
          perror("msgsnd error"); 
     return 0; 
} 
     
// receiving a message. 
int receive( int mqueue ) 
{ 
     struct Msg m; 
     int status; 
     status = msgrcv(mqueue, &m, MAX_BUFFER, 5, 0 ); 
     printf("Se recibio es monda\n");
     if ( status < 0 ) 
          perror("msgsnd error"); 
     return 0; 
} 