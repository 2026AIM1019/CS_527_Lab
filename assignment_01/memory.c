#include<stdio.h>
#include "compiler.h"

char Instruction[256];
char Data[256];

void initialize(char program_file[], char data_file[]){

    // Load Instruction Memory from program.byte
    FILE *file = fopen(program_file, "r");

    if (file == NULL){
        printf("Error : Not able to open program.byte.\n");
        return;
    }

    int value, index = 0;

    while (fscanf(file, "%d", &value) == 1 && index < 256){
        Instruction[index++] = (char)value;
    }
    fclose(file);


    // Load Data Memory from data.byte
    file = fopen(data_file, "r");

    if (file == NULL){
        printf("Error : Not able to open data.byte.\n");
        return;
    }
    
    index = 0;

    while (fscanf(file, "%d", &value) == 1 && index < 256){
        Data[index++] = (char)value;
    }
    fclose(file);

    printf("Memory initialized Sucessfully.\n\n");
}


void finalize(char data_file[]){
    FILE *file = fopen(data_file, "w");

    if (file == NULL){
        printf("Error : Not able to write data.byte. \n");
        return;
    }

    for (int i = 0; i < 256; i++){
        fprintf(file, "%d\n", (unsigned char)Data[i]);
    }
    fclose(file);

    printf("Data Memory Save in data.byte Sucessfully. \n\n");
}
