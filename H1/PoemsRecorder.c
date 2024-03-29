#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_BUFFER 1000

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

void Menu(){
    int x = 0;
    char input[20];
    FILE *Poems = fopen("Poems.txt","r");
    if(Poems == NULL){
        printf("The file 'Poems.txt' is not in this folder, please create this file to save the poems.\n");
        exit(1);
    }
    fclose(Poems);
    do{
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
                x=0;
                pressEnter();
                break;
            case 2:
                removePoem();
                x=0;
                pressEnter();
                break;
            case 3:
                modifyPoem();
                x=0;
                pressEnter();
                break;
            case 4:
                printPoem(false);
                x=0;
                pressEnter();
                break;
            default:
                printf("Please enter a valid number\n");
                x =0;
                pressEnter();
                break;
            }
        }
        else{
            system("clear");
            printf("Please enter a number\n");
            pressEnter();
        }
    }while(x == 0);
    printf("Program finished successfully\n");
}

//Functions
void addPoem(){
    printf("Poem was added\n");
}

void removePoem(){
    printf("Poem was removed\n");
}

void modifyPoem(){
    FILE *Poems = fopen("Poems.txt","r");
    if(Poems == NULL){
        printf("Poems file has been removed from the folder or is corrupt, please create a new file.\n");
        exit(1);
    }
    //We check where is the pointer of the file located, if it's located at the beggining of the file
    //it means the file is empty, therefore no modifications can be done
    fseek(Poems, 0, SEEK_END);
    long size = ftell(Poems);
    fclose(Poems);
    if(size == 0){
        printf("The file has no Poems, please add a Poem first to modify it.\n"); 
    }
    else{
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

        FILE *Poems = fopen("Poems.txt","r+");
        fseek(Poems, 0, SEEK_SET);
        char buffer[MAX_BUFFER];
        int current = 1;

        while (fgets(buffer, sizeof(buffer), Poems) != NULL) {
        if (current == x) {
            printf("Please enter a new poem\n");
            char newPoem[MAX_BUFFER];
            fgets(newPoem, sizeof(newPoem), stdin);
            //Moving back the pointer of the file at the begginning of the line
            fseek(Poems, -strlen(buffer), SEEK_CUR);
            fputs(buffer, Poems);
            fclose(Poems);
            break; 
        }
        current++;
        printf("this cnt: %d\n", cnt);
        printf("this current: %d\n", current);
        }   
    }
}
int printPoem(bool number){
    FILE *Poems = fopen("Poems.txt","r");
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
    printf("Press Enter to continue...");
    while (getchar() != '\n');
    system("clear");
}