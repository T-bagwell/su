#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

int get_local_time()
{
    time_t time_p;

    time(&time_p);

    fprintf(stderr, "time = [%d]\n", time_p);

    return (int) time_p;
}

int time_str2int(unsigned char *str)
{
    return strtod(str, NULL);
}

int check_time_val(int local_time, int remote_time)
{
    if (abs(local_time - remote_time) > 1000) {
        return -1;
    }
    return 1;
}
