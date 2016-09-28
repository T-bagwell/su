#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "flvparser.h"

static unsigned int g_timestamp_int = 0;

static int script_type_parse(unsigned char *data);

static double int2double(uint64_t i)
{
    union av_intfloat64 v;
    v.i = i;
    return v.f;
}
/*
 * get_header_audio check if the flv have audio stream
 * info, a byte with video and audio bit flag
 *
 * return 0 or 1
*/
static int get_header_audio(char info)
{
    return (info & 0x4) >> 2;
}

/*
 * get_header_video check if the flv have video stream
 * info, a byte with video and audio bit flag
 *
 * return 0 or 1
*/
static int get_header_video(char info)
{
    return info & 0x1;
}



/*
 * get_string_len get the key-value value string length
 * data, the data will be parsed
 *
 * return the String length by int
*/
int get_string_len(unsigned char *data)
{

    unsigned char *p = data;
    unsigned char len_char[8];
    int len_int = 0;

    memset(len_char, 0, sizeof(len_char));
    snprintf((char *)len_char, sizeof(len_char), "0x%x%x", data[0], data[1]);
    p = len_char;

    len_int = strtoul((char *)p, NULL, 16);

    return len_int;
}

/*
 * get_bool_value get bool type value (key-value value)
 * data, the data will be parsed
 *
 * return bool value 0 or 1
*/
int get_bool_value(unsigned char *data)
{
    return *data;
}

/*
 * get_key_len the key-value key string length unit: four bytes
 * data, the data will be parsed
 *
 * return the key length by int
*/
int get_key_len(unsigned char *data)
{
    char len_char[16];
    char *p = len_char;

    memset(len_char, 0, sizeof(len_char));
    snprintf(len_char, sizeof(len_char), "0x%x%x%x%x", data[0], data[1], data[2], data[3]);

    return strtoul(p, NULL, 16);
}

int process_ecma_array_end(unsigned char *data)
{
    return 0;
}

/*
 * process_ecma_array The onMetaData sub type named ECMA Array
 * data, the data will be parsed
 *
 * return the offset of the data
*/
int process_ecma_array(unsigned char *data)
{
    int ecma_array_len = 0;
    int keyname_len = 0;
    unsigned char keyname[32];
    unsigned char *p = data;
    int i = 0;

    ecma_array_len = get_key_len(p);
    p += 4;

    for (i = 0; i < ecma_array_len; i++) {
        keyname_len = get_string_len(p);
        p += 2;
        memset(keyname, 0, sizeof(keyname));
        strncpy((char *)keyname, (const char *)p, keyname_len);
        fprintf(stderr, "keyname = [%s]\n", keyname);
        p += keyname_len;
        p += script_type_parse(p);
    }

    if (*p == 0 && *(p + 1) == 0 && *(p + 2) == 9) {
        p += 3;
    }
    return p - data;
}

/*
 * get_double get the double number by long long type
 * data, the data will be parsed
 *
 * return the parse out number of long long
*/
unsigned long long get_double(unsigned char *data)
{
    unsigned char double_number[64];
    unsigned char *p = double_number;

    memset(double_number, 0, sizeof(double_number));
    snprintf((char *)double_number, sizeof(double_number), "0x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x",
             *data, *(data + 1), *(data + 2), *(data + 3), *(data + 4), *(data + 5), *(data + 6), *(data + 7));

    return strtoull((char *)p, NULL, 16);

}

/*
 * script_type_parse parse the script object type and parse the data
 * data, the data will be parsed
 *
 * return the data offset
*/
static int script_type_parse(unsigned char *data)
{
    int ret = 0;
    unsigned char *p = data;
    unsigned char string_output[512];
    unsigned long long number = 0;
    double double_number = 0.0;

    switch (*p) {
    case Number:
        p++;
        number = get_double(p);
        double_number = int2double(number);
        fprintf(stderr, "number = [%.2f]\n", double_number);
        p += 8;
        break;

    case Boolean:
        p++;
        ret = get_bool_value(p);
        p++;
        break;

    case String:
        p++;
        memset(string_output, 0, sizeof(string_output));
        ret = get_string_len(p);
        p += 2;
        strncpy((char *)string_output, (const char *)p, ret);
        fprintf(stderr, "String = [%s]\n", string_output);
        p += ret;
        break;

    case Object:
        p++;
        break;

    case MovieClip:
        p++;
        break;

    case Null:
        p++;
        break;

    case Undefined:
        p++;
        break;

    case Reference:
        p++;
        break;

    case EcmaArray:
        p++;
        ret = process_ecma_array(p);
        p += ret;
        break;

    case ObjectEndMarker:
        p++;
        break;

    case StringArray:
        p++;
        break;

    case Date:
        p++;
        break;

    case LongString:
        p++;
        break;

    default:

        break;
    }
    return p - data;
}

/*
 * do_tag_onMetaData parse the onMetaData tags
 * fd, file describe handle
 * size, will read size of the data, unit: byte
 *
 * return offset of the data
*/
int do_tag_onMetaData(int fd, int size, unsigned char *onMetaData)
{
    int ret = 0;
    unsigned char *p = NULL;

    p = onMetaData;
    while (p) {
        if (p - onMetaData >= size) {
            break;
        }
        ret = script_type_parse(p);
        fprintf(stderr, "in while 1 offset = [%ld]\n", p - onMetaData);
        p += ret;
    }
    return p - onMetaData;
}


int flv_header_parse(int fd, header_t *header)
{
    int ret = 0;

    memset(header, 0, sizeof(header_t));
    ret = read(fd, header, sizeof(header_t));

    if (ret <= 0) {
        fprintf(stderr, "read header error, %d\n", -EINVAL);
        return -EINVAL;
    }

    fprintf(stderr, "flv = [%c %c %c] version = [%x]\n"
            "TypeFlagsAudio = [%x], TypeFlagsVideo = [%x], DataOffset = [%x %x %x %x]\n",
            header->tag_f, header->tag_l, header->tag_v, header->version,
            get_header_audio(header->audio_and_video), get_header_video(header->audio_and_video),
            header->DataOffset[0], header->DataOffset[1], header->DataOffset[2], header->DataOffset[3]);

    return ret;
}


int flv_do_tag(int fd, unsigned long *size, tag_t *tag)
{
    int ret = 0;
    char *p = NULL;
    char data_size[11] = {0};

    ret = read(fd, tag, sizeof(tag_t));
    fprintf(stderr, "%s %d ret = [%d]\n", __func__, __LINE__, ret);
    if (ret <= 0) {
        fprintf(stderr, "Can not ret the fd errno [%d]\n", -EINVAL);
        return -EINVAL;
    }

    p = (char *)tag;

    g_timestamp_int += 15;
//    tag->timestamp[0] = (g_timestamp_int & 0x00FF0000) >> 16;
//    tag->timestamp[1] = (g_timestamp_int & 0x0000FF00) >> 8;
//    tag->timestamp[2] = (g_timestamp_int & 0x000000FF);

    fprintf(stderr, "body . type = [%x]\n", *p);
    fprintf(stderr, "size = %lu tag->type = [%x]"
            "tag->size = [%02x %02x %02x], tag->timestamp = [%02x %02x %02x], "
            "tag->extent_timestamp = [%02x], tag->streamid = [%02x %02x %02x]\n",
            sizeof(tag_t), tag->tagtype,
            tag->size[0], tag->size[1],tag->size[2], tag->timestamp[0], tag->timestamp[1],
            tag->timestamp[2], tag->extent_timestamp, tag->stream_id[0],
            tag->stream_id[1], tag->stream_id[2]);
    memcpy(data_size, tag->size, 3);
    p = data_size;
    snprintf(data_size, sizeof(data_size), "0x%02x%02x%02x", tag->size[0], tag->size[1], tag->size[2]);
    *size = strtoul(data_size, NULL, 16);
    fprintf(stderr, "size = [%lu] %s\n", *size, data_size);

    return tag->tagtype;
}
