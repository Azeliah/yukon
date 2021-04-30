#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


int run = 1;

typedef struct Card Card;
struct Card
{
    struct Card* next;
    struct Card* prev;
    int stackPosition;
    int listID;
    char suit;
    char rank;
    int hidden;
};

typedef struct CardList CardList;
struct CardList
{
    Card* head;
    Card* tail;
    int size;
    int listID;
};

CardList column1;
CardList column2;
CardList column3;
CardList column4;
CardList column5;
CardList column6;
CardList column7;

CardList grid[7];

CardList f1;
CardList f2;
CardList f3;
CardList f4;

// CardList sortedDeck;

CardList deck;


void initCardList (CardList* cardList, int listID){
    cardList->head = NULL;
    cardList->tail = NULL;
    cardList->size = 0;
    cardList->listID = listID;
}

void gameInit () {
    initCardList(&deck, 0);

    initCardList(&column1, 1);
    initCardList(&column2, 2);
    initCardList(&column3, 3);
    initCardList(&column4, 4);
    initCardList(&column5, 5);
    initCardList(&column6, 6);
    initCardList(&column7, 7);

    grid[0] = column1;
    grid[1] = column2;
    grid[2] = column3;
    grid[3] = column4;
    grid[4] = column5;
    grid[5] = column6;
    grid[6] = column7;

    initCardList(&f1, 8);
    initCardList(&f2, 9);
    initCardList(&f3, 10);
    initCardList(&f4, 11);
}


void push(CardList* cardList, Card* cardP){
    
    if (cardList->head == NULL){
        // printf("Head in cardList before first element: %p.\n", cardList->head);
        cardList->head = cardP; 
        cardList->tail = cardP; 
        cardList->head->next = NULL;
        cardList->head->prev = NULL;

        // if using dummy node, assign head->prev and head->next differently.
        // printf("Head in cardList after pushing first element: %p\n", cardList->head); 
    } else {
        cardList->tail->next = cardP;
        cardP->prev = cardList->tail;
        cardList->tail = cardP;

        // if using dummy node, include correcting cardP->next.
    }
    cardList->size++;
    cardP->stackPosition = cardList->size;
    cardP->listID = cardList->listID;
}

Card* pop(CardList* cardList) {
    if (cardList->size == 0) {
        printf("Trying to pop empty list.\n");
        return NULL;
    } else {
        Card* cardToPop = cardList->tail;
        // printf("CardToPop = %c%c\n", cardToPop->rank,cardToPop->suit);
        if(cardList->tail->prev != NULL) {
            Card* cardPrevP = cardList->tail->prev;
            cardList->tail->prev = NULL; 
            cardList->tail = cardPrevP;
            cardList->tail->next = NULL;
            cardList->size--;
            
            // if using dummy node, include correcting cardP->next.
        } else {
            initCardList(cardList, cardList->listID);
        }
        cardToPop->stackPosition = 0;
        return cardToPop;
    }
}

void moveCard(CardList* fromColumn, CardList* toColumn){
    Card* poppedCard = pop(fromColumn);
    if (poppedCard == NULL){
        printf("No card to move. \n");
    } else {
        push(toColumn, poppedCard);
    }
}

void moveStack(CardList* fromColumn, CardList* toColumn, int cards){
    if (fromColumn->size < cards) {
        printf("Trying to move more cards than there are available.");
    } else {
        CardList tempList;
        initCardList(&tempList, -1);
        for (size_t i = 0; i < cards; i++)
        {
            moveCard(fromColumn, &tempList);
        }
        for (size_t i = 0; i < cards; i++)
        {
            moveCard(&tempList, toColumn);
        }
    }
}

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
        // Load specified file using gameID.
    } else {
        // Load default file 00.txt.
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
    //char *lastCommand;

    gameInit();

    /*
    Card card1;
    card1.rank = 'A';
    card1.suit = 'H';

    Card card2;
    card2.rank = '2';
    card2.suit = 'D';

    Card card3;
    card3.rank = '3';
    card3.suit = 'S';

    Card card4;
    card4.rank = '4';
    card4.suit = 'C';

    Card card5;
    card5.rank = '5';
    card5.suit = 'H';
    */

    //do {
        
      //  printf("Command: ");

        // if(gameStarted) reDraw();

        // char *commandGiven = inputHandler();

      //  inputHandler();
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
        
    //} while (run);

    return 0;
}



