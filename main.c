#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define STOP 0


int run = 1;

typedef struct Card Card;
struct Card
{
    char suit;
    char rank;
};

typedef struct DoubleLinkedList DLL;
struct DoubleLinkedList
{
    // CAN WE IMPORT DOUBLELINKEDLISTS FROM HELP FILES??
};





char** str_split(char* str, const char delimiter){
    char** result = 0;
    size_t count = 0;
    char* tmp = str;
    char* lastDelim = 0;
    char delim[2];
    delim[0] = delimiter;
    delim[1] = 0;

    /* Determine number of elements */
    while (*tmp)
    {
        /*  
         *  If a character matches the delimiter, then set lastDelim to that char* 
         *  and increment counter.
         */
        if(delimiter == *tmp)
        {
            count++;
            lastDelim = tmp;
        }
        tmp++;
    }
    if (lastDelim == 0) {
        char** noDelims = malloc(sizeof(char*) * 2);
        *(noDelims) = strdup(str);
        return noDelims;
    }

        /*
         *  Increment count once or twice, then allocate memory on the heap.
         *  Making room for a possible end token and a terminating null value.
         */
    count += lastDelim < (str + strlen(str) - 1);

    count++;

    result = malloc(sizeof(char*) * count);

    size_t ident = 0;
    char* token = strtok(str, delim);

    while (token)
    {
        // Remove assertions.
        assert(ident<count);
        *(result+ident++) = strdup(token);

        token = strtok(0,delim);
    }
    assert(ident == count - 1);
    *(result + ident) = 0;

    return result;
}

void createGame(char* gameID) {
    // STEP 0: Input validation (is gameID 00-99? Else select 00.)
    int a = (int) (*(gameID)) - 48;
    int b = (int) (*(gameID + 1)) - 48;
    printf("a = %d, b = %d", a, b);
    a = a*10 + b;

    if (a > -1 && a < 100) {
        a = 1;
    } else {
        a = 0;
    }

    if (a) {
        // Load specified file.
    } else {
        // Load default file.
    }
    

    // STEP 1: Load data from file into structs.

    // STEP 2: Create double linked lists (columns 1-7 and foundations 1-4).

    // STEP 3: Populate linked lists with data from file.
}

void inputHandler(){
    /*
     *  "MV 7H C6"  Move 7 of hearts to column 6.
     *  "LD xx"     Load game xx (00-99, where 00 is sorted deck)
     *  "HP"        Help with commands
     */

    char input[9];
    char** command;
    int success = 1;

    fgets(input, sizeof(input) + 1, stdin);

    input[strcspn(input,"\n")] = 0;

    command = str_split(input, ' ');

    if(*(command))
    {
        if(strlen(*(command))==2){
            if (!(strncmp(*(command), "LD", 2))){
                printf("Asked to load file %s\n", *(command + 1));

                if (strlen(*(command + 1)) == 2) {
                    createGame(*(command + 1));
                } else {
                    createGame("00");
                }
                
            } else if (!(strncmp(*(command), "MV", 2))) {
                printf("Asked to move card %s to column %s.\n", *(command + 1), *(command + 2));
                // if(gameStarted)
                // moveCard(card, column);
            } else if (!(strncmp(*(command), "QQ", 2))){
                printf("Quitting program.");
                run--;
            } else {
                printf("Invalid command.\n");

                success--;
            }
        } else {
            printf("Invalid command.\n");

            success--;
        }

        // printf("Argument %d: %s\n",i,*(command + i));
        int i = 0;
        for (i = 0; *(command + i); i++)
        {
            free(*(command + i));
        }
        free(command);
    } else {
        printf("Invalid command.\n");
        
        success--;
    }
    /*
    if (success) {
        return *input;
    } else {
        return NULL;
    }
    */
}

int main() 
{
    char *lastCommand;
    do {
        
        printf("Command: ");

        // if(gameStarted) reDraw();

        // char *commandGiven = inputHandler();

        inputHandler();
        /*
        if (commandGiven) {
            strcpy(lastCommand, commandGiven);
        }
        printf("\nLast command given: %s\n\n", lastCommand);
        */

        /*
         *  Print out game state
         *  Print out last command
         *  Print out message statement
         *  Print out input query
         */
        
    } while (run);

    return 0;
}



