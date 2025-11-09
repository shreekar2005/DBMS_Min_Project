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

    execlp("make", "make", "sp_testsp", NULL);
    
    printf("query loop....\n");

    // query processor
    
    printf("ToyDB shutting down.\n");
    
    return 0;
}