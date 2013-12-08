#include <stdio.h>

int main(int argc, char *argv[])
{
    long i = 2;
    long k = 2;

    for (i = 1; i <= 32; i++) {
        k = k * 2;
        fprintf(stdout, "%ld\n", k - 1);
    }
    return 0;
}
