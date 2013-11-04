#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    unsigned long start_number = 0;
    unsigned long end_number = 0;
    unsigned char end_string[] = "20131026161000";
    unsigned char start_string[] = "20131026151900";

    end_number = strtoul((char *)end_string, NULL, 10);
    start_number = strtoul((char *)start_string, NULL, 10);
    fprintf(stdout, "end_string = [%s], end_number = [%lu]\n"
            "start_string = [%s], start_number = [%lu]\n"
            "dec = [%lu]\n",
            end_string, end_number,start_string, start_number,
            end_number - start_number);

    return 0;
}
