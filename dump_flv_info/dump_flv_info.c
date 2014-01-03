/*
 * Copyright (c) 2013 Steven Liu
 *
 *
 * flv_info_dump is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define TAG_FLV_F       'F'
#define TAG_FLV_L       'L'
#define TAG_FLV_V       'V'
#define TAG_TYPE_SCRIPTDATA     0x12
#define TAG_TYPE_VIDEO          0x09
#define TAG_TYPE_AUDIO          0x08

int script_type_parse(unsigned char *data);

union av_intfloat64 {
    uint64_t i;
    double   f;
};

enum script_data_type {
    Number  = 0,
    Boolean,
    String,
    Object,
    MovieClip,
    Null,
    Undefined,
    Reference,
    EcmaArray,
    ObjectEndMarker,
    StringArray,
    Date,
    LongString,
};
typedef enum script_data_type script_type_t;

struct script_string {
    char len[2];
};
typedef struct script_string String_t;

struct flv_header {
    unsigned char tag_f;
    unsigned char tag_l;
    unsigned char tag_v;
    unsigned char version;
    unsigned char audio_and_video;
    unsigned char DataOffset[4];
}__attribute__ ((aligned (1)));
typedef struct flv_header header_t;

struct flv_tag {
    unsigned char tagtype;
    unsigned char size[3];
    unsigned char timestamp[3];
    unsigned char extent_timestamp;
    unsigned char stream_id[3];
}__attribute__((aligned (1)));
typedef struct flv_tag tag_t;

struct flv_body {
    tag_t tag;
    //   unsigned char PreviousTagSize[2];
};
typedef struct flv_body body_t;

struct object_name {
    unsigned short length;
    unsigned char name;
}__attribute__ ((aligned (1))); ;
typedef struct object_name object_name_t;

struct script_object {
    object_name_t name;
};
typedef struct script_object_name script_object_t;

struct video_tag_body {

};

struct video_tag_header {
    unsigned char frame_type;
    unsigned char codec_id;
    unsigned char avc_packet_type;
    unsigned int composition_time;
    struct video_tag_body tag_body;
};

struct video_key_value {
    char type;
    char *desc;
};

/**
 * Reinterpret a 64-bit integer as a double.
 */
static double int2double(uint64_t i)
{
    union av_intfloat64 v;
    v.i = i;
    return v.f;
}

int get_video_frame_type(unsigned char type)
{
    return (type & 0xF0) >> 4;
}

int get_video_codec_id(unsigned char type)
{
    return type & 0x0F;
}

int get_video_composition_time(unsigned char *data)
{
    char composition_time[4] = {0, 0, 0, 0};

    snprintf(composition_time, sizeof(composition_time), "0x%0.2x%0.2x%0.2x", data[0], data[1], data[2]);
    fprintf(stdout, "[%s %d] composition_time = [%s]\n", __func__, __LINE__, composition_time);
    return strtoul(composition_time, NULL, 16);
}

/*
 * get_header_audio check if the flv have audio stream
 * info, a byte with video and audio bit flag
 *
 * return 0 or 1
*/
int get_header_audio(char info)
{
    return (info & 0x4) >> 2;
}

/*
 * get_header_video check if the flv have video stream
 * info, a byte with video and audio bit flag
 *
 * return 0 or 1
*/
int get_header_video(char info)
{
    return info & 0x1;
}

int get_tag_type (char tag)
{
    return tag & 0x1F;
}

/*
 * flv_header_parse just parse the FLV VERSION tag
 * fd, file describe handle
 * return flv version
*/
int flv_header_parse(int fd)
{
    int ret = 0;
    header_t header;

    memset(&header, 0, sizeof(header));
    ret = read(fd, &header, sizeof(header));

    if (ret <= 0) {
        fprintf(stderr, "read header error, %d\n", -EINVAL);
        return -EINVAL;
    }


    fprintf(stdout, "flv = [%c %c %c] version = [%x]\n"
            "TypeFlagsAudio = [%x], TypeFlagsVideo = [%x], DataOffset = [%x %x %x %x]\n",
            header.tag_f, header.tag_l, header.tag_v, header.version,
            get_header_audio(header.audio_and_video), get_header_video(header.audio_and_video),
            header.DataOffset[0], header.DataOffset[1], header.DataOffset[2], header.DataOffset[3]);

    return (int)header.version;
}


/*
 * flv_do_tag just parse all the flv tag
 * fd, file describe handle
 * size, the size of the flv tag body
 *
 * return tag type
*/
int flv_do_tag(int fd, unsigned long *size)
{
    int ret = 0;
    char *p = NULL;
    char data_size[11] = {0};
    body_t body;


    memset(&body, 0, sizeof(body));

    ret = read(fd, &body, sizeof(body));
    fprintf(stdout, "%s %d ret = [%d]\n", __func__, __LINE__, ret);
    if (ret <= 0) {
        fprintf(stderr, "Can not ret the fd errno [%d]\n", -EINVAL);
        return -EINVAL;
    }

    p = (char *)&body;
//    body.tag.tagtype = get_tag_type(body.tag.tagtype);
    fprintf(stdout, "body . type = [%x]\n", *p);
    fprintf(stdout, "size = %lu tag->type = [%x]"
            "tag->size = [%0.2x %0.2x %0.2x], tag->timestamp = [%0.2x %0.2x %0.2x], "
            "tag->extent_timestamp = [%0.2x], tag->streamid = [%0.2x %0.2x %0.2x]\n",
            sizeof(body), body.tag.tagtype,
            body.tag.size[0], body.tag.size[1],body.tag.size[2], body.tag.timestamp[0], body.tag.timestamp[1],
            body.tag.timestamp[2], body.tag.extent_timestamp, body.tag.stream_id[0],
            body.tag.stream_id[1], body.tag.stream_id[2]);
    memcpy(data_size, body.tag.size, 3);
    p = data_size;
    snprintf(data_size, sizeof(data_size), "0x%0.2x%0.2x%0.2x", body.tag.size[0], body.tag.size[1], body.tag.size[2]);
    *size = strtoul(data_size, NULL, 16);
    fprintf(stdout, "size = [%lu] %s\n", *size, data_size);

    memset(data_size, 0, sizeof(data_size));
    snprintf(data_size, sizeof(data_size), "0x%0.2x%1.2x%0.2x",
            body.tag.timestamp[0], body.tag.timestamp[1], body.tag.timestamp[2]);

    p= data_size;
    if (body.tag.tagtype == 0x09)
    fprintf(stdout, "timestamp = [%lu]\n", strtoul(p, NULL, 16));

    return body.tag.tagtype;
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
        fprintf(stdout, "keyname = [%s]\n", keyname);
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
int script_type_parse(unsigned char *data)
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
        fprintf(stdout, "number = [%.2f]\n", double_number);
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
        fprintf(stdout, "String = [%s]\n", string_output);
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
int do_tag_onMetaData(int fd, int size)
{
    int ret = 0;
    unsigned char *p = NULL;

    unsigned char *onMetaData = malloc(size);
    memset(onMetaData, 0, size);
    ret = read(fd, onMetaData, size);

    int fuck = open("./xxx.dat", O_RDWR);
    write(fuck, onMetaData, ret);
    close(fuck);
    p = onMetaData;

    while (p) {
        if (p - onMetaData >= size) {
            break;
        }
        ret = script_type_parse(p);
        fprintf(stdout, "in while 1 offset = [%ld]\n", p - onMetaData);
        p += ret;
    }
    return p - onMetaData;
}

int parse_video_header(unsigned char *data, struct video_tag_header *header)
{
    unsigned char *p = data;
    struct video_key_value video_frame_type[] = {
        {0, NULL},
        {1, "key frame (for AVC, a seekable frame)"},
        {2, "inter frame (for AVC, a non-seekable frame)"},
        {3, "disposable inter frame (H.263 only)"},
        {4, " generated key frame (reserved for server use only)"},
        {5, "video info/command frame"},
    };

    struct video_key_value codec_type[] = {
        {0, NULL},
        {1, NULL},
        {2, "Sorenson H.263"},
        {3, "Screen video"},
        {4, "On2 VP6"},
        {5, "On2 VP6 with alpha channel"},
        {6, "Screen video version 2"},
        {7, "AVC/H.264"},
    };

    struct video_key_value pkt_type[] = {
        {0, "AVC sequence header"},
        {1, "AVC NALU"},
        {2, "AVC end of sequence"},
    };

    header->frame_type = get_video_frame_type(*p);
    header->codec_id = get_video_codec_id(*p);
    p++;
    if (header->codec_id == 0x07) {
        header->avc_packet_type = *p;
        p++;
        header->composition_time = get_video_composition_time(p);
        p += 3;
    }

    fprintf(stdout, "frame_type = [%s], codec_id = [%s], avc_packet_type = [%s], composition_time = [%d]\n",
            video_frame_type[header->frame_type].desc, codec_type[header->codec_id].desc,
            pkt_type[header->avc_packet_type].desc, header->composition_time);
    return p - data;
}

int do_tag_video(int fd, int size)
{
    int ret = 0;
    unsigned char *p = NULL;
    unsigned char *video_data = malloc(size);
    struct video_tag_header video_header;

    memset(video_data, 0, size);

    p = video_data;
    while ((ret = read(fd, video_data, size)) != 0) {
    fprintf(stdout, "video read = [%d] size = [%d]bytes\n", ret, size);
        size -= ret;
    }

    memset(&video_header, 0, sizeof(video_header));
    while(p)
    {
        ret = parse_video_header(p, &video_header);
        p += ret;
        if (video_header.frame_type == 5) {
            fprintf(stdout, "Video frame payload or frame info [%c]\n", *p);
        } else {
            switch (video_header.codec_id) {
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
                break;
            case 6:
                break;
            case 7:
                fprintf(stdout, "H.264 Data\n");
                return 0;
                break;
            default:
                break;
            }
        }

    }
    return 0;
}

int do_tag_audio(int fd, int size)
{
    int ret = 0;
    unsigned char *audio_data = malloc(size);

    memset(audio_data, 0, size);

    while ((ret = read(fd, audio_data, size)) > 0) {
        size -= ret;
        if (size == 0) {
            break;
        }
        fprintf(stdout, "audio read = [%d] bytes\n", ret);
    }

    return 0;
}

/*
 * dump_flv_info just print the flv onMetaData info
 * filename, the flv filename
 *
 * return always right
*/
int dump_flv_info(char *filename)
{
    int fd = 0;
    int tag_type = -1;
    int ret = 0;
    unsigned long size = 0;
    unsigned char PreviousTagSize[4];
    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        fprintf(stdout, "Can not open the file [%s]\n", filename);
        return -ENOENT;
    }
    fprintf(stdout, "filename = [%s]\n", filename);

    flv_header_parse(fd);
    while ((ret = read(fd, PreviousTagSize, sizeof(PreviousTagSize))) != 0) {
        tag_type = flv_do_tag(fd, &size);
        switch(tag_type) {
        case TAG_TYPE_SCRIPTDATA:
            fprintf(stdout, "data_size = [%lu]\n", size);
            do_tag_onMetaData(fd, size);
            break;

        case TAG_TYPE_VIDEO:
            fprintf(stdout, "Type = [Video]\n");
            do_tag_video(fd, size);
            break;

        case TAG_TYPE_AUDIO:
            fprintf(stdout, "Type = [Audio]\n");
            do_tag_audio(fd, size);
            break;
        }
        memset(PreviousTagSize, 0, sizeof(PreviousTagSize));
        fprintf(stdout, "=================================\n");
    }
    return 0;
}

void usage(char **argv)
{
    fprintf(stderr, "Usage:\n"
            "\t%s flv_file_name\n", argv[0]);
    return ;
}

int main(int argc, char *argv[])
{
    char *filename = NULL;

    if (argc < 2) {
        usage(argv);
        return -EINVAL;
    }

    filename = argv[1];

    dump_flv_info(filename);
    return 0;
}
