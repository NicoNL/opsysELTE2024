#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/msg.h>

#define MAX_BUFFER 1024

struct Msg
{
    long priority;
    int number;
    char kit[MAX_BUFFER];
};

int send(int mqueue, int num, char *kit, long pri)
{
    int x = num;
    struct Msg m;
    m.priority = pri;
    m.number = num;
    strcpy(m.kit,kit);

    int status;
    status = msgsnd(mqueue, &m, sizeof(m) -sizeof(long), 0);
    if (status < 0)
        perror("Error sending msg: ");
    return 0;
}

int receive(int mqueue, int *number, char * kit, long pri)
{
    struct Msg m;
    int status;

    status = msgrcv(mqueue, &m, sizeof(m) -sizeof(long), pri, 0);

    if (status < 0)
        perror("Error receiving the msg: ");
    else
        *number = m.number;
        strcpy(kit,m.kit);
    return 0;
}

void handler1(int num)
{
    printf("Real sent a signal with number %d to the UEFA\n",num);
}
void handler2(int num)
{
    printf("Dortmund sent a signal with number %d to the UEFA\n",num);
}

int main(int argc, char *argv[])
{
    int status;
    key_t key = ftok(argv[0], 80801221);
    int msg = msgget(key, 0600 | IPC_CREAT);

    int pipeRLM[2];
    int pipeDOR[2];
    if (pipe(pipeRLM) == -1)
    {
        perror("Error while creating the pipe\n");
        exit(1);
    }
    if (pipe(pipeDOR) == -1)
    {
        perror("Error while creating the pipe\n");
        exit(1);
    }

    signal(SIGUSR1, handler1);
    signal(SIGUSR2, handler2);

    int real = fork();
    if (real < 0)
    {
        perror("Error while forking process\n");
        exit(1);
    }
    if (real > 0) // UEFA
    {
        int dortmund = fork();
        if (dortmund < 0)
        {
            perror("Error while forking process\n");
            exit(1);
        }
        if (dortmund > 0) // UEFA
        {
            printf("U - The UEFA is waiting for the teams to be ready\n");

            pause();
            pause();

            printf("U - The UEFA is now sending the questions\n");

            close(pipeRLM[0]);

            int l = 92;
            char question[92] = "What colour jersey do you plan to wear and how many tickets will you ask your fans to buy?";
            write(pipeRLM[1], &l, sizeof(l));
            write(pipeRLM[1], question, l);
            printf("U - UEFA sent the questions to the Real\n");

            close(pipeRLM[1]);

            close(pipeDOR[0]);
            write(pipeDOR[1], &l, sizeof(l));
            write(pipeDOR[1], question, l);
            printf("U - UEFA sent the questions to the Dortmund\n");

            close(pipeDOR[1]);
            // Waiting for the children to terminate
            int number;
            char kit[MAX_BUFFER];
            receive(msg, &number,kit,11);
            printf("U - This is the number for Dortmund %d and this is the chosen kit %s\n", number,kit);
            receive(msg, &number,kit,10);
            printf("U - This is the number for Real %d and this is the chosen kit %s\n", number,kit);



            status = msgctl(msg, IPC_RMID, NULL);

            int Status;
            wait(&Status);
        }
        else // Dortmund
        {
            sleep(1);

            printf("D-The Dortmund is ready\n");
            kill(getppid(), SIGUSR2);

            close(pipeDOR[1]);

            int l;
            char question[MAX_BUFFER];
            read(pipeDOR[0], &l, sizeof(l));
            read(pipeDOR[0], question, l);
            printf("D-The Dortmund receive the question:\n");
            printf("%s\n", question);


            srand(time(NULL));
            char kit[13];

            int random = rand() % 3 + 1;
            switch (random)
            {
                case(1):
                    strcpy(kit, "yellow-black");
                    break;
                case(2):
                    strcpy(kit,"red-white");
                    break;
                case(3):
                    strcpy(kit,"green-white");
                    break;
            }

            int number = rand() % 100 + 1;
            if(number <= 33)
            {
                number = 5;
            }
            else if(number <= 66 )
            {
                number = 7;
            }
            else
            {
                number = 12000;
            }


            send(msg, number,kit,11);
            printf("D-The Dortmund answered the question\n");
            close(pipeDOR[0]);
        }
    }
    else // Real
    {
        sleep(2);

        printf("R-The Real is ready\n");
        kill(getppid(), SIGUSR1);

        close(pipeRLM[1]);

        int l;
        char question[MAX_BUFFER];
        read(pipeRLM[0], &l, sizeof(l));
        read(pipeRLM[0], question, l);
        printf("R-The Real receive the question:\n");
        printf("%s\n", question);

        srand(time(NULL));
        char kit[13];

        int random = rand() % 3 + 1;
        switch (random)
        {
            case(1):
                strcpy(kit, "yellow-black");
                break;
            case(2):
                strcpy(kit,"red-white");
                break;
            case(3):
                strcpy(kit,"green-white");
                break;
        }

        int number = rand() % 100 + 1;
        if(number <= 33)
        {
            number = 8;
        }
        else if(number <= 66 )
        {
            number = 10;
        }
        else
        {
            number = 15000;
        }


        send(msg, number,kit,10);
        printf("R-The Real answered the question\n");

        close(pipeRLM[0]);
    }
    return 0;
}
