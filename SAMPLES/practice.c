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

struct Msg{
    long priority;
    int number;
};
int receive(int mqueue, int *number);
int send(int mqueue, int num);
void handler1(int num);
void handler2(int num);
void handler3(int num);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Not enough arguements were given\n");
        exit(1);
    }

    int myPipe;
    int chdPipe[2];
    if (pipe(chdPipe) == -1)
    {
        perror("The pipe was not created");
        exit(1);
    }
    char pipepath[12] = "/tmp/nnlpipe";
    int resultPipe = mkfifo(pipepath, S_IRUSR | S_IWUSR);

    if (resultPipe == -1)
    {
        if (errno == 17)
        {
            printf("Pipe is being re-used\n");
        }
        else
        {
            printf("Error while creating the pipe\n");
        }
    }
    int status;
    key_t key = ftok(argv[0],136665);
    int msg =msgget(key,0600 | IPC_CREAT);
    if(msg < 0)
    {
        perror("Error while creating the msqueue\n");
        exit(1);
    }
    int voters = atoi(argv[1]);
    int votersArr[voters];

    signal(SIGUSR1, handler1);
    signal(SIGUSR2, handler2);
    signal(SIGUSR1, handler3);

    pid_t chd1 = fork();

    if (chd1 < 0)
    {
        perror("The fork was not succesful\n");
        exit(1);
    }

    if (chd1 > 0)
    {
        pid_t chd2 = fork();
        if (chd2 < 0)
        {
            perror("The fork was not succesful\n");
            exit(1);
        }
        if (chd2 > 0) // President
        {
            printf("The president is wating for the members\n");
            pause();
            pause();
            printf("The president is ready to work\n");
            myPipe = open(pipepath, O_WRONLY);

            srand(time(NULL));
            for (int i = 0; i < voters; i++)
            {
                int id = rand() % 900 + 101;
                write(myPipe, &id, sizeof(id));
            }

            close(myPipe);
            unlink(pipepath);
            pause();
            int number;
            receive(msg,&number);
            printf("this is the mfn number %d\n", number);

            int Status;
            wait(&Status);
        }
        else // Member1
        {
            sleep(1);
            printf("The first member is ready to work\n");
            kill(getppid(), SIGUSR1);

            int votersArray[voters];
            myPipe = open(pipepath, O_RDONLY);
            for (int i = 0; i < voters; i++)
            {
                int id;
                int voter = i + 1;
                read(myPipe, &id, sizeof(id));
                printf("Voter number %d has the ID: %i\n", voter, id);
                votersArray[i] = id;
            }
            close(myPipe);
            srand(time(NULL));
            kill(chd1, SIGUSR1);

            close(chdPipe[0]);
            for (int i = 0; i < voters; i++)
            {
                write(chdPipe[1], &votersArray[i], sizeof(int));
                int errProb = rand() % 100 + 1;
                if (errProb <= 20)
                {
                    int l = 11;
                    char s[11] = "cannot vote";
                    write(chdPipe[1], &l, sizeof(int));
                    write(chdPipe[1], s, l);
                }
                else
                {
                    int l = 8;
                    char s[8] = "can vote";
                    write(chdPipe[1], &l, sizeof(int));
                    write(chdPipe[1], s, l);
                }
            }
            close(chdPipe[1]);
        }
    }
    else // Member2
    {
        sleep(2);
        printf("The second member is ready to work\n");
        kill(getppid(), SIGUSR2);
        pause();
        close(chdPipe[1]);
        for (int i = 0; i < voters; i++)
        {
            int id, l;
            int voter = i + 1;
            char s[11];
            read(chdPipe[0], &id, sizeof(int));
            read(chdPipe[0], &l, sizeof(int));
            read(chdPipe[0], s, l);
            s[l] ='\0';
            printf("The voter #%d with ID %d %s\n", voter, id, s);
        }
        close(chdPipe[0]);
        kill(getppid(), SIGUSR2);
        srand(time(NULL));
            //int number =  rand() % 6 +1;
        send(msg,3);
        status = msgctl(msg, IPC_RMID, NULL);
    }
}

void handler1(int num)
{
    printf("Member 1 sent signal with number %d was sent\n", num);
}
void handler2(int num)
{
    printf("Member 2 sent  signal with number %d was sent\n", num);
}
void handler3(int num)
{
    printf("Member 2 sent  signal with number %d was sent\n", num);
}

int send(int mqueue, int num)
{
    int x = -1;
    if (num == 1)
    {
        srand(time(NULL));
        x = rand() % 6 + 1;
    }
    struct Msg m = {5, x};
    int status;
    status = msgsnd(mqueue, &m, sizeof(m) -sizeof(long), 0);
    if (status < 0)
        perror("Error sending msg: ");
    return 0;
}

int receive(int mqueue, int *number)
{
    struct Msg m;
    int status;

    status = msgrcv(mqueue, &m, sizeof(m) -sizeof(long), 5, 0);

    if (status < 0)
        perror("Error receiving the msg: ");
    else
        // printf( "- ♚ The president received a new message\n" );
        *number = m.number;
    return 0;
}