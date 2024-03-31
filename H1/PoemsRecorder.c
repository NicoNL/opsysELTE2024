#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_BUFFER 1000
#define FILENAME "Poems.txt"

void Menu();
void pressEnter();
void addPoem();
void removePoem();
void modifyPoem();
int printPoem(bool number);

int main(){
    Menu();
    return 0;
}

//Program Menu
void Menu(){
    int x = 0;
    char input[30];
    FILE *Poems = fopen(FILENAME,"r");
    if(Poems == NULL){
        printf("The file 'Poems.txt' is not in this folder, a new empty 'Poems.txt' file will be created\n");
        FILE *newPoems = fopen(FILENAME,"w");
        if(newPoems == NULL){
            printf("Error while opening file\n");
            exit(1);
        }
        fclose(newPoems);
        pressEnter();
    }
    fclose(Poems);
    do{
        system("clear");
        printf("Welcome to the Watering Poems Manager\nPlease enter a number to choose the corresponding option:\n");
        printf("1.Add a new Poem\n2.Remove a Poem\n3.Modify an existing Poem\n4.Print Poems\nEnter 'exit' to leave\n\n");
        fgets(input,sizeof(input),stdin);
        if(strcmp(input, "exit\n") == 0){
            system("clear");
            printf("Exiting...\n");
            break;
        }
        if(x = atoi(input)){
            system("clear");
            switch (x)
            {
            case 1:
                addPoem();
                pressEnter();
                break;
            case 2:
                removePoem();
                pressEnter();
                break;
            case 3:
                modifyPoem();
                pressEnter();
                break;
            case 4:
                printPoem(false);
                pressEnter();
                break;
            default:
                printf("Please enter a valid number\n");
                pressEnter();
                break;
            }
            x=0;
        }
        else{
            system("clear");
            printf("Please enter a number\n");
            pressEnter();
        }
    }while(x == 0);
    printf("Program finished successfully\n");
}

//Main Methods
void addPoem(){
    FILE *Poems = fopen(FILENAME,"a");
    if(Poems == NULL){
        printf("Poems file has been removed from the folder or is corrupt, please create a new file.\n");
        exit(1);
    }
    printf("Please enter a new Poem:\n");
    char newPoem[300];
    fgets(newPoem,sizeof(newPoem),stdin);
    while(strlen(newPoem) < 2){
        system("clear");
        printf("Please enter a valid Poem:\n");
        fgets(newPoem,sizeof(newPoem),stdin);
    }
    fprintf(Poems,newPoem);
    fclose(Poems);
    printf("Poem was added successfully\n");
}

void removePoem(){
    FILE *Poems = fopen(FILENAME,"r");
    if(Poems == NULL){
        printf("Poems file has been removed from the folder or is corrupt, please create a new file.\n");
        exit(1);
    }
    //We check where is the pointer of the file located, if it's located at the beggining of the file
    //it means the file is empty, therefore no modifications can be done
    fseek(Poems, 0, SEEK_END);
    long size = ftell(Poems);

    if(size == 0){
        printf("The file has no Poems, please add a Poem first to delete it.\n"); 
    }
    else{
        //Set the pointer of the file back to the beginning
        fseek(Poems, 0,SEEK_SET);
        int x;
        char input[20];
        printf("These are the current poems in the file:\n");
        int cnt = printPoem(true);
        printf("Please enter a number to delete the corresponding poem:\n");
        fgets(input,sizeof(input),stdin);

        while( !(x = atoi(input)) || (x <= 0 || x > cnt)){
            printf("Please enter a valid number:\n");
            fgets(input,sizeof(input),stdin);
        }
        char buffer[MAX_BUFFER];
        FILE *tempF = fopen("temp.txt","w");
        if(tempF == NULL){
            printf("Error while opening temp file\n");
            exit(1);
        }
        bool reading = true;
        int current =1;
        while (fgets(buffer, sizeof(buffer), Poems) != NULL) {
            if(current != x){
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

void modifyPoem(){
    FILE *Poems = fopen(FILENAME,"r");
    if(Poems == NULL){
        printf("Poems file has been removed from the folder or is corrupt, please create a new file.\n");
        exit(1);
    }
    //We check where is the pointer of the file located, if it's located at the beggining of the file
    //it means the file is empty, therefore no modifications can be done
    fseek(Poems, 0, SEEK_END);
    long size = ftell(Poems);

    if(size == 0){
        printf("The file has no Poems, please add a Poem first to modify it.\n"); 
    }
    else{
        //Set the pointer of the file back to the beginning
        fseek(Poems, 0,SEEK_SET);
        int x;
        char input[20];
        printf("These are the current poems in the file:\n");
        int cnt = printPoem(true);
        printf("Please enter a number to modify the corresponding poem:\n");
        fgets(input,sizeof(input),stdin);

        while( !(x = atoi(input)) || (x <= 0 || x > cnt)){
            printf("Please enter a valid number:\n");
            fgets(input,sizeof(input),stdin);
        }
        char buffer[MAX_BUFFER];
        system("clear");
        printf("Please enter a new Poem:\n");
        char newPoem[300];
        fgets(newPoem,sizeof(newPoem),stdin);
        while(strlen(newPoem) < 2){
            printf("Please enter a valid Poem:\n");
            fgets(newPoem,sizeof(newPoem),stdin);
        }

        FILE *tempF = fopen("temp.txt","w");
        if(tempF == NULL){
            printf("Error while opening temp file\n");
            exit(1);
        }
        bool reading = true;
        int current =1;
        while (fgets(buffer, sizeof(buffer), Poems) != NULL) {
            if(current == x){
                fprintf(tempF, newPoem);
            }
            else{
                fprintf(tempF,buffer);
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

//This method takes a boolean to print or not the enumeration of the Lines and it returns an int to return
//the amount of lines in the file
int printPoem(bool number){
    FILE *Poems = fopen(FILENAME,"r");
    char buffer[MAX_BUFFER];
    int cnt = 0;
    if(Poems == NULL){
        printf("Poems file has been removed from the folder or is corrupt, please create a new file.\n");
        exit(1);
    }
    fseek(Poems, 0, SEEK_END);
    long size = ftell(Poems);
    if(size == 0){
        printf("The file has no Poems, please add a Poem first to print it.\n");
    }
    else{
        //In case the file is not empty we set the pointer at the beginning of the file again
        fseek(Poems,0,SEEK_SET);
        while (fgets(buffer, sizeof(buffer), Poems) != NULL) {
        //In case we want to list the poems we require a boolean
        cnt++;
        if(number){
            printf("%d. ", cnt);
        }
        printf("%s", buffer);
    }
    if(!number){
        printf("\n\nTotal number of Poems: %d", cnt);
    }
    printf("\n");
    }
    fclose(Poems);
    return cnt;
}

//Complementary Methods
void pressEnter() {
    printf("Press Enter to continue...\n");
    while (getchar() != '\n');
    system("clear");
}