#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

int parser_ipaddr_to_int(unsigned char *ipaddr, unsigned char *val1, unsigned char *val2, unsigned char *val3, unsigned char *val4)
{
    unsigned char *p = ipaddr;
    unsigned char tmp_ipaddr[4];
    int i = 0;
    int j = 0;
    
    while (p) {
        memset(tmp_ipaddr, 0, sizeof(tmp_ipaddr));
        while (*p != '.' && *p != '\0') {
            tmp_ipaddr[i] = *p;
            i++;
            p++;
        }
        if (j == 0) {
            *val1 = atoi(tmp_ipaddr);
        } else if (j == 1) {
            *val2 = atoi(tmp_ipaddr);
        } else if (j == 2) {
            *val3 = atoi(tmp_ipaddr);
        } else if (j == 3) {
            *val4 = atoi(tmp_ipaddr);
            break;
        }
        fprintf(stdout, "yyy\n");
        i = 0;
        j++;
        p++;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int input_fd = 0;
    int output_fd = 0;
    int ret = 0;
    int read_size = 0;
    unsigned char buffer[40960];
    unsigned char *p = NULL;
    unsigned char ipaddr[32];
    unsigned int ipaddr1 = 0;
    unsigned int ipaddr2 = 0;
    unsigned int ipaddr3 = 0;
    unsigned int ipaddr4 = 0;
    int port_in = 0;
    int port_out = 0;
    int modify_flag = 0;

    memset(buffer, 0, sizeof(buffer));

    memset(ipaddr, 0, sizeof(ipaddr));
    strncpy(ipaddr, argv[3], strlen(argv[3]));

    parser_ipaddr_to_int(ipaddr, &ipaddr1, &ipaddr2, &ipaddr3, &ipaddr4);

    fprintf(stdout, "%x, %x, %x, %x\n", ipaddr1, ipaddr2, ipaddr3, ipaddr4);

    port_in = atoi(argv[4]);
    port_out = atoi(argv[5]);

    input_fd = open(argv[1], O_RDONLY);
    output_fd = open(argv[2], O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    port_in = atoi(argv[4]);
    port_out = atoi(argv[5]);

    while ((ret = read(input_fd, buffer, sizeof(buffer)))) {
        p = buffer;
        read_size = ret;
        while (read_size != 0) {
            if (p[0] == ipaddr1 && p[1] == ipaddr2 && p[2] == ipaddr3 && p[3] == ipaddr4) {
                fprintf(stdout, "xxxxxxxx\n");
                modify_flag = 1;
            }


            if (((p[0] == ((port_in & 0xFF00) >> 8)) && (p[1] == (port_in & 0xFF))) && modify_flag == 1) {
                p[0] = (port_out & 0xFF00) >> 8;
                p[1] = port_out & 0xFF;
                modify_flag = 0;
                fprintf(stdout, "000\n");
            }
            p++;
            read_size--;
        }

        fprintf(stdout, "xxx\n");
        ret = write(output_fd, buffer, ret);
        memset(buffer, 0, sizeof(buffer));
    }

    close(output_fd);
    close(input_fd);

    return 0;
}
