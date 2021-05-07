#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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

CardList deck;

void initCardList (CardList* cardList, int listID){

    cardList->head = NULL;
    cardList->tail = NULL;
    cardList->size = 0;
    cardList->listID = listID;
}

void gameInitialize () {
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

    if (!(fromColumn->tail == NULL)){
        fromColumn->tail->hidden = 0;
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

Card* searchCard(int numRank, int numSuit){
    
    if (foundations[numSuit].size > numRank) {
        return NULL; // WISH: Handle moves from foundations.
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
