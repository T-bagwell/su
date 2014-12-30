#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

typedef struct {
    uint32_t type;
    uint32_t short_size;
    uint64_t size;
    uint64_t start;
} mp4_atom_t;

#define FOURCC(a, b, c, d) ((uint32_t)(a)) + \
    ((uint32_t)(b) << 8) + \
    ((uint32_t)(c) << 16) + \
    ((uint32_t)(d) << 24)



int get_mp4_header(int fd)
{
    unsigned int ret = 0;
    unsigned char buffer[4];

    ret = read(fd, buffer, 4);
    if (ret != 4) {
        return 0;
    }

    return ret;
}

int get_mp4_type(char *dst, int fd)
{
    int ret = 0;
    char buffer[4];

    ret = read(fd, buffer, 4);
    if (ret != 4) {
        return 0;
    } else {
        strncpy(dst, buffer, 4);
    }


    return ret;
}

int get_mp4_min_version(int fd)
{
    int ret = 0;

    read(fd, &ret, 4);

    return ret;
}

int get_mp4_comp_brands_str(char *dst, int fd)
{
    int ret = 0;
    char buffer[8];

    ret = read(fd, buffer, 8);
    if (ret != 8) {
        return 0;
    } else {
        strncpy(dst, buffer, 8);
    }

    return ret;
}

int main(int argc, char *argv[])
{

    int fd = 0;
    int ret = 0;
    unsigned int box_type = 0;
    char media_type[8];
    char comp_brands_str[16];
    char *filename = NULL;

    filename = argv[1];

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Cannot open the file [%s]\n", filename);
        return -ENOENT;
    }

    get_mp4_header(fd);

    while (1) {
        ret = read(fd, &box_type, 4);
        switch (box_type) {
        case FOURCC('f', 't', 'y', 'p'):
            memset(media_type, 0, sizeof(media_type));
            get_mp4_type(media_type, fd);
            fprintf(stdout, "the type = [%s]\n", media_type);
            ret = get_mp4_min_version(fd);
            fprintf(stdout, "min_ver = %x\n", ret);

            memset(comp_brands_str, 0, sizeof(comp_brands_str));
            get_mp4_comp_brands_str(comp_brands_str, fd);
            fprintf(stdout, "comp_brands_str = [%s]\n", comp_brands_str);
            break;

        case FOURCC('m', 'o', 'o', 'v'):

        default:
            fprintf(stdout, "the type in default\n");
            return 0;
            break;
        }
    }

    return 0;
}
