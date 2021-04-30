#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

char** fileReader(char* ID){
    char tempArray[2];
    
    char** cards = malloc(sizeof(char*) * 52);

    char str[13];

    strcpy(str,"decks/");
    strcat(str, ID);
    strcat(str,".txt");

    FILE *fp;
    fp = fopen (str, "r");
    int i = 0;
    if(fp==NULL){ exit(-1);}
    while(!feof(fp)){
        fgets(tempArray, sizeof(tempArray) + 3, fp);
        *(cards + i++) = strdup(tempArray);
    }

    fclose(fp);   
    
    return cards;
}