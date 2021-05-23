#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef struct line {
    char *string;
} line;

int main() {
    line l;
    l.string = malloc(5);
    char ch = '\0';
    printf("%s", ch == '\0' ? "true" : "false");
}