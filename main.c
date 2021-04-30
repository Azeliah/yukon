#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "fileReader.c"


int run = 1;

typedef struct Card Card;
struct Card
{
    struct Card* next; // Point to next card in current link list, where this is located.
    struct Card* prev; // And the previous card. NULL if they don't exist.
    int stackPosition; // Provided by push function.
    int listID; // Provided by push function
    int numRank; // Provided on data import and "typecast" (createCard)
    char suit; // Provided on data import and "typecast" (createCard)
    char rank; // Provided on data import and "typecast" (createCard)
    int hidden; // Provided when setting up new game.
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

CardList foundations[4];

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

    foundations[0] = f1;
    foundations[1] = f2;
    foundations[2] = f3;
    foundations[3] = f4;
}

Card* getCLCard (CardList* cardList, int depth){

    if (cardList->size < depth) {
        return NULL;
    } else {
        Card* card = cardList->head;
        for (int i = 0; i < depth ; i++)
        {
            card = card->next;
        }
        return card;
    }
}

void draw(){

    system("clear");
    printf("C1\tC2\tC3\tC4\tC5\tC6\tC7\n\n");
    int rowMax = 7;
    for (int i = 0 ; i < 7; i++)
    {
        if (grid[i].size > rowMax){ rowMax = grid[i].size; }
    }
    
    for (int i = 0; i < rowMax; i++)
    {
        for (int j = 0; j < 7 ; j++)
        {
            Card* card = getCLCard(&grid[j], i);
            if(card != NULL){
                printf("%c%c", card->rank, card->suit);
            }
            printf("\t");
        }

        printf("\t");

        // Rows 0, 2, 4, 6 should have foundations.
        if (i % 2 == 0 && i < 7) {
            if (foundations[i/2].tail == NULL) {
                printf("[]\t");
            } else {
                printf("%c%c\t",foundations[i/2].tail->rank, foundations[i/2].tail->suit);
            }
            printf("F%d", i/2 + 1);
        }
        printf("\n");
    }
    printf("\nLAST Command: "); // TODO: lastCommandHandler()

    printf("\nMessage: "); // TODO: messageHandler().

    printf("\nINPUT > ");
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
        printf("Trying to pop empty list. ListID: %d.\n",cardList->listID);
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
        for (int i = 0; i < cards; i++)
        {
            moveCard(fromColumn, &tempList);
        }
        for (int i = 0; i < cards; i++)
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

Card createCard (char* cardString){

    // char offset 0 = A,2,3,4,5,6,7,8,9,T,J,Q,K
    // char offset 1 = C,D,S,H
    // provide numRank, suit, rank

    Card card;
    card.rank = *(cardString);
    card.suit = *(cardString + 1);
    switch (card.rank)
    {
    case 'A':
        card.numRank = 1;
        break;
    case 'T':
        card.numRank = 10;
        break;
    case 'J':
        card.numRank = 11;
        break;
    case 'Q':
        card.numRank = 12;
        break;
    case 'K':
        card.numRank = 13;
        break;
    default:
        card.numRank = (int) (card.rank) - 48;
        break;
    }
    return card;
}

void createGrid(){

    moveCard(&deck, &grid[0]);
    for (int i = 1; i < 7; i++) 
    {
        for (int j = i; j < 7; j++)
        {
            // Later: placeHidden()
            moveCard(&deck, &grid[j]);
        }
    }

    for (int i = 0; i < 5; i++)
    {
        for (int j = 1; j < 7; j++)
        {
            moveCard(&deck, &grid[j]);
        }
    }
}

void createGame(char* gameID) {

    int a = (int) (*(gameID)) - 48;
    int b = (int) (*(gameID + 1)) - 48;
    a = a*10 + b;

    if (a > -1 && a < 100) {
        a = 1;
    } else {
        a = 0;
    }

    char** cards;

    if (a) {
        cards = fileReader(gameID);
    } else {
        cards = fileReader("00");
    }

    Card* castCards = malloc(sizeof(Card) * 52);

    for (size_t i = 0; i < 52; i++)
    {
        castCards[i] = createCard(*(cards + 51 - i));
        push(&deck,&castCards[i]);
    }

    free(cards);

    createGrid();
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
}

int main() 
{
    gameInit();

    while (run){
        draw();

        inputHandler();
    }
    
    printf("\n\nFinished.\n");

    return 0;
}
