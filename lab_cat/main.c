#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void cat(FILE* stream, size_t* line , char flagb, char flagE, char flagn, char flags);

int main(int argc, char** argv) {
    char flagb = 0, flagE = 0, flagn = 0, flags = 0; 
    int opt;
    while ((opt = getopt(argc, argv, "bEns")) != -1) {
        switch (opt) {
            case 'b':
                flagb = 1;
                break;
            case 'E':
                flagE = 1;
                break;
            case 'n':
                flagn = 1;
                break;
            case 's':
                flags = 1;
                break;
        }
    }
    if (flagb) { flagn = 0; }

    size_t* line = (size_t*) malloc(sizeof(size_t)); // line's number for output
    *line = 1;
    // input from stdin
    if (optind == argc) {
        cat(stdin, line, flagb, flagE, flagn, flags);
    }
    // input from files
    for (; optind < argc; ++optind) {
        FILE* input = fopen(argv[optind], "r");
        if (!input) {
            printf("%s: %s: no such file\n", argv[0], argv[optind]);
            continue;
        }
        cat(input, line, flagb, flagE, flagn, flags);
        fclose(input);
    }
    free(line);
    return 0;
}

void cat(FILE* stream, size_t* line, char flagb, char flagE, char flagn, char flags) {
    char str[1024];
    size_t strLen, prevStrLen = 1;
    while (fgets(str, 1024, stream) != NULL) {
        strLen = strlen(str) - 1;
        str[strLen] = '\0'; // delete \n from str
        if (flags && strLen == 0 && prevStrLen == 0) {
            continue;
        }
        if (flagb && strLen != 0) {
            printf("%6lu ", *line);
            ++*line;
        }
        if (flagn) {
            printf("%6lu ", *line);
            ++*line;
        }
        printf("%s", str);
        if (flagE) {
            printf("$");
        }
        printf("\n");
        prevStrLen = strLen;
    }
}
