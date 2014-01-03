#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

uint64_t get_local_time()
{
    char buffer[32];
    time_t timep;
    struct tm *p;
    time(&timep);
    p = localtime(&timep);
    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "%.4d%.2d%.2d%.2d%.2d%.2d",
             1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday,
             p->tm_hour, p->tm_min, p->tm_sec);
    return strtoul(buffer, NULL, 10);
}

int main()
{
    printf("time = [%llu]\n", get_local_time());
    return 0;
}
