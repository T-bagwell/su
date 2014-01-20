#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "flvparser.h"

static FILE *g_output_file = NULL;
static int g_have_metadata = FALSE;


static int init_output_file()
{
    g_output_file = fopen("./xxx.flv", "wb");

    return 0;
}

static void fini_output_file()
{
    fclose(g_output_file);
}

static int get_video_frame_type(unsigned char type)
{
    return (type & 0xF0) >> 4;
}



int do_video_seek(int fd)
{
    int tag_type = -1;
    int ret = 0;
    int write_first_tag = FALSE;
    unsigned long size = 0;
    unsigned char PreviousTagSize[4];
    unsigned char buffer[409600];
    header_t header;
    tag_t tag;


    memset(&header, 0, sizeof(header));
    ret = flv_header_parse(fd, &header);
    if (ret < 0)
        return -EINVAL;
    ret = read(fd, PreviousTagSize, sizeof(PreviousTagSize));
    if (g_have_metadata == FALSE) {
        fwrite(&header, sizeof(header), 1, g_output_file);
        fwrite(PreviousTagSize, sizeof(PreviousTagSize), 1, g_output_file);
    }

    while (1) {
        tag_type = flv_do_tag(fd, &size, &tag);
        if (size == 5) {
            return 0;
        }
        switch(tag_type) {
        case TAG_TYPE_SCRIPTDATA:
            fprintf(stderr, "Type = [MetaData]\n");
            ret = read(fd, buffer, size);
            do_tag_onMetaData(fd, size, buffer);
            if (g_have_metadata == FALSE) {
                fwrite(&tag, sizeof(tag), 1, g_output_file);
                fwrite(buffer, size, 1, g_output_file);
            } else {
                ret = read(fd, PreviousTagSize, sizeof(PreviousTagSize));
                goto skip_write_previous_tag_size;
            }
            fprintf(stderr, "data_size = [%lu]\n", size);
            break;

        case TAG_TYPE_VIDEO:
            ret = read(fd, buffer, size);
            if (ret <= 0) {
                break;
            }

            if (get_video_frame_type(buffer[0]) == FRAME_TYPE_KEYFRAME) {
                write_first_tag = TRUE;
            }

            if (g_have_metadata == TRUE) {
                if (buffer[1] == VIDEO_SEQENCE_START) {
                    write_first_tag = FALSE;
                    fprintf(stderr, "in seq end\n");
                    read(fd, PreviousTagSize, sizeof(PreviousTagSize));
                    goto skip_write_previous_tag_size;
                }
            }

            if (write_first_tag == FALSE) {
                read(fd, PreviousTagSize, sizeof(PreviousTagSize));
                goto skip_write_previous_tag_size;
            } else {
                fwrite(&tag, sizeof(tag), 1, g_output_file);
                fwrite(buffer, ret, 1, g_output_file);
                fprintf(stderr, "Type = [Video]\n");
                g_have_metadata = TRUE;
            }
            break;

        case TAG_TYPE_AUDIO:
            if (write_first_tag == FALSE) {
                ret = read(fd, buffer, size);
                read(fd, PreviousTagSize, sizeof(PreviousTagSize));
                goto skip_write_previous_tag_size;
            }
            ret = read(fd, buffer, size);
            fwrite(&tag, sizeof(tag), 1, g_output_file);
            fwrite(buffer, ret, 1, g_output_file);
            fprintf(stderr, "Type = [Audio]\n");
            break;

        default:
            fprintf(stderr, "Cannot read right type of flv\n");
            return -EINVAL;
        }
        memset(PreviousTagSize, 0, sizeof(PreviousTagSize));
        ret = read(fd, PreviousTagSize, sizeof(PreviousTagSize));
        fwrite(PreviousTagSize, sizeof(PreviousTagSize), 1, g_output_file);
        fprintf(stderr, "=================================\n");

skip_write_previous_tag_size:
        ;
    }
    return 0;
}
int main(int argc, char *argv[])
{
    int i = 0;
    int fd = 0;
    char filename[1024];

    i = 0;

    init_output_file();
    while (1) {
        memset(filename, 0, sizeof(filename));
        snprintf(filename, sizeof(filename), "./%d.flv", i);
        fprintf(stderr, "file = [%s]\n", filename);
        fd = open(filename, O_RDONLY);
        if (fd < 0) {
            fprintf(stderr, "Cannot open the file %s\n", filename);
            break;
        }
        do_video_seek(fd);
        close(fd);
        i++;
    }
    fini_output_file();

    return 0;
}
