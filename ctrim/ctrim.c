#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *ctrim(char *str)
{

    char *p = str;

    while (p) {
        if (*p != ' ' && *p != '\t') {
            break;
        }
        p++;
    }

    return p;
}

int main(int argc, char *argv[])
{
    char *test_string1 = "   abcde";
    char *test_string2 = " fuck";

    fprintf(stdout, "test_string1 = [%s], test_string2 = [%s]\n", ctrim(test_string1), ctrim(test_string2));
    return 0;
}
