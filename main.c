#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "fileReader.c"

int run = 1;
int drawEnabled = 1;

typedef struct Card Card;
struct Card
{
    struct Card* next; // Point to next card in current link list, where this is located.
    struct Card* prev; // And the previous card. NULL if they don't exist.
    int stackPosition; // Provided by push function.
    int listID; // Provided by push function
    int numRank; // Provided on data import and "typecast" (createCard)
    int numSuit; // Provided on data import and "typecast" (createCard)
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

char* lastMessage;
char* lastCommand;

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

    if(drawEnabled){
        system("clear");        
    }

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
                if(card->hidden){
                    printf("[]");
                } else {
                printf("%c%c", card->rank, card->suit);
                }
            }
            printf("\t");
        }

        printf("\t");

        // Rows 0, 2, 4, 6 should have foundations displayed.
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
    printf("\nLAST Command: %s", lastCommand); // TODO: lastCommandHandler()

    printf("\nMessage: %s", lastMessage); 

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
        printf("Trying to pop empty list. ListID: %d.\n",cardList->listID); // TODO: Use messageHandler instead.
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

    if (!(fromColumn->tail == NULL)){
        fromColumn->tail->hidden = 0;
    }
}

void moveStack(CardList* fromColumn, CardList* toColumn, int cards){

    if (fromColumn->size < cards) {
        printf("Trying to move more cards than there are available."); // TODO: Use messageHandler instead
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
    card.hidden = 0;
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
    switch (card.suit)
    {
    case 'C':
        card.numSuit = 0;
        break;
    case 'D':
        card.numSuit = 1;
        break;
    case 'S':
        card.numSuit = 2;
        break;
    case 'H':
        card.numSuit = 3;
        break;
    default:
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
            moveCard(&deck, &grid[j]);
            grid[j].tail->hidden = 1;
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

void messageHandler(char* outputMessage){
    lastMessage = strdup(outputMessage);
}

void lastCommandHandler(char* outputMessage){
    lastCommand = strdup(outputMessage);
}

int isLetter(char input){
    // return 0 if not a letter, 1 if upper case, 2 if lower case
    int charNum = (int) (input);
    if (charNum > 64 && charNum < 91) { 
        return 1; 
    } else if (charNum > 96 && charNum < 123) {
        return 2;
    } else {
        return 0;
    }
}

char* convertToUpperCase(char* input) {
    size_t inputLength = strlen(input);
    char* convert = malloc(sizeof(char) * inputLength);
    int letter = 0;
    for (size_t i = 0; i < inputLength; i++)
    {
        letter = isLetter(*(input + i));
        if (letter == 2) {
            *(convert + i) = *(input + i) - 32;
        } else {
            *(convert + i) = *(input + i);
        }
    }
    return convert;
}

int assertParameter(char* input, char assertType){
    // input is ALWAYS 2 upper case characters.

    int value;

    switch (assertType)
    {
    case 'c':
        // Card type parameter. We want to ensure it has the card format, e.g. TH for 10 of hearts.
        // If card is correctly formatted, return the numeric value + offset divisible by 13 based on suit.

        switch (*(input))
        {
        case 'A':
            value = 1;
            break;
        case 'T':
            value = 10;
            break;
        case 'J':
            value = 11;
            break;
        case 'Q':
            value = 12;
            break;
        case 'K':
            value = 13;
            break;
        default:
            value = (int) (*(input)) - 48;
            if (value < 2 || value > 9){
                return -1;
            }
            break;
        }
        switch (*(input + 1))
        {
        case 'C':
            break;
        case 'D':
            value += 13;
            break;
        case 'S':
            value += 26;
            break;
        case 'H':
            value += 39;
            break;
        default:
            return -1;
            break;
        }

        break;
    
    case 'g':
        // Grid type parameter
        if (!(*(input) == 'C'))
        {
            return -1;
        }
        value = (int) (*(input + 1)) - 49; // Subtracting 1 on top of ASCII number offset.
        if (value < 0 || value > 6) {
            return -1;
        }

        break;
    
    default:

        break;
    }
    return value;

}

Card* searchCard(int numRank, int numSuit){
    
    if (foundations[numSuit].size > numRank) {
        printf("1\n");
        return NULL;
    }

    int columnSize;
    Card* currentCard;
    int cardFound = 0;

    for (int i = 0; i < 7; i++)
    {
        columnSize = grid[i].size;
        if(columnSize > 0) {
            currentCard = grid[i].tail;
        
            if (currentCard->numRank == numRank){
                if (currentCard->numSuit == numSuit)
                {
                    cardFound = 1;
                }

            }
            for (int j = 0; j < columnSize - 1; j++)
            {
                if (cardFound)
                {
                    break;
                }
                currentCard = currentCard->prev;
                if (currentCard->numRank == numRank){
                    if (currentCard->numSuit == numSuit)
                    {
                        cardFound = 1;
                    }
                }
            }
            if (cardFound)
            {
                break;
            }
        }
    }
    return currentCard;
}

int moveCardCommand(char* cardName, char* columnName){
    // TODO: Make this.

    int cardNum = assertParameter(cardName,'c');
    int columnNum = assertParameter(columnName,'g');

    if (cardNum == -1 || columnNum == -1) {
        // TODO: Post error message through messageHandler(). Input is not formatted properly.
        messageHandler("Wrong use of MV command. Type HP for more info on commands.");
        return 0;
    }
    
    // STEP 2: Find the specified card by searching for the numRank of the card
    // and then matching numSuit.
    int numSuit = (cardNum - 1) / 13;
    int numRank = cardNum - (13 * numSuit);
    
    Card* targetCard = searchCard(numRank,numSuit);

    int cardsToMove;

    // STEP 3: Determine if move is legal. Card has to have hidden == 0 and column
    // tail has to be one rank higher and off-suit.

    if (targetCard == NULL) {
        // TODO: Post error message through messageHandler(). Card is on foundation.
        // WISH: Moving top card on foundation back to the board.
        messageHandler("Trying to move a card that is on a foundation.");
        return 0;
    } else if (targetCard->hidden) {
        // TODO: Post error message through messageHandler(). Card has not been revealed.
        messageHandler("Trying to move a card that hasn't been revealed");
        return 0;
    } else if (grid[columnNum].tail == NULL) {
        if (numRank == 13) {
            cardsToMove = grid[targetCard->listID - 1].size - targetCard->stackPosition + 1;
            moveStack(&grid[targetCard->listID - 1], &grid[columnNum], cardsToMove);
            messageHandler("Card(s) successfully moved.");
            return 1;
        } else {
            // TODO: Post error message through messageHandler(). Trying to move non-king to empty column.
            messageHandler("Trying to move a non-king to an empty column.");
            return 0;
        }
    } else if (grid[columnNum].tail->numRank != numRank + 1 || grid[columnNum].tail->numSuit == numSuit) {
        // TODO: Post error message through messageHandler(). Target column doesn't meet requirements.
        messageHandler("Illegal move: Please try again.");
        return 0;
    } else {
        cardsToMove = grid[targetCard->listID - 1].size - targetCard->stackPosition + 1;
        moveStack(&grid[targetCard->listID - 1], &grid[columnNum], cardsToMove);
        messageHandler("Card(s) successfully moved.");
        return 1;
    }

    // STEP 4: If move is illegal, update displayMessage. If move is legal,
    // perform move and update lastCommand.

}

void moveFoundation(char* columnName) {
    // STEP 1: Assert integrity of columnName and use second char of column 
    // name to reference grid index.
    
    // STEP 2: Check if tail element can be moved to it's designated foundation.

    // STEP 3: If move is legal, perform it and update lastCommand.

    // STEP 4: If move is not legal, update displayMessage.
}

void showCommand(){
    // TODO: Implement this. Populate lists with 00.txt according to SW in
    // assignment. 

    char** cards;

    cards = fileReader("00");

    Card* castCards = malloc(sizeof(Card) * 52);

    for (size_t i = 0; i < 52; i++)
    {
        castCards[i] = createCard(*(cards + 51 - i));
        push(&deck,&castCards[i]);
    }

    free(cards);

    for (int j = 0; j < 52; j++)
    {
        moveCard(&deck, &grid[j % 7]);
    }
}

void createGame(char* gameID) {

    gameInit();

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
    char** commandTest;
    int success = 1;

    fgets(input, sizeof(input) + 1, stdin);

    char* convert = convertToUpperCase(input); // Convert any lower case characters to upper case characters.

    convert[strcspn(convert,"\n")] = 0;
    
    command = str_split(convert, ' ');

    free(convert);
    
    if(*(command))
    {
        if(strlen(*(command)) == 2){
            if (!(strncmp(*(command), "LD", 2))){
                printf("Asked to load file %s\n", *(command + 1));

                if (strlen(*(command + 1)) == 2) {
                    createGame(*(command + 1));
                } else {
                    createGame("00");
                }
                
            } else if (!(strncmp(*(command), "MV", 2))) {
                printf("Asked to move card %s to column %s.\n", *(command + 1), *(command + 2));
                if(strlen(*(command + 1)) == 2 && strlen(*(command + 2)) == 2){
                    success = moveCardCommand(*(command + 1), *(command + 2)); 
                } else {
                    messageHandler("Wrong use of MV command. Type HP for more info on commands.");
                    success--;
                    }

            } else if (!(strncmp(*(command), "QQ", 2))){
                printf("Quitting program.");
                run--;
            } else if (!(strncmp(*(command), "FD", 2))){
                moveFoundation(*(command + 1)); // TODO: Placeholder, see method.
            } else if (!(strncmp(*(command), "HP", 2))){
                // TODO: Maybe use draw to do this, and just not draw the cards? Help flag?
            } else if (!(strncmp(*(command), "SW", 2))){
                showCommand(); // TODO: PlaceHolder
            } else if (!(strncmp(*(command), "TT", 2))){
                drawEnabled = 0;
            } else if (!(strncmp(*(command), "QT", 2))){
                drawEnabled = 1;
            } else {
                // TODO: Make use of messageHandler
                
                printf("Invalid command.\n");

                success--;
            }
        } else {
            // TODO: Make use of messageHandler
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
        // TODO: Make use of messageHandler
        printf("Invalid command.\n");
        
        success--;
    }
    if (success){
        lastCommandHandler(input);
    }
}

int main() 
{    
    gameInit();
    lastCommandHandler("No commands given.");
    messageHandler("Welcome! Type HP to show available commands.");

    printf("\n\nFirst command: ");
    inputHandler(); // draw() clears output, so this is here to ensure we get output before the clear happens and should be removed later.

    while (run){
        draw();

        inputHandler();
    }
    
    printf("\n\nFinished.\n");

    return 0;
}
