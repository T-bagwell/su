#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>

#define FILENAME_MAX_LEN 1024

struct fragments {
    float duration;
    char description[FILENAME_MAX_LEN];
    char fragment_name[FILENAME_MAX_LEN];
};

struct fragments frag[10240];

static int parse_playlist(char *playlist, char *url, int *count)
{
    FILE *fp = fopen(playlist, "r");
    char buf[2048];
    int linesize = 0;
    char *p = NULL;
    char *end = NULL;
    char key[FILENAME_MAX_LEN];
    int fragment_counter = 0;
    int in_fragment = 0;

    if (fp == NULL) {
        fprintf(stderr, "Cannot open the file %s\n", playlist);
        return -ENOENT;
    }

    memset(buf, 0, sizeof(buf));
    while (fgets(buf, sizeof(buf), fp)) {
        linesize = strlen(buf);
        buf[linesize - 1] = 0;
        p = buf;
        if (buf[0] == '#') {
            p++;
            if (!strncasecmp(p, "EXT-X-KEY:", strlen("EXT-X-KEY:"))) {
                p = strstr(p, "URI=");
                if (p) {
                    p += strlen("URI=");
                    if (*p == '"') {
                        p++;
                        end = strstr(p, "\"");
                        p[end - p] = 0;
                    }
                    while (*p == ' ' || *p == '\t') {
                        p++;
                    }
                    printf("key url = [%s]\n", p);
                    strncpy(frag[fragment_counter].fragment_name, p, strlen(p));
                    fragment_counter++;
                }
            } else if (!strncasecmp(p, "EXTINF:", strlen("EXTINF:"))) {
                in_fragment = 1;
                p += strlen("EXTINF:");
                end = strstr(p, ",");
                if (end) {
                    p[end - p] = 0;
                }
                while (*p == ' ' || *p == '\t') {
                    p++;
                }
                printf("durtion = [%s]\n", p);
                frag[fragment_counter].duration = strtold(p, NULL);
            }
        } else {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            strncpy(frag[fragment_counter].fragment_name, p, strlen(p));
            in_fragment = 0;
            fragment_counter++;
        }

        memset(buf, 0, sizeof(buf));
    }

    *count = fragment_counter;
    return 0;
}

static int download_fragment(char *url, char *prefix)
{
    char buf[FILENAME_MAX_LEN] = {0, };
    char *p = buf;
    strncpy(p, "wget ", strlen("wget "));
    strncat(p, prefix, strlen(prefix));
    strncat(p, url, strlen(url));


    FILE *pfp = popen(p, "r");
    printf("command = [%s]\n", p);
    if (pfp == NULL) {
        printf("exec command %s error\n", p);
        return -ENOENT;
    }

    while(fgets(buf, sizeof(buf), pfp)) {
        ;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int count = 0;
    char *url = argv[1];
    char *playlist = argv[2];
    int i = 0;

    parse_playlist(playlist, frag, &count);

    printf("count = %d\n", count);
    for (i = 0; i < count; i++) {
        printf("frag[%d].durtion = [%lf], frag[%d].name = [%s]\n", i, frag[i].duration, i, frag[i].fragment_name);
        download_fragment(frag[i].fragment_name, url);
    }
    return 0;
}

