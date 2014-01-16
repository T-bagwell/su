#include <stdio.h>

int main(int argc, char *argv[])
{
    char buffer[8] = {0};
    int time = 7598027;

    snprintf(buffer, sizeof(buffer), "%06x", time);

    fprintf(stdout, "string = [%s]\n", buffer);
    return 0;
}
