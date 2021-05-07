#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*
 * Split a string into smaller strings using a delimiter.
 * E.g. str_split("A,b,QQ",',') returns ("A","b","QQ").
 */


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
        assert(ident<count);
        *(result+ident++) = strdup(token);

        token = strtok(0,delim);
    }
    assert(ident == count - 1);
    *(result + ident) = 0;

    return result;
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

/*
 * Convert a string to an identical string, except lower case characters
 * are now upper case.
 */

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

/*
 * Determine whether two chars represent a card if used with assertType == 'c'.
 * Likewise if two chars represent a column using assertType == 'g'.
 * Returns a value representing the card or column if success, else -1.
 */

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
        printf("You f'd up somewhere... assertParameter() called with '%c'", assertType);
        break;
    }
    return value;

}

/*
 * HELP MENU.
 */

void printHelp() {
    printf("WELCOME TO YUKON HELP DESK!\n\n");
    printf("Command name\t| Syntax\t\t\t| Description\n");
    printf("-----------------------------------------------------------------------------------------------------\n");
    printf("FD\t\t| FD [column name]\t\t| Moves the last card in the column to a foundation.\n");
    printf("HP\t\t| HP\t\t\t\t| Displays this text.\n");
    printf("LD\t\t| LD [2 digit number]\t\t| Loads a card deck file to play with.\n");
    printf("MV\t\t| MV [Card name] [Column name]\t| Move the card to the named column.\n");
    printf("RD\t\t| RD\t\t\t\t| Redraw the game to \"just\" escape from this message.\n");
    printf("SW\t\t| SW\t\t\t\t| Deal an ordered deck without hiding cards.\n");
    // printf("TT\t\t| TT\t\t\t\t| Turn off console clears (for testing purposes).\n");
    // printf("QT\t\t| QT\t\t\t\t| Turn on console clears (default setting).\n");
    printf("QQ\t\t| QQ\t\t\t\t| Quit the game.\n");
    printf("\n");
    printf("INPUT > ");
}