#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void strPushBack(char** str, size_t* size, size_t* capacity, const size_t capacityAdditional, const char sybmol);
void toLowerCase(char* str);

int main(int argc, char** argv) {
    /* Set grep options */
    int opt;
    char flag_s = 0;
    while ((opt = getopt(argc, argv, ":s")) != -1) {
        switch (opt) {
            case 's':
                flag_s = 1;
                break;
            case '?':
                break;
            case ':':
                break;
            default:
                break;
        }
    }
    optind = 1;
    char flag_i = 0;
    char flag_h = 0;
    char flag_n = 0;
    char flag_v = 0;
    size_t flag_A_value = 0;
    size_t flag_B_value = 0;
    size_t flag_C_value = 0;
    while ((opt = getopt(argc, argv, ":ihnvA:B:C:")) != -1) {
        switch (opt) {
            case 'i':
                flag_i = 1;
                break;    
            case 'h':
                flag_h = 1;
                break;
            case 'n':
                flag_n = 1;
                break;
            case 'v':
                flag_v = 1;
                break;
            case 'A':
                flag_A_value = atoi(optarg);
                break;
            case 'B':
                flag_B_value = atoi(optarg);
                break;
            case 'C':
                flag_C_value = atoi(optarg);
                break;
            case '?':
                if (!flag_s) { printf("%s: unknown option %c\n", argv[0], optopt); }
                break;
            case ':': {
                if (!flag_s) { printf("%s: option %c needs argument\n", argv[0], optopt); }
            }
        }
    }
    if (flag_C_value) {
        flag_A_value = 0;
        flag_B_value = 0;
    }

    if (argc - optind < 2) {
        printf("Usage: %s [OPTION]... PATTERNS [FILE]...\n", argv[0]);
        return 0;
    }

    /* Template to compare with */
    char* template = argv[optind];
    size_t templateSize = strlen(template);
    ++optind;

    /* Parse each file into strs */
    char* str;
    size_t strSize, strCapacity, strCapacityAdditional = 32;
    int line;
    char lineStr[128], fileStr[256];
    char** strs = NULL;
    size_t strsSize = 0;
    char* tempArr = NULL;
    size_t maxStrSize = 0; // variable for lowerStr array

    for (; optind < argc; ++optind) {
        FILE* file = fopen(argv[optind], "r");
        if (!file) {
            if (!flag_s) { printf("%s: %s: no such file or directory\n", argv[0], argv[optind]); }
            continue;
        }
        sprintf(fileStr, "%s: ", argv[optind]);
        str = NULL;
        strSize = 0;
        strCapacity = 64;
        line = 1;
        char c;
        while ((c = fgetc(file)) != EOF) {
            if (c == '\n') {
                str = realloc(str, (strSize + 1) * sizeof(char));
                str[strSize] = '\0';
                if (flag_n) {
                    sprintf(lineStr, "%d: ", line);
                    tempArr = (char*) malloc((strlen(lineStr) + strSize + 1) * sizeof(char));
                    strcpy(tempArr, lineStr);
                    strcat(tempArr, str);
                    free(str);
                    str = tempArr;
                    tempArr = NULL;
                    strSize += strlen(lineStr);
                }
                if (!flag_h) {
                    tempArr = (char*) malloc((strlen(fileStr) + strSize + 1) * sizeof(char));
                    strcpy(tempArr, fileStr);
                    strcat(tempArr, str);
                    free(str);
                    str = tempArr;
                    tempArr = NULL;
                    strSize += strlen(fileStr);
                }
                maxStrSize = (strSize > maxStrSize) ? strSize : maxStrSize;
                ++strsSize;
                strs = realloc(strs, strsSize * sizeof(char*));
                strs[strsSize - 1] = str;
                str = NULL;
                strSize = 0;
                strCapacity = 10;
                ++line;
                continue;
            }
            strPushBack(&str, &strSize, &strCapacity, strCapacityAdditional, c);
        }
        if (str != NULL) {
            str = realloc(str, (strSize + 1) * sizeof(char));
            str[strSize] = '\0';
            ++strsSize;
            strs = realloc(strs, strsSize * sizeof(char*));
            strs[strsSize - 1] = str;
            str = NULL;
        }
        fclose(file);
    }

    /* Add fitting strings to res */
    char** res = NULL;
    size_t resSize = 0;
    char* lowerStr = (char*) malloc((maxStrSize + 1) * sizeof(char));
    char* lowerTemplate = (char*) malloc((templateSize + 1) * sizeof(char));
    char* resIndx = (char*) malloc(strsSize * sizeof(char)); // resIndx[i] = 0 if strs[i] was added to res and resIndx[i] = 1 if was not
    for (size_t i = 0; i < strsSize; ++i) {
        resIndx[i] = 0;
    }

    for (size_t i = 0; i < strsSize; ++i) {
        char strFits = 0;
        if (flag_i) {
            strcpy(lowerStr, strs[i]);
            strcpy(lowerTemplate, template);
            toLowerCase(lowerStr);
            toLowerCase(lowerTemplate);
            if (strstr(lowerStr, lowerTemplate) != NULL) {
                strFits = 1;
            }
        }
        else {
            if (strstr(strs[i], template) != NULL) {
                strFits = 1;
            }
        }
        
        if ((strFits && !flag_v) || (!strFits && flag_v)) {
            if (flag_A_value || flag_C_value) {
                size_t j = flag_A_value != 0 ? flag_A_value : flag_C_value;
                for (; j > 0; --j) {
                    if (j <= i && resIndx[i - j] == 0) {
                        ++resSize;
                        res = realloc(res, resSize * sizeof(char*));
                        res[resSize - 1] = strs[i - j];
                        resIndx[i - j] = 1;
                    }
                }
            }
            if (resIndx[i] == 0) {
                ++resSize;
                res = realloc(res, resSize * sizeof(char*));
                res[resSize - 1] = strs[i];
                resIndx[i] = 1;
            }
            if (flag_B_value || flag_C_value) {
                size_t max = flag_B_value != 0 ? flag_B_value : flag_C_value;
                for (size_t j = 1; j <= max; ++j) {
                    if (i + j < strsSize && resIndx[i + j] == 0) {
                        ++resSize;
                        res = realloc(res, resSize * sizeof(char*));
                        res[resSize - 1] = strs[i + j];
                        resIndx[i + j] = 1;
                    }
                }
            }
        }
    }

    free(resIndx);
    resIndx = NULL;
    free(lowerStr);
    free(lowerTemplate);
    lowerStr = NULL;
    lowerTemplate = NULL;

    for (size_t i = 0; i < resSize; ++i) {
        printf("%s\n", res[i]);
    }

    /* Release memory */
    for (size_t i = 0; i < strsSize; ++i) {
        free(strs[i]);
    }
    free(strs);
    free(res);
    return 0;
}

// Add one element to char* 
void strPushBack(char** str, size_t* size, size_t* capacity, const size_t capacityAdditional, const char symbol) {
    if (*size == 0) {
        *str = (char*) malloc(*capacity * sizeof(char));
    }
    ++*size;
    if (*size > *capacity) {
        *capacity += capacityAdditional;
        *str = realloc(*str, *capacity * sizeof(char));
    }
    (*str)[*size - 1] = symbol;
}

void toLowerCase(char* str) {
    size_t i = 0;
    while (str[i]) {
        if (str[i] >= 'A' && str[i] <= 'Z') {
            str[i] = str[i] - 'A' + 'a';
        }
        ++i;
    }
}
