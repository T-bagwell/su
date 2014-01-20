#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

void do_video_seek()
{

}
int main(int argc, char *argv[])
{
    int i = 0;
    int len = 0;
    unsigned long start_offset = 0;
    char *request_method = NULL;
    char *data = NULL;
    char *p = NULL;
    char execname[512];
    char tmp_execname[512];
    char number[16];
    char ascii_code[3];
    char ascii_char = 0;


    printf ("Content-type: text/html\r\n");
    printf ("\r\n\n");

    memset(tmp_execname, 0, sizeof(tmp_execname));
    memset(execname, 0, sizeof(execname));
    memset(ascii_code, 0, sizeof(ascii_code));
    memset(number, 0, sizeof(number));

    request_method = getenv ("REQUEST_METHOD");
    if (!strncasecmp (request_method, "POST", 4)) {
        data = getenv ("CONTENT_LENGTH");
        len = atoi (data);
        fgets (execname, len + 1, stdin);
    } else {
        data = getenv ("QUERY_STRING");
        strncpy (execname, data, strlen (data));
    }

    data = execname;

    while (*data) {
        if (*data == '%') {
            ascii_code[0] = *(data + 1);
            ascii_code[1] = *(data + 2);
            p = ascii_code;
            ascii_char = strtol (p, '\0', 16);
            tmp_execname[i] = ascii_char;
            i++;
            data += 3;
        }
        tmp_execname[i] = *data;
        data++;
        i++;
    }

    p = data;
    while (*p != '\0') {
        if (!strncasecmp(data, "start=", strlen("start="))) {
            p += strlen("start=");
            while (*p != '&' && *p != '\0') {
                number[i] = *p;
                p++;
                i++;
            }
        }
        p++;
        data++;
    }
     p = number;
    start_offset = strtoul(p, 0, 10);

    start_offset *= 1000;
    start_offset &= 0xFFFFFF;


    do_video_seek();

    return 0;
}
