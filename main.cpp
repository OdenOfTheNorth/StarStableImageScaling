#include "tgaHandler.h"
#include <cstdio>

int main(int argc, char **argv) {

    tgaHandler handler;
    if (argc!=4)
        printf("You gave %d Inputed it you sould give 4. Try again\n", argc);
    else{        
        handler.CreateAndCopy(argv[1], argv[2], argv[3]);
    }

    return 0;
}