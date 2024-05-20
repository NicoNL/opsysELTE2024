#include <stdio.h>
#include <unistd.h>   // FORK
#include <sys/wait.h> // PID code
#include <stdlib.h>   // exit(1)
#include <signal.h> //Signals

void handler1(int num);
void handler2(int num);

#define MAX_BUFFER 1024
int main(int argc, char *argv[])
{
    int myPipe[2];
    char buffer[MAX_BUFFER];

    if(pipe(myPipe) == -1)
    {
        perror("Error while opening unamed Pipe\n");
        exit(1);
    }
    
    signal(SIGUSR1, handler1);
    signal(SIGUSR2, handler2);

    if (argc != 2)
    {
        printf("Invalid number of arguments\n");
        exit(1);
    }

    pid_t chd1 = fork();

    if (chd1 < 0)
    {
        perror("The fork was not successful\n");
        exit(1);
    }
    if (chd1 > 0) 
    {
        pid_t chd2 = fork();
        if(chd2 < 0)
        {
            perror("The fork was not succesful\n");
            exit(1);
        }
        if(chd2> 0) //THE TRUSTEE
        {
            printf("The trustee is waiting for the expeditions to arrived\n");
            pause(); // WAITING FOR A SIGNAL
            pause(); // WAITING FOR A SIGNAL
            printf("Enter a string:\n");
            char hello[20];
            scanf(hello);
            printf("%s",hello);
             int *ptr = NULL;
            *ptr = 10; // Trying to access memory location pointed by NULL pointer
            return 0;

            int Status; //waiting for the expeditions to finalize
            wait(&Status);


        }
        else //1ST EXPEDITION
        {
            sleep(1);
            printf("The first expedition has arrived\n");
            kill(getppid(), SIGUSR1);

        }

        
    }
    else // 2ND EXPEDITION 
    {
        sleep(2);
        printf("The second expedition has arrived\n");
        kill(getppid(), SIGUSR2);


    }

    return 0;
}

void handler1(int num)
{
    printf("A signal with number %d was sent to the Trustee\n");

}
void handler2(int num)
{
    printf("A signal with number %d was sent to the Trustee\n");
    
}