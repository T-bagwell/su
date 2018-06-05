#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffer_print.h"

static int toupper(int c)
{
    if (c >= 'a' && c <= 'z') {
        c ^= 0x20;
    }

    return c;
}

int stristart(const char *str, const char *pfx, const char **ptr)
{
    while (*pfx && toupper((unsigned)*pfx) == toupper((unsigned)*str)) {
        pfx++;
        str++;
    }
    if (!*pfx && ptr) {
        *ptr = str;
    }

    return !*pfx;
}

char *stristr(const char *s1, const char *s2)
{
    if (!*s2) {
        return (char*)(intptr_t)s1;
    }
    do {
        if (stristart(s1, s2, NULL)) {
            return (char*)(intptr_t)s1;
        }
    } while (*s1++);

    return NULL;
}
char *string_replace(const char *str, const char *from, const char *to)
{
    char *ret = NULL;
    const char *pstr2, *pstr = str;
    size_t tolen = strlen(to), fromlen = strlen(from);
    BPrint pbuf;

    bprint_init(&pbuf, 1, BPRINT_SIZE_UNLIMITED);
    while ((pstr2 = stristr(pstr, from))) {
        bprint_append_data(&pbuf, pstr, pstr2 - pstr);
        pstr = pstr2 + fromlen;
        bprint_append_data(&pbuf, to, tolen);
    }
    bprint_append_data(&pbuf, pstr, strlen(pstr));
    if (!bprint_is_complete(&pbuf)) {
        bprint_finalize(&pbuf, NULL);
    } else {
        bprint_finalize(&pbuf, &ret);
    }

    return ret;
}

int main(int argc, char *argv[])
{
    const char *haystack = "Education consists mainly in what we have unlearned.";
    const char * const needle[] = {"learned.", "unlearned.", "Unlearned"};
    char *ptr;

#define TEST_STRING_REPLACE(haystack, needle, expected) \
    ptr = string_replace(haystack, needle, "instead"); \
    if (ptr == NULL) { \
        printf("error, received null pointer!\n"); \
    } else { \
        if (strcmp(ptr, expected) != 0) \
            printf( "expected: %s, received: %s\n", expected, ptr); \
        free(ptr); \
    }

    printf("Test start\n");
    TEST_STRING_REPLACE(haystack, needle [0], "Education consists mainly in what we have uninstead");
    TEST_STRING_REPLACE(haystack, needle [1], "Education consists mainly in what we have instead");
    TEST_STRING_REPLACE(haystack, needle [2], "Education consists mainly in what we have instead.");
    TEST_STRING_REPLACE(haystack, needle [1], "Education consists mainly in what we have instead");
    printf("Test end\n");
    return 0;
}
