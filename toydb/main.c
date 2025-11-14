// THIS IS DUMMY MAIN.C, actually currently just tests are working but we have to write this main.c to create database which will work like real database (like taking query, processing that and all...)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "pflayer/include/pf.h"
#include "amlayer/include/am.h"
#include "splayer/include/sp.h"

int main(int argc, char *argv[]) {
    
    printf("Starting ToyDB...\n");

    // PF_Init(max_num_page_for_buffer, replacement_strat);
    
    printf("ToyDB is ready.\n");
    
    printf("query loop....\n");

    // query processor
    
    printf("ToyDB shutting down.\n");
    
    return 0;
}