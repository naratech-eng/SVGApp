#define _POSIX_C_SOURCE = 200809L
#define _GNU_SOURCE

#include <SVGParser.h>
#include <string.h>
#include <ctype.h>

int returnInt() {
    return 5;
}

char *returnString() {
    char *test = NULL;

    test = strdup("hello world");
    return test;
}

char *modifyString(const char *input) {
    char *test = NULL;

    printf("input string: %s\n", input);

    test = strdup(input);

    for (int i = 0; i < strlen(test); i++) {
        test[i] = toupper(test[i]);
    }

    return test;
}

