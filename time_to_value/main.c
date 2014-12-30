#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "time_to_val.h"

unsigned char *time_string = "1419932834";

int main(int argc, char *argv[])
{
    int local_time = 0;
    int remote_time = 0;
    int ret = 0;

    local_time = get_local_time();
    remote_time = time_str2int(time_string);
    
    ret = check_time_val(local_time, remote_time);
    fprintf(stderr, "localtime = [%d], remote_time = [%d], the ret = [%d]\n", 
            local_time, remote_time, ret);
    return 0;
}
