#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "fileReader.c"
#include "linkedList.c"
#include "inputHelper.c"

int run = 1;
int clearEnabled = 1;
int gameWon;
int helpText = 1;

void gameInit(){
    gameWon = 0;

    gameInitialize();
}

char* lastMessage;
char* lastCommand;


void draw(){

    if(clearEnabled){
        system("clear");        
    }

    if (helpText) {
        printHelp();
        helpText = 0;
        return;
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
    printf("\nLAST Command: %s", lastCommand);

    printf("\nMessage: %s", lastMessage); 

    printf("\nINPUT > ");
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

int moveCardCommand(char* cardName, char* columnName){

    int cardNum = assertParameter(cardName,'c');
    int columnNum = assertParameter(columnName,'g');

    if (cardNum == -1 || columnNum == -1) {
        messageHandler("Wrong use of MV command. Type HP for more info on commands.");
        return 0;
    }
    
    // STEP 2: Find the specified card by searching for the numRank of the card
    // and then matching numSuit.
    int numSuit = (cardNum - 1) / 13;
    int numRank = cardNum - (13 * numSuit);
    
    Card* targetCard = searchCard(numRank, numSuit);

    int cardsToMove;

    // STEP 3: Determine if move is legal. Card has to have hidden == 0 and column
    // tail has to be one rank higher and off-suit.
    // STEP 4: If move is illegal, update displayMessage. If move is legal,
    // perform move and update lastCommand.

    if (targetCard == NULL) {
        messageHandler("Trying to move a card that is on a foundation.");
        return 0;
    } else if (targetCard->hidden) {
        messageHandler("Trying to move a card that hasn't been revealed");
        return 0;
    } else if (grid[columnNum].tail == NULL) {
        if (numRank == 13) {
            cardsToMove = grid[targetCard->listID - 1].size - targetCard->stackPosition + 1;
            moveStack(&grid[targetCard->listID - 1], &grid[columnNum], cardsToMove);
            messageHandler("Card(s) successfully moved.");
            return 1;
        } else {
            messageHandler("Trying to move a non-king card to an empty column.");
            return 0;
        }
    } else if (grid[columnNum].tail->numRank != numRank + 1 || grid[columnNum].tail->numSuit == numSuit) {
        messageHandler("Illegal move: Please try again.");
        return 0;
    } else {
        cardsToMove = grid[targetCard->listID - 1].size - targetCard->stackPosition + 1;
        moveStack(&grid[targetCard->listID - 1], &grid[columnNum], cardsToMove);
        messageHandler("Card(s) successfully moved.");
        return 1;
    }


}

int moveFoundation(char* columnName) {
    // STEP 1: Assert integrity of columnName and use second char of column 
    // name to reference grid index.
    int columnNum = assertParameter(columnName,'g');
    
    // STEP 2: Check if tail element can be moved to it's designated foundation.
    Card* targetCard = grid[columnNum].tail;
    
    if (targetCard == NULL) {
        return 0;
    }

    int foundationSize = foundations[targetCard->numSuit].size;
    if (targetCard->numRank != foundationSize + 1) {
        return 0;
    }

    // STEP 3: If move is legal, perform it and update lastCommand.

    moveCard(&grid[columnNum], &foundations[targetCard->numSuit]);

    int isGameWon = 1;

    for (int i = 0; i < 4; i++)
    {
        if (foundations[i].size != 13) {
            isGameWon = 0;
            break;
        }
    }
    if (isGameWon) {
        gameWon = 1;
    }

    return 1;
}

void showCommand(){

    gameInit();

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
        messageHandler("Loaded requested card file.");
        cards = fileReader(gameID);
    } else {
        messageHandler("Invalid file ID, loaded game 00 instead.");
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

    char* commandGiven = strdup(convert); // Duplicate convert so that it can be output later.
    
    command = str_split(convert, ' ');

    free(convert);
    
    if(*(command))
    {
        if(strlen(*(command)) == 2){
            if (!(strncmp(*(command), "LD", 2))){
                if (strlen(*(command + 1)) == 2) {
                    createGame(*(command + 1));
                } else {
                    messageHandler("Invalid file ID, loaded game 00 instead.");
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
                success = moveFoundation(*(command + 1));
                if (success) {
                    messageHandler("Target card moved to foundation.");
                } else {
                    messageHandler("Target card is not able to be moved to foundation.");
                }
            } else if (!(strncmp(*(command), "HP", 2))){
                helpText = 1;
            } else if (!(strncmp(*(command), "SW", 2))){
                showCommand(); 
            } else if (!(strncmp(*(command), "TT", 2))){
                clearEnabled = 0;
                messageHandler("Console clears disabled!");
            } else if (!(strncmp(*(command), "QT", 2))){
                clearEnabled = 1;
                messageHandler("Console clears enabled!");
            }else if (!(strncmp(*(command), "RD", 2))){
                messageHandler("Returned to game.");
            } else {
                messageHandler("Invalid command. Type HP for more info on commands.");
                success--;
            }
        } else {
            messageHandler("Invalid command. Type HP for more info on commands.");
            success--;
        }

        int i = 0;
        for (i = 0; *(command + i); i++)
        {
            free(*(command + i));
        }
        free(command);
    } else {
        messageHandler("Invalid command. Type HP for more info on commands.");

        success--;
    }
    if (success){
        lastCommandHandler(commandGiven);
        if (gameWon) {
            messageHandler("Congratulations! You won the game!");
        }
    }
}

int main() 
{    
    gameInit();
    lastCommandHandler("No valid command given.");
    while (run){
        draw();

        inputHandler();
    }
    
    printf("\n\nFinished.\n");

    return 0;
}
