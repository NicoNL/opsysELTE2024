
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

#define MAX_BUFFER 1000
#define FILENAME "Poems.txt"

struct Msg
{
    long priority;          // category of the msg
    char mtext[MAX_BUFFER]; // this is the message itself
};

int send(int mqueue, char *poem);
int receive(int mqueue, char *poem);

void Menu();
void pressEnter();
void addPoem();
void removePoem();
void modifyPoem();
int printPoem(bool number);
int waterPoem();

void handler1(int num);
void handler2(int num);

int main()
{
    Menu();
    return 0;
}

// Program Menu
void Menu()
{
    int x = 0;
    char input[30];
    FILE *Poems = fopen(FILENAME, "r");
    if (Poems == NULL)
    {
        printf("The file 'Poems.txt' is not in this folder, a new empty 'Poems.txt' file will be created\n");
        FILE *newPoems = fopen(FILENAME, "w");
        if (newPoems == NULL)
        {
            printf("Error while opening file\n");
            exit(1);
        }
        fclose(newPoems);
        pressEnter();
    }
    fclose(Poems);
    do
    {
        system("clear");
        printf("Welcome to the Watering Poems Manager\nPlease enter a number to choose the corresponding option:\n");
        printf("1.Add a new Poem\n2.Remove a Poem\n3.Modify an existing Poem\n4.Print Poems\n5.Water a Poem\nEnter 'exit' to leave\n\n");
        fgets(input, sizeof(input), stdin);
        if (strcmp(input, "exit\n") == 0)
        {
            system("clear");
            printf("Exiting...\n");
            break;
        }
        if (x = atoi(input))
        {
            system("clear");
            switch (x)
            {
            case 1:
                addPoem();
                x = 0;
                break;
            case 2:
                removePoem();
                break;
            case 3:
                modifyPoem();
                break;
            case 4:
                printPoem(false);
                break;
            case 5:
                x = waterPoem();
                break;
            default:
                printf("Please enter a valid number\n");
                break;
            }
            if (x != 1)
            {
                x = 0;
                pressEnter();
            }
        }
        else
        {
            system("clear");
            printf("Please enter a number\n");
            pressEnter();
        }
    } while (x == 0);
    if (x != 1)
    {
        printf("Program finished successfully\n");
    }
}

// Main Methods
void addPoem()
{
    FILE *Poems = fopen(FILENAME, "a");
    if (Poems == NULL)
    {
        printf("Poems file has been removed from the folder or is corrupt, please create a new file.\n");
        exit(1);
    }
    printf("Please enter a new Poem:\n");
    char newPoem[300];
    fgets(newPoem, sizeof(newPoem), stdin);
    while (strlen(newPoem) < 2)
    {
        system("clear");
        printf("Please enter a valid Poem:\n");
        fgets(newPoem, sizeof(newPoem), stdin);
    }
    fprintf(Poems, newPoem);
    fclose(Poems);
    printf("Poem was added successfully\n");
}

void removePoem()
{
    FILE *Poems = fopen(FILENAME, "r");
    if (Poems == NULL)
    {
        printf("Poems file has been removed from the folder or is corrupt, please create a new file.\n");
        exit(1);
    }
    // We check where is the pointer of the file located, if it's located at the beggining of the file
    // it means the file is empty, therefore no modifications can be done
    fseek(Poems, 0, SEEK_END);
    long size = ftell(Poems);

    if (size == 0)
    {
        printf("The file has no Poems, please add a Poem first to delete it.\n");
    }
    else
    {
        // Set the pointer of the file back to the beginning
        fseek(Poems, 0, SEEK_SET);
        int x;
        char input[20];
        printf("These are the current poems in the file:\n");
        int cnt = printPoem(true);
        printf("Please enter a number to delete the corresponding poem:\n");
        fgets(input, sizeof(input), stdin);

        while (!(x = atoi(input)) || (x <= 0 || x > cnt))
        {
            printf("Please enter a valid number:\n");
            fgets(input, sizeof(input), stdin);
        }
        char buffer[MAX_BUFFER];
        FILE *tempF = fopen("temp.txt", "w");
        if (tempF == NULL)
        {
            printf("Error while opening temp file\n");
            exit(1);
        }
        bool reading = true;
        int current = 1;
        while (fgets(buffer, sizeof(buffer), Poems) != NULL)
        {
            if (current != x)
            {
                fprintf(tempF, buffer);
            }
            current++;
        }
        fclose(tempF);
        fclose(Poems);
        remove(FILENAME);
        rename("temp.txt", FILENAME);
        printf("Poem was removed successfully\n");
    }
}

void modifyPoem()
{
    FILE *Poems = fopen(FILENAME, "r");
    if (Poems == NULL)
    {
        printf("Poems file has been removed from the folder or is corrupt, please create a new file.\n");
        exit(1);
    }
    // We check where is the pointer of the file located, if it's located at the beggining of the file
    // it means the file is empty, therefore no modifications can be done
    fseek(Poems, 0, SEEK_END);
    long size = ftell(Poems);

    if (size == 0)
    {
        printf("The file has no Poems, please add a Poem first to modify it.\n");
    }
    else
    {
        // Set the pointer of the file back to the beginning
        fseek(Poems, 0, SEEK_SET);
        int x;
        char input[20];
        printf("These are the current poems in the file:\n");
        int cnt = printPoem(true);
        printf("Please enter a number to modify the corresponding poem:\n");
        fgets(input, sizeof(input), stdin);

        while (!(x = atoi(input)) || (x <= 0 || x > cnt))
        {
            printf("Please enter a valid number:\n");
            fgets(input, sizeof(input), stdin);
        }
        char buffer[MAX_BUFFER];
        system("clear");
        printf("Please enter a new Poem:\n");
        char newPoem[300];
        fgets(newPoem, sizeof(newPoem), stdin);
        while (strlen(newPoem) < 2)
        {
            printf("Please enter a valid Poem:\n");
            fgets(newPoem, sizeof(newPoem), stdin);
        }

        FILE *tempF = fopen("temp.txt", "w");
        if (tempF == NULL)
        {
            printf("Error while opening temp file\n");
            exit(1);
        }
        bool reading = true;
        int current = 1;
        while (fgets(buffer, sizeof(buffer), Poems) != NULL)
        {

            if (current == x)
            {
                fprintf(tempF, newPoem);
            }
            else
            {
                fprintf(tempF, buffer);
            }
            current++;
        }
        fclose(tempF);
        fclose(Poems);
        remove("FILENAME");
        rename("temp.txt", FILENAME);
        printf("Poem was modifed successfuly\n");
    }
}

// This method takes a boolean to print or not the enumeration of the Lines and it returns an int to return
// the amount of lines in the file
int printPoem(bool number)
{
    FILE *Poems = fopen(FILENAME, "r");
    char buffer[MAX_BUFFER];
    int cnt = 0;
    if (Poems == NULL)
    {
        printf("Poems file has been removed from the folder or is corrupt, please create a new file.\n");
        exit(1);
    }
    fseek(Poems, 0, SEEK_END);
    long size = ftell(Poems);
    if (size == 0)
    {
        printf("The file has no Poems, please add a Poem first to print it.\n");
    }
    else
    {
        printf("------------------------------------------POEMS------------------------------------------.\n");
        // In case the file is not empty we set the pointer at the beginning of the file again
        fseek(Poems, 0, SEEK_SET);
        while (fgets(buffer, sizeof(buffer), Poems) != NULL)
        {
            // In case we want to list the poems we require a boolean
            cnt++;
            if (number)
            {
                printf("%d. ", cnt);
            }
            else
            {
                printf("- ");
            }
            printf("%s", buffer);
        }
        if (!number)
        {
            printf("\n\nTotal number of Poems: %d\n", cnt);
        }
        printf("\n");
    }
    fclose(Poems);
    return cnt;
}

int waterPoem()
{
    int cnt = printPoem(false); // we show the user the current poems and obtain the count from it
    if (cnt > 1) // if the count is less than 2 then this option wont run bc the user can delete the only single poem in the file
    {
        key_t key; // declaration of the key for the msg
        char cwd[1024]; // buffer for saving the current path of the file, because we're not using argv[0]
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            strcat(cwd, "/a.out"); // adding the name of the program to the path
        }
        else
        {
            perror("Error while obtaining the path\n");
            exit(1);
        }


        int status;                     // msg status
        key = ftok(cwd, 7123217); // create the key
        int msg = msgget(key, 0600 | IPC_CREAT); // new message queue

        if (msg < 0) // in case the file was named different
        {
            if (getcwd(cwd, sizeof(cwd)) != NULL)
            {
                strcat(cwd, "/PoemsRecorder"); // adding the name of the program to the path
                key = ftok(cwd, 7123217); 
            }
            if(msg < 0){
                perror("Error while creating a new message queue\nPlease make sure to compile the program with the default name (a.out)");
                exit(1);
            }
        }
        printf("Used Key: %d\n", key);
        
        char pipepath[25] = "/tmp/NicolasUniquePipe"; //declaring and defining my pipe
        int myPipe = mkfifo(pipepath, S_IRUSR | S_IWUSR); // making sure JUST the current user has the permission to read and write on this Pipe
        int result; // for opening the pipe
        if (myPipe == -1)
        {
            // Error given when file could not been created because it already exists so it's being re-used
            if (errno == 17)
            {
                printf("The given pipe is being re-used\n");
            }
            else
            {
                printf("Error number: %i", errno);
                exit(1);
            }
        }
        signal(SIGUSR1, handler1); // chd - parent
        signal(SIGUSR2, handler2); // parent - chd

        char buffer[MAX_BUFFER]; // Buffer for reading from the file

        pid_t child = fork(); //fork begins here!

        if (child < 0)
        {
            perror("The fork was not successful\n");
            exit(1);
        }
        if (child > 0) // Mama Bunny
        {
            printf("♛ - Most talented bunny boy was sent to Barátfa\n\n");
            pause();
            printf("\n♛ - Signal arrived to Mama bunny\n");

            srand(time(NULL)); // seed to generate random numbers
            int index1, index2;

            index1 = rand() % cnt + 1; // (0 -cnt-1) +1
            do
            {
                index2 = rand() % cnt + 1;
            } while (index2 == index1);

            FILE *Poems = fopen(FILENAME, "r");

            if (Poems == NULL)
            {
                printf("Poems file has been removed from the folder or is corrupt, please create a new file.\n");
                exit(1);
            }
            int line = 1;
            result = open(pipepath, O_WRONLY);
            while (fgets(buffer, sizeof(buffer), Poems) != NULL)
            {
                if (line == index1 || line == index2)
                {
                    int l = strlen(buffer);
                    write(result, &l, sizeof(int)); // write the length of the poem in the pipe
                    write(result, buffer, l); // write the poem in the pipe
                    printf("\n♛ - Mama bunny wrote a poem in the pipe\n");
                    sleep(1);
                    kill(child, SIGUSR2); // Let the child know we have written a something in the pipe
                    pause();
                }
                line++;
            }

            char poemToDelete[MAX_BUFFER];

            
            receive(msg, poemToDelete);
            
            fseek(Poems, 0, SEEK_SET);            // Set the pointer back to the file to beginning
            FILE *tempF = fopen("temp.txt", "w"); // Create a new temp file
            if (tempF == NULL)
            {
                printf("Error while opening temp file\n");
                exit(1);
            }
            while (fgets(buffer, sizeof(buffer), Poems) != NULL)
            {
                if (strcmp(poemToDelete, buffer) != 0) // print those lines diff from the line we want to delete
                {
                    fprintf(tempF, buffer);
                }
            }
            fclose(tempF);
            fclose(Poems);
            remove(FILENAME);
            rename("temp.txt", FILENAME);

            printf("\n♛ - Poem was removed successfully\n\n");

            status = msgctl(msg, IPC_RMID, NULL); // delete the message queue
            unlink(pipepath);
            close(result);

            sleep(2);

            return 0;
        }
        else // Bunny Boy
        {
            sleep(1);
            printf("♟-Bunny boy is walking to Barátfa:\n");
            sleep(1);
            printf(" ♟\n");
            sleep(1);
            printf("  ♟\n");
            sleep(1);
            printf(" ♟\n");
            sleep(1);
            printf("♟-Bunny boy arrived to Barátfa\n");
            sleep(1);
            kill(getppid(), SIGUSR1);

            char poem1[MAX_BUFFER];
            char poem2[MAX_BUFFER];
            result = open(pipepath, O_RDONLY); // opening pipe
            pause(); // Waiting for the signal for reading from the pipe
            int l;

            read(result, &l, sizeof(l));
            read(result, buffer, l);
            strcpy(poem1, buffer); // Copy the Poem from the pipe to poem1
            poem1[l - 1] = '\0'; //we do this because when we encounter chars with trash when copying
            printf("♟-Bunny boy received the first Poem: %s\n", poem1);
            sleep(3);
            kill(getppid(), SIGUSR1);
            pause(); // Waiting for the signal for reading from the pipe againg

            read(result, &l, sizeof(l));
            read(result, buffer, l);
            strcpy(poem2, buffer); // Copy the Poem from the pipe to poem1
            poem2[l - 1] = '\0';
            printf("♟-Bunny boy received the second Poem: %s\n", poem2);
            sleep(3);
            kill(getppid(), SIGUSR1);

            close(result); //we close the pipe

            srand(time(NULL)); //seed for the obtaining random numbers , using seconds since 1970 until today
            int poemNum = rand() % 2 + 1; // numbers from (0-1) +1
            char selectedPoem[MAX_BUFFER];

            if (poemNum == 1)
            {
                strcpy(selectedPoem, poem1);
            }
            else
            {
                strcpy(selectedPoem, poem2);
            }
            sleep(1);
            send(msg, selectedPoem);
            printf("♟-Bunny boy sent a poem to Mama Bunny in a message queue\n");
            sleep(1);
            printf("♟-Bunny boy chose the poem: %s\n", selectedPoem);
            printf("♟-May I water!\n\n");

            return 1; //return one to let child process go out of the do-while loop
        }
    }else{
        printf("This option requires 2 poems, please add more poems\n");
        return 0;
    }
}
// Complementary Methods
void pressEnter()
{
    printf("Press Enter to continue...\n");
    while (getchar() != '\n');
    system("clear");
}
int send(int mqueue, char *poem)
{
    struct Msg m;
    m.priority = 17;
    strcpy(m.mtext, poem);
    int status;

    status = msgsnd(mqueue, &m, strlen(m.mtext) + 1, 0);

    if (status < 0)
        perror("Error while using function msgsnd\n");

    printf("♟-Bunny boy sent a message with the code : %ld\n", m.priority);
    return 0;
}
int receive(int mqueue, char *poem)
{
    struct Msg m;
    int status;

    status =  msgrcv(mqueue, &m, MAX_BUFFER, 17, 0);

    if (status < 0)
        perror("Error while using function msgsnd\n");
    else
        printf("\n♛ - Mama Bunny received a message with the code : %ld\n", m.priority);
    strcpy(poem, m.mtext);
    poem[strlen(m.mtext) ] = '\n'; //This is for making the comparison in line 432 succeful because the file has lines with \n
    poem[strlen(m.mtext) +1 ] = '\0'; // Making sure the char after the new \n is the end of the string '\0'
    return 0;
}

void handler1(int num)
{
    printf("♟-Signal with number %d was sent to Mama Bunny\n", num);
}
void handler2(int num)
{
    printf("\n♛ - Signal with number %d was sent to Bunny boy\n\n", num);
}