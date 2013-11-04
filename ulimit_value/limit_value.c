#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int main(int argc, char *argv[])
{
    fprintf(stdout, "the int limit = [%d]\n", INT_MAX);
    fprintf(stdout, "the unsigned int limit = [%u]\n", UINT_MAX);
    fprintf(stdout, "the long limit = [%ld]\n", LONG_MAX);
    fprintf(stdout, "the unsigned long limit = [%lu]\n", ULONG_MAX);
    fprintf(stdout, "the long long limit = [%lld]\n", LLONG_MAX);
    fprintf(stdout, "the unsigned long long limit = [%llu]\n", ULLONG_MAX);

    return 0;
}
