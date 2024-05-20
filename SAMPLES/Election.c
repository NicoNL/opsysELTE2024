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



void chd2pres(int num);
void chd1pres(int num);
void chd1chd2(int num);
int receive(int mqueue, int *number);
int send(int mqueue, int num);
bool isRepeated(int *array, int number, int index);

struct Msg
{
    long priority;
    int number;
};

// THINGS TO DO OPEN THE NAMED PIPE IN THE CORRESPONDING MODE, SEND INFO TRHOUGH IT AND
//  CLOSE IT IN EACH OF THE CHILDREN
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Not enough arguments were given, please write one 'int' argument\n");
        exit(1);
    }
    if (atoi(argv[1]) < 2)
    {
        printf("Invalid number of voters");
        exit(1);
    }

    int status;
    key_t key = ftok(argv[0], 17042000);
    int msg = msgget(key, 0600 | IPC_CREAT);
    int voters = atoi(argv[1]);
    int votersArr[voters];

    signal(SIGUSR1, chd2pres);
    signal(SIGUSR2, chd1pres);
    signal(SIGTERM, chd1chd2);

    int pipeID[2], result;
    char ID[4];
    pid_t chd2 = fork();
    char pipepath[28] = "/tmp/uniquePipeNeverCreated";

    int fid = mkfifo(pipepath, S_IRUSR | S_IWUSR);
    if (fid == -1)
    {
        if (errno == 17)
        {
            // printf("The pipe has been previously created, it will be re-used\n");
        }
        else
        {
            printf("Error number: %i", errno);
            exit(1);
        }
    }
    if (pipe(pipeID) == -1)
    {
        perror("Error while opening the pipe!");
        exit(1);
    }

    if (chd2 < 0)
    {
        perror("The fork was not successful\n");
        exit(1);
    }
    if (chd2 > 0) // President
    {
        pid_t chd1 = fork();
        if (chd1 < 0)
        {
            perror("The fork was not successful\n");
            exit(1);
        }
        if (chd1 > 0) // The president
        {
            printf("- ♚ The president is now waiting for the other members\n");
            close(pipeID[0]);
            // 1. Waiting for the two child signals
            pause();
            pause();

            printf("- ♚ The president now is ready to work\n");
            // 1. ID CREATION
            srand(time(NULL));

            for (int i = 0; i < voters; i++)
            {
                int x;
                do
                {
                    x = rand() % 999 + 100;
                } while ((isRepeated(votersArr, x, i)));

                write(pipeID[1], &x, sizeof(x));
                votersArr[i] = x;
                sleep(1);
            }

            close(pipeID[1]);
            // 3. Waiting for the second child to send the random numbers
            pause();

            for (int i = 0; i < voters; i++)
            {
                int number;
                receive(msg, &number);
                if (number > 0)
                {
                    printf("- ♚ Voter number %d has the number %d\n", i + 1, number);
                    sleep(1);
                }
            }

            int Status;
            wait(&Status);
        }
        else // 1st member
        {
            sleep(1);
            close(pipeID[1]);

            printf("❶ The first member is ready to work\n");
            // 1. SIGNAL TO PRESIDENT

            kill(getppid(), SIGUSR2);
            sleep(1);

            // 1. ID PRINTING
            for (int i = 0; i < voters; i++)
            {
                sleep(1);
                int num;
                read(pipeID[0], &num, sizeof(num));
                printf("❶ Voter number %d ID: %d\n", i + 1, num);
            }

            // 2. SIGNAL TO SECOND CHILD TO START TASK 2
            kill(chd2, SIGTERM);

            // 2 . CAN // CANNOT VOTE WRITING
            result = open(pipepath, O_WRONLY);
            for (int i = 0; i < voters; i++)
            {
                srand(time(NULL));
                int erroProb = rand() % 100 + 1;
                if (erroProb <= 20)
                {
                    int vote = 0;
                    write(result, &vote, sizeof(vote));
                }
                else
                {
                    int vote = 1;
                    write(result, &vote, sizeof(vote));
                }
                sleep(1);
            }

            close(result);
            close(pipeID[0]);
        }
    }
    else // 2nd member
    {
        sleep(2);

        printf("❷ The second member is ready to work\n");

        // 1. SIGNAL TO PRESIDENT
        kill(getppid(), SIGUSR2);
        // WAIT UNTIL FIRST TASK IS FINISHED
        pause();
        // 2 . CAN // CANNOT VOTE PRINTING
        result = open(pipepath, O_RDONLY);
        for (int i = 0; i < voters; i++)
        {
            sleep(1);
            int vote;
            read(result, &vote, sizeof(vote));
            if (vote == 1)
            {
                votersArr[i] = 1;
                printf("❷ Voter number %d can vote\n", i + 1);
            }
            else
            {
                votersArr[i] = 0;
                printf("❷ Voter number %d cannot vote\n", i + 1);
            }
        }
        // 3. Signal to president to obtain random numbers
        kill(getppid(), SIGUSR2);
        for (int i = 0; i < voters; i++)
        {
            sleep(2);
            int x = 0;
            if (votersArr[i] == 1)
            {
                x = 1;
            }
            send(msg, x);
        }
        status = msgctl(msg, IPC_RMID, NULL);

        close(result);
    }
}

// SIGNALS HANDLERS
void chd2pres(int num)
{
    printf("A signal with number %d was sent to the president\n");
}
void chd1pres(int num)
{
    printf("A signal with number %d was sent to the president\n");
}
void chd1chd2(int num)
{
    printf("A signal with number %d was sent to the second member\n");
}
// MSG QUEUE FUNCTIONS
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
    status = msgsnd(mqueue, &m, sizeof(x), 0);
    if (status < 0)
        perror("Error sending msg: ");
    return 0;
}

int receive(int mqueue, int *number)
{
    struct Msg m;
    int status;

    status = msgrcv(mqueue, &m, sizeof(int), 5, 0);

    if (status < 0)
        perror("Error receiving the msg: ");
    else
        // printf( "- ♚ The president received a new message\n" );
        *number = m.number;
    return 0;
}
// Function to make sure the ID's are not repeated
bool isRepeated(int *array, int number, int index)
{
    for (int i = 0; i < index; i++)
    {
        if (array[i] == number)
        {
            return true;
        }
    }
    return false;
}

/*

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *generar_string() {
    char *cadena = (char *)malloc(50 * sizeof(char)); // Asignación de memoria para el string
    if (cadena == NULL) {
        printf("Error: No se pudo asignar memoria\n");
        exit(1);
    }
    strcpy(cadena, "Ejemplo de string generado por una función");
    return cadena;
}

int main() {
    char *mi_string = generar_string(); // Llamada a la función para obtener el string

    // Uso del string
    printf("El string generado es: %s\n", mi_string);

    // Liberar la memoria asignada al string
    free(mi_string);

    return 0;
}

*/