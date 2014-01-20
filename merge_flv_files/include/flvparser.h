#ifndef __FLV_PARSER_H__
#define __FLV_PARSER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "flvparser.h"

#define TAG_FLV_F               'F'
#define TAG_FLV_L               'L'
#define TAG_FLV_V               'V'
#define TAG_TYPE_SCRIPTDATA     0x12
#define TAG_TYPE_VIDEO          0x09
#define TAG_TYPE_AUDIO          0x08

#define FRAME_TYPE_KEYFRAME     0x01
#define VIDEO_SEQENCE_START     0x00
#define VIDEO_SEQENCE_END       0x02

#define TRUE                    1
#define FALSE                   0



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

/*
 * flv_header_parse just parse the FLV VERSION tag
 * fd, file describe handle
 * return flv version
*/
int flv_header_parse(int fd, header_t *header);

/*
 * flv_do_tag just parse all the flv tag
 * fd, file describe handle
 * size, the size of the flv tag body
 *
 * return tag type
*/
int flv_do_tag(int fd, unsigned long *size, tag_t *tag);

int do_tag_onMetaData(int fd, int size, unsigned char *onMetaData);



#endif
