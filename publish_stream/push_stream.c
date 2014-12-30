#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define LIST_FILE       "./list.txt"


void usage()
{
    fprintf(stderr, "please input the -f list_file -s url\n"
            "for example:"
            "\n\t ./push_stream -f ./list.txt -s rtmp://192.168.8.234/live?vhost=dev/lq\n");
}

int main(int argc, char *argv[])
{
    int len = 0;
    FILE *fp = NULL;
    char buffer[512];
    char command[1024];
    char *filename = LIST_FILE;
    char *server_name = NULL;
    char ch = 0;


    memset(buffer, 0, sizeof(buffer));

    if (argc < 3) {
        usage();
        return 0;
    }

    while ((ch = getopt(argc, argv, "f:s:")) != -1) {
        switch (ch) {
            case 'f':
                filename = optarg;
                break;
            case 's':
                server_name = optarg;
            case '?':
            default:
                usage();
        }
    }

    while (1) {
        fp = fopen(filename, "r");
        while (fgets(buffer, sizeof(buffer), fp)) {
            len = strlen(buffer);
            buffer[len - 1] = '\0';
            memset(command, 0, sizeof(command));
            snprintf(command, sizeof(command),
                     "ffmpeg -re -i %s -strict -2 -vcodec libx264 -b:v 800k"
                     " -acodec aac -s 768x326 -f flv"
                     " -mpv_flags strict_gop -rtmp_live any %s", buffer, server_name);
            fprintf(stdout, "command = [%s]\n", command);
            system(command);
            memset(buffer, 0, sizeof(buffer));
        }
        fclose(fp);
    }
    return 0;
}
