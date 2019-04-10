#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MKTAG(a,b,c,d) (((a) << 24) | ((b) << 16) | ((c) << 8) | (d))

struct tag_info {
    unsigned int type;
    char name[5];
};

struct filetypebox {
    unsigned char name[5];
    unsigned char major_brand[5];
    unsigned int minor_version;
    unsigned char *compatible_brands;
};
struct tag_context {
    struct tag_info type;
    struct tag_info context;
    struct tag_context *next;
};

struct ID3v2_header {
    uint8_t fid[4];
    uint32_t version;
    uint8_t flags_a;
    uint8_t flags_b;
    uint8_t flags_c;
    uint8_t flags_d;
    uint32_t size;
};

struct MovieHeaderBox {
    uint64_t creation_time;
    uint64_t modification_time;
    uint64_t timescale;
    uint64_t duration;
    uint32_t rate;
    uint16_t volume;
    uint32_t matrix[9];
    uint32_t next_track_id;
};

struct TrackHeaderBox {
    uint64_t creation_time;
    uint64_t modification_time;
    uint32_t track_ID;
    uint64_t duration;
    int16_t layer;
    int16_t alternate_group;
    int16_t volume;
    int32_t matrix[9];
    uint32_t width;
    uint32_t height;
};

struct MediaHeaderBox {
    uint64_t creation_time;
    uint64_t modification_time;
    uint32_t timescale;
    uint64_t duration;
    char pad;
    uint16_t language;
};

struct HandlerBox {
    uint32_t handler_type;
    char *name;
};

struct VideoMediaHeaderBox {
    uint16_t graphicsmode;
    uint16_t opcolor[3];
};

struct SoundMediaHeaderBox {
    int16_t balance;
};

struct HintMediaHeaderBox {
    uint16_t maxPDUsize;
    uint16_t avgPDUsize;
    uint32_t maxbitrate;
    uint32_t avgbitrate;
};

struct VisualSampleEntry {
    uint16_t width;
    uint16_t height;
    uint32_t horizresolution;
    uint32_t vertresolution;
    uint16_t frame_count;
    char compressorname[32];
    uint16_t depth;
    uint16_t data_reference_index;
};

struct AudioSampleEntry {
    uint16_t channelcount;
    uint16_t samplesize;
    uint32_t samplerate;
    uint16_t data_reference_index;
};

struct PixelAspectRatioBox {
    uint32_t hSpacing;
    uint32_t vSpacing;
};

struct CleanApertureBox {
    uint32_t cleanApertureWidthN;
    uint32_t cleanApertureWidthD;
    uint32_t cleanApertureHeightN;
    uint32_t cleanApertureHeightD;
    uint32_t horizOffN;
    uint32_t horizOffD;
    uint32_t vertOffN;
    uint32_t vertOffD;
};

struct ColourInformationBox {
    uint32_t colour_type;
    uint16_t colour_primaries;
    uint16_t transfer_characteristics;
    uint16_t matrix_coefficients;
    uint8_t full_range_flag;
};

struct HintSampleEntry {
    uint8_t *data;
    uint16_t data_reference_index;
};

struct SampleDescriptionBox {
    uint32_t entry_count;
    struct AudioSampleEntry *ase;
    struct VisualSampleEntry *vse;
    struct HintSampleEntry *hse;
};

struct MovieFragmentHeaderBox {
    uint32_t sequence_number;
};

struct SchemeTypeBox {
    uint8_t scheme_type[5];
    uint32_t scheme_version;
    uint8_t *scheme_uri;
};

struct SchemeInformationBox {
    uint8_t *scheme_specific_data;
};

struct TrackEncryptionBox {
    uint8_t default_crypt_byte_block;
    uint8_t default_skip_byte_block;
    uint8_t default_isProtected;
    uint8_t default_Per_Sample_IV_Size;
    uint8_t defult_KID[16];
    uint8_t default_constant_IV_size;
    uint8_t *default_constant_IV;
};

struct AVCDecoderConfigurationRecord {
    uint8_t configurationVersion;
    uint8_t AVCProfileIndication;
    uint8_t profile_compatibility;
    uint8_t AVCLevelIndication;
    uint8_t lengthSizeMinusOne;
    uint8_t numOfSequenceParameterSets;
    uint16_t sequenceParameterSetLength;
    uint8_t *sequenceParameterSetNALUnit;
    uint8_t numOfPictureParameterSets;
    uint16_t pictureParameterSetLength;
    uint8_t *pictureParameterSetNALUnit;
    uint8_t chroma_format;
    uint8_t bit_depth_luma_minus8;
    uint8_t bit_depth_chroma_minus8;
    uint8_t numOfSequenceParameterSetExt;
    uint16_t sequenceParameterSetExtLength;
    uint8_t *sequenceParameterSetExtNALUnit;
};

struct ProtectionSchemeInfoBox {
    uint32_t data_format;
    struct SchemeTypeBox *schm;
    struct SchemeInformationBox *schi;
};

struct EditListBox {
    uint32_t entry_count; /* unsigned int(32) entry_count; */
    uint64_t segment_duration;
    int64_t media_time;
    int16_t media_rate_integer;
    int16_t media_rate_fraction;
};

struct TimeToSampleBox {
    uint32_t entry_count;
    uint32_t sample_count;
    uint32_t sample_delta;
};

struct CompositionOffsetBox {
    uint32_t entry_count;
    uint32_t sample_count;
    uint32_t sample_offset;
};

struct SampleToChunkBox {
    uint32_t entry_count;
    uint32_t first_chunk;
    uint32_t samples_per_chunk;
    uint32_t sample_description_index;
};

struct SampleSizeBox {
    uint32_t sample_size;
    uint32_t sample_count;
    uint32_t entry_size;
};

struct SyncSampleBox {
    uint32_t entry_count;
    uint32_t sample_number;
};

struct ChunkOffsetBox {
    uint32_t entry_count;
    uint32_t chunk_offset;
};

struct TrackRunBoxSamples {
    uint32_t sample_duration;
    uint32_t sample_size;
    uint32_t sample_flags;
    uint32_t sample_composition_time_offset;
};

struct TrackRunBox {
    uint32_t sample_count;
    int32_t data_offset;
    uint32_t first_sample_flags;
    struct TrackRunBoxSamples trunss[1024];
};

struct TrackFragmentHeaderBox {
    uint32_t track_ID;
    uint64_t base_data_offset;
    uint32_t sample_description_index;
    uint32_t default_sample_duration;
    uint32_t default_sample_size;
    uint32_t default_sample_flags;
};

struct TrackFragmentBaseMediaDecodeTimeBox {
    uint64_t baseMediaDecodeTime;
};

struct ID3v2Box {
    uint32_t pad;
    uint32_t language;
    uint8_t *ID3v2data;
};

struct SPS_NAL {
    int profile_idc;
    int constraint_set0_flag;
    int constraint_set1_flag;
    int constraint_set2_flag;
    int constraint_set3_flag;
    int constraint_set4_flag;
    int constraint_set5_flag;
    int reserved_zero_2bits;
    int level_idc;
};

struct SampleAuxiliaryInformationSizesBox {
    uint32_t aux_info_type;
    uint32_t aux_info_type_parameter;
    uint8_t default_sample_info_size;
    uint32_t sample_count;
    uint8_t *sample_info_size;
};

struct SampleAuxiliaryInformationOffsetsBox {
    uint32_t aux_info_type;
    uint32_t aux_info_type_parameter;
    uint32_t entry_count;
    uint64_t *offset;
};

struct subSampleEncryptionBoxSamples {
    uint16_t BytesOfClearData;
    uint32_t BytesOfProtectedData;
};
struct SampleEncryptionBoxSamples {
    uint64_t InitializationVector;
    uint16_t subsample_count;
    struct subSampleEncryptionBoxSamples subsencs[10240];
};
struct SampleEncryptionBox {
    uint32_t sample_count;
    struct SampleEncryptionBoxSamples sencsamples[10240];
};

/* defined in 14496-1 7.2.6.6 */
struct profileLevelIndicationIndexDescriptor {
    uint8_t profileLevelIndicationIndex;
};
struct DecoderSpecificInfo {
};
struct DecoderConfigDescriptor {
    uint8_t objectTypeIndication;
    uint8_t streamType;
    uint8_t upStream;
    uint8_t reserved;
    uint32_t bufferSizeDB;
    uint32_t maxBitrate;
    uint32_t avgBitrate;
    struct DecoderSpecificInfo decSpecificInfo[2];
    struct profileLevelIndicationIndexDescriptor profileLevelIndicationIndexDescr[256];
};

/* defined in 14496-1 */
struct ES_Descriptor {
    uint16_t ES_ID;
    uint8_t streamDependenceFlag;
    uint8_t URL_Flag;
    uint8_t OCRstreamFlag;
    uint8_t streamPriority;
    uint16_t dependsOn_ES_ID;
    uint8_t URLlength;
    uint8_t *URLstring;
    uint16_t OCR_ES_Id;
    
};
struct tag_info tag_table[] = {
    { MKTAG('f','t','y','p'), "ftyp"},
    { MKTAG('m','p','4','2'), "mp42"},
    { MKTAG('m','v','h','d'), "mvhd"},
    { MKTAG('t','r','a','k'), "trak"},
    { MKTAG('t','k','h','d'), "tkhd"},
    { MKTAG('m','d','i','a'), "mdia"},
    { MKTAG('m','d','h','d'), "mdhd"},
    { MKTAG('h','d','l','r'), "hdlr"},
    { MKTAG('h','d','l','r'), "hdlr"},
    {0, 0},
};

struct tag_context *tags_root;

int32_t get_u8(int fd)
{
    unsigned char buf;
    int32_t ret = 0;
    ret = read(fd, &buf, 1);
    if (ret <= 0) {
        fprintf(stderr, "get u8 error\n");
        return ret;
    }

    return buf;
}

uint32_t get_size(int fd)
{
    unsigned char buf[4];
    int ret = 0;

    read(fd, buf, 4);

    return buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3];
}

uint32_t get_size_from_buf(unsigned char *buf)
{
    int ret = 0;

    return buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3];
}

int get_tag_type(int fd)
{
    unsigned char buf[4];
    read(fd, buf, 4);
    return buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3];
}

unsigned int get_int_from_buf(unsigned char *buf)
{
    return buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3];
}

int get_short_from_buf(unsigned char *buf)
{
    return buf[0] << 8 | buf[1];
}

void get_tag_from_buf(unsigned char *buf, unsigned char *out)
{
    memcpy(out, buf, 4);
}

void get_tag_from_fd(int fd, unsigned char *out)
{
    char buf[5];
    read(fd, buf, 4);
    memcpy(out, buf, 4);
}

uint64_t get_long_from_buf(unsigned char *buf)
{
    uint32_t high = buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3];
    uint32_t low = buf[4] << 24 | buf[5] << 16 | buf[6] << 8 | buf[7];
    uint64_t ret = (uint64_t)high << 32 | (uint64_t)low;

    return ret;
}

int parse_ID3v2_header(unsigned char *in)
{
    printf("in parse_ID3v2_header\n");
    struct ID3v2_header *id32h;
    unsigned char frame_id[5];
    uint32_t frame_size = 0;
    uint32_t frame_flags = 0;
    uint8_t *frame_data = NULL;
    unsigned char *p = in;

    /*
     *The first part of the ID3v2 tag is the 10 byte tag header, laid out
   as follows:

     ID3v2/file identifier      "ID3"
     ID3v2 version              $04 00
     ID3v2 flags                %abcd0000
     ID3v2 size             4 * %0xxxxxxx

   The first three bytes of the tag are always "ID3", to indicate that
   this is an ID3v2 tag, directly followed by the two version bytes. The
   first byte of ID3v2 version is its major version, while the second
   byte is its revision number. In this case this is ID3v2.4.0. All
   revisions are backwards compatible while major versions are not. If
   software with ID3v2.4.0 and below support should encounter version
   five or higher it should simply ignore the whole tag. Version or
   revision will never be $FF.
     *
     * */
    id32h = malloc(sizeof(struct ID3v2_header));
    if (!id32h)
        return -ENOMEM;

    /* ID3v2/file identifier "ID3" */
    strncpy((char *)id32h->fid, (char *)p, 3);
    p += 3;

    /* ID3v2 version $04 00 */
    id32h->version = get_short_from_buf(p);
    p += 2;

    /* ID3v2 flags %abcd0000 */
    id32h->flags_a = (*p & 0x80) >> 7;
    id32h->flags_b = (*p & 0x40) >> 6;
    id32h->flags_c = (*p & 0x20) >> 5;
    id32h->flags_d = (*p & 0x10) >> 4;
    p++;

    /* ID3v2 size 4 * %0xxxxxxx */
    id32h->size = get_int_from_buf(p);
    p += 4;

    printf("id32h->fid = [%s], "
           "id32h->version = 0x%04x, "
           "id32h->size = [0x%08x]\n",
           id32h->fid,
           id32h->version,
           id32h->size);
    /*
     *  a - Unsynchronisation
     *      Bit 7 in the 'ID3v2 flags' indicates whether or not
     *      unsynchronisation is applied on all frames (see section 6.1 for
     *      details); a set bit indicates usage.
     * */
    if (id32h->flags_a) {
        printf("id32h->flags_a\n");
    }

    /*
     * b - Extended header
     *      The second bit (bit 6) indicates whether or not the header is
     *      followed by an extended header. The extended header is described in
     *      section 3.2. A set bit indicates the presence of an extended header.
     * */
    if (id32h->flags_b) {
        printf("id32h->flags_b\n");
    }

    /*
     * c - Experimental indicator
     *      The third bit (bit 5) is used as an 'experimental indicator'. This
     *      flag SHALL always be set when the tag is in an experimental stage.
     * */
    if (id32h->flags_c) {
        printf("id32h->flags_c\n");
    }

    /*
     * d - Footer present
     *      Bit 4 indicates that a footer (section 3.4) is present at the very
     *      end of the tag. A set bit indicates the presence of a footer.
     **/
    if (id32h->flags_d) {
        printf("id32h->flags_d\n");
    }

    /* 4.   ID3v2 frame overview  */
    /*
     *  All ID3v2 frames consists of one frame header followed by one or more
     *  fields containing the actual information. The header is always 10
     *  bytes and laid out as follows:
     *      Frame ID      $xx xx xx xx  (four characters)
     *      Size      4 * %0xxxxxxx
     *      Flags         $xx xx
     *
     *  The frame ID is made out of the characters capital A-Z and 0-9.
     *  Identifiers beginning with "X", "Y" and "Z" are for experimental
     *  frames and free for everyone to use, without the need to set the
     *  experimental bit in the tag header. Bear in mind that someone else
     *  might have used the same identifier as you. All other identifiers are
     *  either used or reserved for future use.
     *
     *  The frame ID is followed by a size descriptor containing the size of
     *  the data in the final frame, after encryption, compression and
     *  unsynchronisation. The size is excluding the frame header ('total
     *  frame size' - 10 bytes) and stored as a 32 bit synchsafe integer.
     *
     * In the frame header the size descriptor is followed by two flag
     * bytes. These flags are described in section 4.1.
     *
     * There is no fixed order of the frames' appearance in the tag,
     * although it is desired that the frames are arranged in order of
     * significance concerning the recognition of the file. An example of
     * such order: UFID, TIT2, MCDI, TRCK ...
     *
     * A tag MUST contain at least one frame. A frame must be at least 1
     * byte big, excluding the header.
     *
     * If nothing else is said, strings, including numeric strings and URLs
     * [URL], are represented as ISO-8859-1 [ISO-8859-1] characters in the
     * range $20 - $FF. Such strings are represented in frame descriptions
     * as <text string>, or <full text string> if newlines are allowed. If
     * nothing else is said newline character is forbidden. In ISO-8859-1 a
     * newline is represented, when allowed, with $0A only.
     *
     * Frames that allow different types of text encoding contains a text
     * encoding description byte. Possible encodings:
     *
     * $00   ISO-8859-1 [ISO-8859-1]. Terminated with $00.
     * $01   UTF-16 [UTF-16] encoded Unicode [UNICODE] with BOM. All
     *      strings in the same frame SHALL have the same byteorder.
     *      Terminated with $00 00.
     * $02   UTF-16BE [UTF-16] encoded Unicode [UNICODE] without BOM.
     *      Terminated with $00 00.
     * $03   UTF-8 [UTF-8] encoded Unicode [UNICODE]. Terminated with $00.
     *
     * Strings dependent on encoding are represented in frame descriptions
     * as <text string according to encoding>, or <full text string
     * according to encoding> if newlines are allowed. Any empty strings of
     * type $01 which are NULL-terminated may have the Unicode BOM followed
     * by a Unicode NULL ($FF FE 00 00 or $FE FF 00 00).
     *
     * The timestamp fields are based on a subset of ISO 8601. When being as
     * precise as possible the format of a time string is
     * yyyy-MM-ddTHH:mm:ss (year, "-", month, "-", day, "T", hour (out of
     * 24), ":", minutes, ":", seconds), but the precision may be reduced by
     * removing as many time indicators as wanted. Hence valid timestamps
     * are yyyy, yyyy-MM, yyyy-MM-dd, yyyy-MM-ddTHH, yyyy-MM-ddTHH:mm and
     * yyyy-MM-ddTHH:mm:ss. All time stamps are UTC. For durations, use
     * the slash character as described in 8601, and for multiple non-
     * contiguous dates, use multiple strings, if allowed by the frame
     * definition.
     *
     * The three byte language field, present in several frames, is used to
     * describe the language of the frame's content, according to ISO-639-2
     * [ISO-639-2]. The language should be represented in lower case. If the
     * language is not known the string "XXX" should be used.
     *
     * All URLs [URL] MAY be relative, e.g. "picture.png", "../doc.txt".
     *
     * If a frame is longer than it should be, e.g. having more fields than
     * specified in this document, that indicates that additions to the
     * frame have been made in a later version of the ID3v2 standard. This
     * is reflected by the revision number in the header of the tag.
    *
    * */
    /* Frame ID      $xx xx xx xx  (four characters) */
    memset(frame_id, 0, sizeof(frame_id));
    strncpy((char *)frame_id, (char *)p, 4);
    p += 4;
    /* Size      4 * %0xxxxxxx */
    frame_size = get_int_from_buf(p);
    p += 4;
    /* Flags         $xx xx */
    frame_flags = get_short_from_buf(p);
    p += 2;

    printf("frame_id = [%s], "
           "frame_size = [0x%08x], "
           "frame_flags = [0x%04x]\n",
           frame_id,
           frame_size,
           frame_flags);
    frame_data = malloc(frame_size);
    if (!frame_data)
        return -ENOMEM;

    int i = 0;
    uint8_t *cur = p;
    while(*p != '\0') {
        frame_data[i] = *p++;
        i++;
    }

    p++;
    printf("frame_data = [%s]\n", frame_data);
    printf("end tag = [%s]\n", p);

    return 0;
}

int parse_ID32(int fd, int id32_size)
{
    printf("\t\t\tin ID32\n");
    struct ID3v2Box *id32;
    int size = id32_size - 4; /* aligned(8) class ID3v2Box extends FullBox('ID32', version=0, 0) */
    char ext_version_flags[5];
    unsigned int tag_name = 0;
    unsigned char *buf = malloc(id32_size);
    unsigned char *p = NULL;
    int i = 0;
    if (!buf)
        return -ENOMEM;

    id32 = malloc(sizeof(struct ID3v2Box));
    if (!id32)
        return -ENOMEM;

    /* read version and flags of the extends */
    read(fd, ext_version_flags, 4);

    /* read id32 context */
    read(fd, buf, size);
    p = buf;

    id32->pad = (*p & 0x80) >> 7;
    id32->language = get_short_from_buf(p) & 0x7FFF;
    p += 2;
    id32->ID3v2data = p;

    printf("id32->pad = [%d]\n", id32->pad);
    printf("id32->language = [%d]\n", id32->language);

    parse_ID3v2_header(p);

    printf("ID32 id32_size = [0x%02x]\n", id32_size);

    return 0;
}


int parse_mvhd(int fd, int mvhd_size)
{
    printf("in mvhd\n");
    struct MovieHeaderBox *mvhd;
    int size = mvhd_size - 4; /* aligned(8) class MovieHeaderBox extends FullBox(‘mvhd’, version, 0)  */
    char ext_version_flags[5];
    unsigned int tag_name = 0;
    unsigned char *buf = malloc(mvhd_size);
    unsigned char *p = NULL;
    int i = 0;

    if (!buf)
        return -ENOMEM;

    mvhd = malloc(sizeof(struct MovieHeaderBox));
    if (!mvhd)
        return -ENOMEM;

    /* read version and flags of the extends */
    read(fd, ext_version_flags, 4);
    /* read mvhd context */
    read(fd, buf, size);
    p = buf;
    if (ext_version_flags[0] == 0) {
        /* unsigned int(32)  creation_time; */
        mvhd->creation_time = get_int_from_buf(p);
        p += 4;
        /* unsigned int(32)  modification_time; */
        mvhd->modification_time = get_int_from_buf(p);
        p += 4;
        /* unsigned int(32)  timescale; */
        mvhd->timescale = get_int_from_buf(p);
        p += 4;
        /* unsigned int(32)  duration; */
        mvhd->duration = get_int_from_buf(p);
        p += 4;
    } else if (ext_version_flags[0] == 1) {
        /* unsigned int(64)  creation_time; */
        mvhd->creation_time = get_long_from_buf(p);
        p += 8;
        /* unsigned int(64)  modification_time; */
        mvhd->modification_time = get_long_from_buf(p);
        p += 8;
        /* unsigned int(32)  timescale; */
        mvhd->timescale = get_int_from_buf(p);
        p += 4;
        /* unsigned int(64)  duration; */
        mvhd->duration = get_long_from_buf(p);
        p += 8;
    } else {
        printf("mvhd version parse error\n");
        return -EINVAL;
    }
    /* template int(32) rate = 0x00010000 */
    mvhd->rate = get_int_from_buf(p);
    p += 4;
    /* template int(16) volume = 0x0100; */
    mvhd->volume = get_short_from_buf(p);
    p += 2;
    p += 2; /* const bit(16) reserved = 0; */
    p += 8; /* const unsigned int(32)[2] reserved = 0 */
    /* template int(32)[9] matrix */
    for (i = 0; i< 9; i++) {
        mvhd->matrix[i] = get_int_from_buf(p);
        p += 4;
    }
    p += 24; /* bit(32)[6]  pre_defined = 0; */
    mvhd->next_track_id = get_int_from_buf(p); /* unsigned int(32)  next_track_ID; */
    p += 4;

    if (p - buf != size) 
        return -EINVAL;

    return 0;
}

int parse_tkhd(int fd, int tkhd_size)
{
    printf("in tkhd\n");

    struct TrackHeaderBox *tkhd;
    int size = tkhd_size - 4; /* aligned(8) class TrackHeaderBox extends FullBox(‘tkhd’, version, 0)  */
    char ext_version_flags[5];
    unsigned int tag_name = 0;
    unsigned char *buf = malloc(tkhd_size);
    unsigned char *p = NULL;
    int i = 0;

    if (!buf)
        return -ENOMEM;

    tkhd = malloc(sizeof(struct TrackHeaderBox));
    if (!tkhd)
        return -ENOMEM;
    memset(tkhd, 0, sizeof(struct TrackHeaderBox));

    /* read version and flags of the extends */
    read(fd, ext_version_flags, 4);
    /* read tkhd context */
    read(fd, buf, size);
    p = buf;
    if (ext_version_flags[0] == 0) {
        /* unsigned int(32)  creation_time; */
        tkhd->creation_time = get_int_from_buf(p);
        p += 4;
        /* unsigned int(32)  modification_time; */
        tkhd->modification_time = get_int_from_buf(p);
        p += 4;
        /* unsigned int(32)  timescale; */
        tkhd->track_ID = get_int_from_buf(p);
        p += 4;
        /* const unsigned int(32)  reserved = 0; */
        p += 4;
        /* unsigned int(32)  duration; */
        tkhd->duration = get_int_from_buf(p);
        p += 4;
    } else if (ext_version_flags[0] == 1) {
        /* unsigned int(64)  creation_time; */
        tkhd->creation_time = get_long_from_buf(p);
        p += 8;
        /* unsigned int(64)  modification_time; */
        tkhd->modification_time = get_long_from_buf(p);
        p += 8;
        /* unsigned int(32)  timescale; */
        tkhd->track_ID = get_int_from_buf(p);
        p += 4;
        /* const unsigned int(32)  reserved = 0; */
        p += 4;
        /* unsigned int(64)  duration; */
        tkhd->duration = get_long_from_buf(p);
        p += 8;
    } else {
        printf("tkhd version parse error\n");
        return -EINVAL;
    }
    /* const unsigned int(32)[2] reserved = 0; */
    p += 8;
    /* template int(16) layer = 0; */
    tkhd->layer = get_short_from_buf(p);
    p += 2;
    /* template int(16) alternate_group = 0; */
    tkhd->alternate_group = get_short_from_buf(p);
    p += 2;
    /* volume
     *  a fixed 8.8 value specifying the track's relative audio volume.
     *  Full volume is 1.0 (0x0100) and is the normal value.
     *  Its value is irrelevant for a purely visual track.
     *  Tracks may be composed by combining them according to their volume,
     *  and then using the overall Movie Header Box volume setting;
     *  or more complex audio composition (e.g. MPEG-4 BIFS) may be used. */
    /* template int(16) volume = 0x0100; */
    tkhd->volume = get_short_from_buf(p) >> 8;
    p += 2;
    p += 2; /* const unsigned int(16) reserved = 0; */
    /* template int(32)[9] matrix */
    printf("\t\t");
    for (i = 0; i< 9; i++) {
        tkhd->matrix[i] = get_int_from_buf(p);
        printf("[0x%08x], ", tkhd->matrix[i]);
        p += 4;
    }
    /*
     * width and height
     *  specify the track's visual presentation size as fixed-point 16.16 values.
     *  These need not be the same as the pixel dimensions of the images,
     *  which is documented in the sample description(s);
     *  all images in the sequence are scaled to this size,
     *  before any overall transformation of the track represented by the matrix.
     *  The pixel dimensions of the images are the default values.
     * */
    tkhd->width = get_int_from_buf(p) >> 16; /* unsigned int(32)  width; */
    p += 4;
    tkhd->height = get_int_from_buf(p) >> 16; /* unsigned int(32)  height; */
    p += 4;
    printf("\n\t\t");
    printf("tkhd->creation_time = [%u]\n\t\t"
           "tkhd->modification_time = [%u]\n\t\t"
           "tkhd->track_ID = [%d]\n\t\t"
           "tkhd->duration = [%u]\n\t\t"
           "tkhd->layer = [%d]\n\t\t"
           "tkhd->alternate_group = [%d]\n\t\t"
           "tkhd->volume = [%d]\n\t\t"
           "tkhd->width = [%d]\n\t\t"
           "tkhd->height = [%d] \n",
           (uint32_t)tkhd->creation_time,
           (uint32_t)tkhd->modification_time,
           (int32_t)tkhd->track_ID,
           (uint32_t)tkhd->duration,
           (int32_t)tkhd->layer,
           (int32_t)tkhd->alternate_group,
           (int32_t)tkhd->volume,
           (int32_t)tkhd->width,
           (int32_t)tkhd->height);
    if (p - buf != size)
        return -EINVAL;
    return 0;
}

int parse_tref(int fd, int size)
{
    printf("in tref\n");
    unsigned char *buf1 = malloc(size);
    if (!buf1)
        return -ENOMEM;
    read(fd, buf1, size);
    return 0;
}

int parse_trgr(int fd, int size)
{
    printf("in trgr\n");
    unsigned char *buf1 = malloc(size);
    if (!buf1)
        return -ENOMEM;
    read(fd, buf1, size);
    return 0;
}
int parse_elst(int fd, int elst_size)
{
    printf("in elst\n");
    struct EditListBox *elst;
    int size = elst_size - 4; /* aligned(8) class  EditListBox extends FullBox(‘elst’, version, 0)  */
    char ext_version_flags[5];
    unsigned int tag_name = 0;
    unsigned char *buf = malloc(elst_size);
    unsigned char *p = NULL;
    int i = 0;

    if (!buf)
        return -ENOMEM;

    elst = malloc(sizeof(struct EditListBox));
    if (!elst)
        return -ENOMEM;

    /* read version and flags of the extends */
    read(fd, ext_version_flags, 4);
    /* read elst context */
    read(fd, buf, size);
    p = buf;

    /* unsigned int(32) entry_count; */
    elst->entry_count = get_int_from_buf(p);
    p += 4;

    for (i = 0; i < elst->entry_count; i++) {
        if (ext_version_flags[0] == 0) {
            /* unsigned int(32) segment_duration; */
            elst->segment_duration = get_int_from_buf(p);
            p += 4;
            /* int(32)  media_time; */
            elst->media_time = get_int_from_buf(p);
            p += 4;
        } else if (ext_version_flags[0] == 1) {
            /* unsigned int(64) segment_duration; */
            elst->segment_duration = get_long_from_buf(p);
            p += 8;
            /* int(32)  media_time; */
            elst->media_time = get_long_from_buf(p);
            p += 8;
        } else {
            printf("mvhd version parse error\n");
            return -EINVAL;
        }
        /* int(16) media_rate_integer; */
        elst->media_rate_integer = get_short_from_buf(p);
        p += 2;
        /* int(16) media_rate_fraction = 0; */
        elst->media_rate_fraction = get_short_from_buf(p);
        p += 2;
        printf("\t\tentry = %d "
               "segment_duration = %lld "
               "media_time = %lld "
               "media_rate_integer = %d "
               "media_rate_fraction = %d\n",
               elst->entry_count,
               elst->segment_duration,
               elst->media_time,
               elst->media_rate_integer,
               elst->media_rate_fraction);
    }

    if (p - buf != size)
        return -EINVAL;

    return 0;
}

int parse_edts(int fd, int edts_size)
{
    int size = edts_size;
    int tag_size = 0;
    unsigned char buf[5];
    unsigned int tag_name = 0;

    printf("in edts\n");
    while(size > 0) {
        tag_size = get_size(fd);
        if (tag_size <= 0) {
            break;
        }
        size -= tag_size;
        memset(buf, 0, sizeof(buf));
        get_tag_from_fd(fd, buf);
        tag_name = get_int_from_buf(buf);
        printf("\t");
        switch (tag_name) {
            case MKTAG('e','l','s','t'):
                parse_elst(fd, tag_size - 8);
                break;
            default:
                break;
        }
    }

    return 0;
}

int parse_mdhd(int fd, int mdhd_size)
{
    printf("in mdhd\n");

    struct MediaHeaderBox *mdhd;
    int size = mdhd_size - 4; /* aligned(8) class MediaHeaderBox extends FullBox(‘mdhd’, version, 0)  */
    char ext_version_flags[5];
    unsigned int tag_name = 0;
    unsigned char *buf = malloc(mdhd_size);
    unsigned char *p = NULL;
    int i = 0;

    if (!buf)
        return -ENOMEM;

    mdhd = malloc(sizeof(struct MediaHeaderBox));
    if (!mdhd)
        return -ENOMEM;
    memset(mdhd, 0, sizeof(struct MediaHeaderBox));

    /* read version and flags of the extends */
    read(fd, ext_version_flags, 4);
    /* read mdhd context */
    read(fd, buf, size);
    p = buf;
    if (ext_version_flags[0] == 0) {
        /* unsigned int(32)  creation_time; */
        mdhd->creation_time = get_int_from_buf(p);
        p += 4;
        /* unsigned int(32)  modification_time; */
        mdhd->modification_time = get_int_from_buf(p);
        p += 4;
        /* unsigned int(32)  timescale; */
        mdhd->timescale = get_int_from_buf(p);
        p += 4;
        /* unsigned int(32)  duration; */
        mdhd->duration = get_int_from_buf(p);
        p += 4;
    } else if (ext_version_flags[0] == 1) {
        /* unsigned int(64)  creation_time; */
        mdhd->creation_time = get_long_from_buf(p);
        p += 8;
        /* unsigned int(64)  modification_time; */
        mdhd->modification_time = get_long_from_buf(p);
        p += 8;
        /* unsigned int(32)  timescale; */
        mdhd->timescale = get_int_from_buf(p);
        p += 4;
        /* unsigned int(64)  duration; */
        mdhd->duration = get_long_from_buf(p);
        p += 8;
    } else {
        printf("mdhd version parse error\n");
        return -EINVAL;
    }

    /* bit(1) pad = 0; */
    mdhd->pad = get_short_from_buf(p) >> 15;
    /* unsigned int(5)[3] language; // ISO-639-2/T language code */
    mdhd->language = get_short_from_buf(p) & 0x7FFF;
    p += 2;
    /* unsigned int(16) pre_defined = 0; */
    p += 2;

    printf("\t\t\t");
    printf("mdhd->creation_time = [%u]\n\t\t\t"
           "mdhd->modification_time = [%u]\n\t\t\t"
           "mdhd->timescale = [%u]\n\t\t\t"
           "mdhd->duration = [%u]\n\t\t\t"
           "mdhd->pad = [%d]\n\t\t\t"
           "mdhd->language = [%d]\n",
           (uint32_t)mdhd->creation_time,
           (uint32_t)mdhd->modification_time,
           (uint32_t)mdhd->timescale,
           (uint32_t)mdhd->duration,
           (int32_t)mdhd->pad,
           (int32_t)mdhd->language);

    if (p - buf != size)
        return -EINVAL;

    return 0;
}

int parse_hdlr(int fd, int hdlr_size, int *handler_type)
{
    printf("in hdlr\n");

    struct HandlerBox *hdlr;
    int size = hdlr_size - 4; /* aligned(8) class HandlerBox extends FullBox(‘hdlr’, version, 0)  */
    char ext_version_flags[5];
    unsigned int tag_name = 0;
    unsigned char *buf = malloc(hdlr_size);
    unsigned char *p = NULL;
    int i = 0;

    if (!buf)
        return -ENOMEM;

    hdlr = malloc(sizeof(struct HandlerBox));
    if (!hdlr)
        return -ENOMEM;
    memset(hdlr, 0, sizeof(struct HandlerBox));

    /* read version and flags of the extends */
    read(fd, ext_version_flags, 4);
    /* read hdlr context */
    read(fd, buf, size);
    p = buf;

    /* unsigned int(32) pre_defined = 0; */
    p += 4;
    /* unsigned int(32) handler_type; */
    hdlr->handler_type = get_int_from_buf(p);
    p += 4;

    switch (hdlr->handler_type) {
        case MKTAG('v','i','d','e'):
            *handler_type = MKTAG('v','i','d','e');
            break;
        case MKTAG('s','o','u','n'):
            *handler_type = MKTAG('s','o','u','n');
            break;
        case MKTAG('h','i','n','t'):
            printf("hint");
            *handler_type = MKTAG('h','i','n','t');
            break;
        case MKTAG('m','e','t','a'):
            printf("meta\n");
            *handler_type = MKTAG('m','e','t','a');
            break;
        case MKTAG('a','u','x','v'):
            printf("auxv");
            *handler_type = MKTAG('a','u','x','v');
            break;
        case MKTAG('I','D','3','2'):
            *handler_type = MKTAG('I','D','3','2');
            break;
        default:
            printf("unknown handler type\n");
            break;
    }

    /* const unsigned int(32)[3] reserved = 0; */
    p += 12;

    if (size - (p - buf) > 0) {
        hdlr->name = malloc(size - (p - buf));
        if (!hdlr->name) {
            return -ENOMEM;
        }
        strncpy(hdlr->name, (char *)p, size - (p - buf));
        p += size - (p - buf);
        printf("\t\t\thdlr->name = [%s]\n", hdlr->name);
    }
    if (p - buf != size)
        return -EINVAL;
    return 0;
}

int parse_udta(int fd, int size)
{
    printf("in udta\n");
    unsigned char *buf1 = malloc(size);
    if (!buf1)
        return -ENOMEM;
    read(fd, buf1, size);
    return 0;
}

int parse_vmhd(int fd, int vmhd_size)
{
    printf("in vmhd\n");

    struct VideoMediaHeaderBox *vmhd;
    int size = vmhd_size - 4; /* aligned(8) class VideoMediaHeaderBox extends FullBox(‘vmhd’, version, 0)  */
    char ext_version_flags[5];
    unsigned int tag_name = 0;
    unsigned char *buf = malloc(vmhd_size);
    unsigned char *p = NULL;
    int i = 0;

    if (!buf)
        return -ENOMEM;

    vmhd = malloc(sizeof(struct VideoMediaHeaderBox));
    if (!vmhd)
        return -ENOMEM;
    memset(vmhd, 0, sizeof(struct VideoMediaHeaderBox));

    /* read version and flags of the extends */
    read(fd, ext_version_flags, 4);
    /* read vmhd context */
    read(fd, buf, size);
    p = buf;

    /* template unsigned int(16) graphicsmode = 0; // copy, see below */
    /* graphicsmode specifies a composition mode for this video track,
     * from the following enumerated set,
     * which may be extended by derived specifications:
     *  copy = 0 copy over the existing image */
    vmhd->graphicsmode = get_short_from_buf(p);
    p += 2;
    /* opcolor
     * a set of 3 colour values (red, green, blue) available for use by graphics modes
     * */
    vmhd->opcolor[0] = get_short_from_buf(p);
    p += 2;
    vmhd->opcolor[1] = get_short_from_buf(p);
    p += 2;
    vmhd->opcolor[2] = get_short_from_buf(p);
    p += 2;

    printf("\t\t\t\t");
    printf("vmhd->graphicsmode = [%d]\n\t\t\t\t"
           "vmhd->opcolor[0] = [%d]\n\t\t\t\t"
           "vmhd->opcolor[1] = [%d]\n\t\t\t\t"
           "vmhd->opcolor[2] = [%d]\n",
           vmhd->graphicsmode,
           vmhd->opcolor[0],
           vmhd->opcolor[1],
           vmhd->opcolor[2]);
    if (p - buf != size)
        return -EINVAL;

    return 0;
}

int parse_smhd(int fd, int smhd_size)
{
    printf("in smhd\n");

    struct SoundMediaHeaderBox *smhd;
    int size = smhd_size - 4; /* aligned(8) class SoundMediaHeaderBox extends FullBox(‘smhd’, version, 0)  */
    char ext_version_flags[5];
    unsigned int tag_name = 0;
    unsigned char *buf = malloc(smhd_size);
    unsigned char *p = NULL;
    int i = 0;

    if (!buf)
        return -ENOMEM;

    smhd = malloc(sizeof(struct SoundMediaHeaderBox));
    if (!smhd)
        return -ENOMEM;
    memset(smhd, 0, sizeof(struct SoundMediaHeaderBox));

    /* read version and flags of the extends */
    read(fd, ext_version_flags, 4);
    /* read smhd context */
    read(fd, buf, size);
    p = buf;

    /* balance
     *  a fixed-point 8.8 number that places mono audio tracks in a stereo space;
     *      0 is centre (the normal value);
     *      full left is -1.0 and full right is 1.0.
     * */
    smhd->balance = get_short_from_buf(p) >> 8;
    p += 2;
    /* const unsigned int(16)  reserved = 0; */
    p += 2;
    printf("\t\t\t\tsmhd->balance = [%d]\n", smhd->balance);
    if (p - buf != size)
        return -EINVAL;

    return 0;
}

int parse_hmhd(int fd, int hmhd_size)
{
    printf("in hmhd\n");
    struct  HintMediaHeaderBox *hmhd;
    int size = hmhd_size - 4; /* aligned(8) class HintMediaHeaderBox extends FullBox(‘hmhd’, version, 0)  */
    char ext_version_flags[5];
    unsigned int tag_name = 0;
    unsigned char *buf = malloc(hmhd_size);
    unsigned char *p = NULL;
    int i = 0;

    if (!buf)
        return -ENOMEM;

    hmhd = malloc(sizeof(struct HintMediaHeaderBox));
    if (!hmhd)
        return -ENOMEM;
    memset(hmhd, 0, sizeof(struct HintMediaHeaderBox));

    /* read version and flags of the extends */
    read(fd, ext_version_flags, 4);
    /* read hmhd context */
    read(fd, buf, size);
    p = buf;

    /* unsigned int(16)  maxPDUsize; */
    hmhd->maxPDUsize = get_short_from_buf(p);
    p += 2;
    /* unsigned int(16)  avgPDUsize; */
    hmhd->avgPDUsize = get_short_from_buf(p);
    p += 2;
    /* unsigned int(32)  maxbitrate; */
    hmhd->maxbitrate = get_int_from_buf(p);
    p += 4;
    /* unsigned int(32)  avgbitrate; */
    hmhd->avgbitrate = get_int_from_buf(p);
    p += 4;
    /* unsigned int(32)  reserved = 0; */
    p += 4;

    if (p - buf != size)
        return -EINVAL;

    return 0;
}

int parse_nmhd(int fd, int size)
{
    printf("in nmhd\n");
    unsigned char *buf1 = malloc(size);
    if (!buf1)
        return -ENOMEM;
    read(fd, buf1, size);
    free(buf1);
    buf1 = NULL;

    return 0;
}

/*
 *  TODO:
 *  8.7.2.2 Syntax
 *  aligned(8) class DataEntryUrlBox (bit(24) flags)
 *      extends FullBox(‘url ’, version = 0, flags) {
 *          string location;
 *      }
 *  aligned(8) class DataEntryUrnBox (bit(24) flags)
 *      extends FullBox(‘urn ’, version = 0, flags) {
 *          string name;
 *          string location;
 *          }
 *  aligned(8) class DataReferenceBox
 *      extends FullBox(‘dref’, version = 0, 0) {
 *      unsigned int(32)  entry_count;
 *      for (i=1; i <= entry_count; i++) {
 *          DataEntryBox(entry_version, entry_flags) data_entry;
 *      }
 *  }
 *  8.7.2.3 Semantics
 *  version
 *      an integer that specifies the version of this box
 *  entry_count
 *      an integer that counts the actual entries
 *  entry_version
 *      an integer that specifies the version of the entry format
 *  entry_flags
 *      a 24-bit integer with flags; one flag is defined (x000001) which means that the media
 *  data
 *      in the same file as the Movie Box containing this data reference.
 *      data_entry is a URL or URN entry. Name is a URN, and is required in a URN entry.
 *      Location is a URL, and is required in a URL entry and optional in a URN entry,
 *      where it gives a location to find the resource with the given name.
 *      Each is a null-terminated string using UTF-8 characters.
 *      If the self-contained flag is set, the URL form is used and no string is present;
 *      the box terminates with the entry-flags field.
 *      The URL type should be of a service that delivers a file (e.g. URLs of type file, http, ftp etc.),
 *      and which services ideally also permit random access.
 *      Relative URLs are permissible and are relative to the file containing the Movie Box that contains this data reference.
 * */
int parse_dref(int fd, int size)
{
    printf("in dref\n");
    unsigned char *buf1 = malloc(size);
    if (!buf1)
        return -ENOMEM;
    read(fd, buf1, size);
    free(buf1);
    buf1 = NULL;
    return 0;
}

int parse_dinf(int fd, int dinf_size)
{
    int size = dinf_size;
    int tag_size = 0;
    unsigned char buf[5];
    unsigned int tag_name = 0;

    printf("in dinf\n");
    while(size > 0) {
        tag_size = get_size(fd);
        if (tag_size <= 0) {
            break;
        }
        size -= tag_size;
        memset(buf, 0, sizeof(buf));
        get_tag_from_fd(fd, buf);
        tag_name = get_int_from_buf(buf);
        printf("\t\t\t\t");
        switch (tag_name) {
            case MKTAG('d','r','e','f'):
                parse_dref(fd, tag_size - 8);
                break;
            default:
                break;
        }
    }

    return 0;
}

int parse_esds(unsigned char *in, int esds_size)
{
    printf("in esds\n");
    unsigned char *p = in;
    int size = esds_size - 4; /* class ESDBox extends Box(‘esds’) */

    if (!in)
        return -ENOMEM;

    printf("size = [%ld] esds_size = [%d]\n", p - in, esds_size);
    return 0;
}


int parse_clap(unsigned char *in, int clap_size)
{
    printf("in colr\n");
    unsigned char *p = in;
    struct CleanApertureBox *clap;
    int size = clap_size - 4; /* class CleanApertureBox extends Box(‘clap’) */

    if (!in)
        return -ENOMEM;

    clap = malloc(sizeof(struct CleanApertureBox));
    if (!clap)
        return -ENOMEM;
    memset(clap, 0, sizeof(struct CleanApertureBox));
    clap->cleanApertureWidthN = get_int_from_buf(p);
    p += 4;
    clap->cleanApertureWidthD = get_int_from_buf(p);
    p += 4;
    clap->cleanApertureHeightN = get_int_from_buf(p);
    p += 4;
    clap->cleanApertureHeightD = get_int_from_buf(p);
    p += 4;
    clap->horizOffN = get_int_from_buf(p);
    p += 4;
    clap->horizOffD = get_int_from_buf(p);
    p += 4;
    clap->vertOffN = get_int_from_buf(p);
    p += 4;
    clap->vertOffD = get_int_from_buf(p);
    p += 4;

    printf("size = [%ld] colr_size = [%d]\n", p - in, clap_size);
    return 0;
}

int parse_pasp(unsigned char *in, int pasp_size)
{
    printf("in pasp\n");
    unsigned char *p = in;
    struct PixelAspectRatioBox *pasp;
    int size = pasp_size; /* class PixelAspectRatioBox extends Box(‘pasp’) */

    if (!in)
        return -ENOMEM;

    pasp = malloc(sizeof(struct PixelAspectRatioBox));
    if (!pasp)
        return -ENOMEM;

    memset(pasp, 0, sizeof(struct PixelAspectRatioBox));
    pasp->hSpacing = get_int_from_buf(p);
    p += 4;
    pasp->vSpacing = get_int_from_buf(p);
    p += 4;

    printf("\t\t\t\t\t\t\t");
    printf("pasp->hSpacing = [%d]\n\t\t\t\t\t\t\t"
           "pasp->vSpacing = [%d]\n",
           pasp->hSpacing, pasp->vSpacing);
    return 0;
}

int parse_colr(unsigned char *in, int colr_size)
{
    printf("in colr\n");
    unsigned char *p = in;
    struct ColourInformationBox *colr;
    int size = colr_size; /* class ColourInformationBox extends Box(‘colr’) */

    if (!in)
        return -ENOMEM;

    colr = malloc(sizeof(struct ColourInformationBox));
    if (!colr)
        return -ENOMEM;

    memset(colr, 0, sizeof(struct ColourInformationBox));
    /* unsigned int(32) colour_type; */
    colr->colour_type = get_int_from_buf(p);
    p += 4;

    if (colr->colour_type == MKTAG('n','c','l','x')) {
        /* unsigned int(16) colour_primaries; */
        colr->colour_primaries = get_short_from_buf(p);
        p += 2;
        /* unsigned int(16) transfer_characteristics; */
        colr->transfer_characteristics = get_short_from_buf(p);
        p += 2;
        /* unsigned int(16) matrix_coefficients */
        colr->matrix_coefficients = get_short_from_buf(p);
        p += 2;
        /* unsigned int(1) full_range_flag; */
        colr->full_range_flag = get_short_from_buf(p) & 0x80;
        p++;
    }

    printf("\t\t\t\t\t\t\t");
    printf("colr->colour_primaries = [%d]\n\t\t\t\t\t\t\t"
           "colr->transfer_characteristics = [%d]\n\t\t\t\t\t\t\t"
           "colr->matrix_coefficients = [%d]\n\t\t\t\t\t\t\t"
           "colr->full_range_flag = [%d]\n",
           colr->colour_primaries,
           colr->transfer_characteristics,
           colr->matrix_coefficients,
           colr->full_range_flag);
    printf("\t\t\t\t\t\t\tsize = [%ld] colr_size = [%d]\n", p - in, colr_size);
    return 0;
}

int parse_schm(unsigned char *in, int schm_size)
{
    printf("in schm\n");

    struct SchemeTypeBox *schm;
    int size = schm_size - 4; /* aligned(8) class SchemeTypeBox extends FullBox(‘schm’, version, 0)  */
    char ext_version_flags[5];
    unsigned int tag_name = 0;
    unsigned char *p = in;
    int i = 0;

    if (!in)
        return -ENOMEM;

    schm = malloc(sizeof(struct SchemeTypeBox));
    if (!schm)
        return -ENOMEM;
    memset(schm, 0, sizeof(struct SchemeTypeBox));

    /* read version and flags of the extends */
    for (i = 0; i < 4; i++) {
        ext_version_flags[i] = p[i];
    }
    p += 4;

    /* unsigned int(32) scheme_type; */
    get_tag_from_buf(p, schm->scheme_type);
    p += 4;
    /* unsigned int(32) scheme_version; */
    schm->scheme_version = get_int_from_buf(p);
    p += 4;

    if (ext_version_flags[1] == 0x00 && ext_version_flags[2] == 0x00 && ext_version_flags[2] == 0x01) {
        printf("unsigned int(8) scheme_uri[]; // browser uri in 8.12.5.2\n");
        return -ENOMEM;
    }

    printf("\t\t\t\t\t\t\t\tschm->scheme_type = [%s] "
           "schm->scheme_version = [0x%08x]\n",
           schm->scheme_type,
           schm->scheme_version);

    if (p - in != size)
        return -EINVAL;

    return 0;
}

/* tenc is defined in 23001-7 8.2.1 Definition */
int parse_tenc(unsigned char *in, int tenc_size)
{
    printf("in tenc\n");
    struct TrackEncryptionBox *tenc;
    int size = tenc_size - 4; /* aligned(8) class TrackEncryptionBox extends FullBox(‘tenc’, version, 0)  */
    char ext_version_flags[5];
    unsigned int tag_name = 0;
    unsigned char *p = in;
    int i = 0;

    if (!in)
        return -ENOMEM;
    tenc = malloc(sizeof(struct TrackEncryptionBox));
    if (!tenc)
        return -ENOMEM;
    memset(tenc, 0, sizeof(struct TrackEncryptionBox));

    /* read version and flags of the extends */
    for (i = 0; i < 4; i++) {
        ext_version_flags[i] = p[i];
    }
    p += 4;
    /* unsigned int(8) reserved = 0; */
    p++;

    if (ext_version_flags[0] == 0x00) {
        /* unsigned int(8) reserved = 0; */
    } else {
        /* unsigned int(4) default_crypt_byte_block; */
        tenc->default_crypt_byte_block = (*p & 0xF0) >> 4;
        /* unsigned int(4) default_skip_byte_block; */
        tenc->default_skip_byte_block = *p & 0x0F;
    }
    p++;
    /* unsigned int(8) default_isProtected; */
    tenc->default_isProtected = *p;
    p++;
    /* unsigned int(8) default_Per_Sample_IV_Size; */
    tenc->default_Per_Sample_IV_Size = *p;
    p++;
    /* unsigned int(8)[16] defult_KID; */
    memcpy(tenc->defult_KID, p, sizeof(tenc->defult_KID));
    p += 16;

    if (tenc->default_isProtected && !tenc->default_Per_Sample_IV_Size) {
        /* unsigned int(8) default_constant_IV_size; */
        tenc->default_constant_IV_size = *p;
        p++;
        /* unsigned int(8) [define_constant_IV_size] default_constant_IV; */
        if (tenc->default_constant_IV_size > 0) {
            tenc->default_constant_IV = malloc(tenc->default_constant_IV_size);
            memcpy(tenc->default_constant_IV, p, tenc->default_constant_IV_size);
            p += tenc->default_constant_IV_size;
        }
    }

    printf("\t\t\t\t\t\t\t\t\t");
    printf("tenc->default_crypt_byte_block = [%d]\n\t\t\t\t\t\t\t\t\t"
          "tenc->default_skip_byte_block = [%d]\n\t\t\t\t\t\t\t\t\t"
          "tenc->default_isProtected = [%d]\n\t\t\t\t\t\t\t\t\t"
          "tenc->default_Per_Sample_IV_Size = [%d]\n\t\t\t\t\t\t\t\t\t"
          "tenc->default_constant_IV_size = [%d]",
          tenc->default_crypt_byte_block,
          tenc->default_skip_byte_block,
          tenc->default_isProtected,
          tenc->default_Per_Sample_IV_Size,
          tenc->default_constant_IV_size);

    printf("\n\t\t\t\t\t\t\t\t\t");
    printf("tenc->defult_KID [");
    for (i = 0; i < 16; i++) {
        printf(" 0x%02x ", tenc->defult_KID[i]);
    }
    printf("]");
    printf("\n\t\t\t\t\t\t\t\t\t");
    printf("tenc->default_constant_IV [");
    for (i = 0; i < tenc->default_constant_IV_size; i++) {
        printf(" 0x%02x ", tenc->default_constant_IV[i]);
    }
    printf("]");
    printf("\n");
    printf("\t\t\t\t\t\t\t\t\t");
    printf("tenc size = [%ld] size = [%d]\n", p - in, tenc_size);
    if (p - in != tenc_size)
        return -EINVAL;

    return 0;
}

int parse_schi(unsigned char *in, int schi_size)
{
    printf("in schi\n");
    unsigned char *p = in;
    int tmp_size = 0;
    unsigned char tag_buf[5];
    uint32_t tag_name = 0;
    struct SchemeInformationBox *schi;
    int context_size = schi_size;

    schi = malloc(sizeof(struct SchemeInformationBox));
    if (!schi)
        return -ENOMEM;

    while (context_size > 0) {
        tmp_size = get_size_from_buf(p);
        p += 4;
        memset(tag_buf, 0, sizeof (tag_buf));
        get_tag_from_buf(p, tag_buf);
        p += 4;
        tag_name = get_int_from_buf(tag_buf);
        printf("\t\t\t\t\t\t\t\t");
        switch (tag_name) {
            case MKTAG('t','e','n','c'):
                parse_tenc(p, tmp_size - 8);
                p += tmp_size - 8;
                break;
            default:
                printf("unkown, [%s]\n", tag_buf);
                break;
        }

        context_size -= tmp_size;
    }

    printf("\t\t\t\t\t\t\t\tschi size = [%d]\n", schi_size);

    return 0;
}


int parse_sinf(unsigned char *buf, int size)
{
    printf("in sinf\n");
    unsigned char *p = buf;
    int tmp_size = 0;
    unsigned char tag_buf[5];
    uint32_t tag_name = 0;
    struct ProtectionSchemeInfoBox *psib;
    int context_size = size;

    psib = malloc(sizeof(struct ProtectionSchemeInfoBox));
    if (!psib)
        return -ENOMEM;

    while (context_size > 0) {
        tmp_size = get_size_from_buf(p);
        p += 4;
        memset(tag_buf, 0, sizeof (tag_buf));
        get_tag_from_buf(p, tag_buf);
        p += 4;
        tag_name = get_int_from_buf(tag_buf);
        printf("\t\t\t\t\t\t\t");
        switch (tag_name) {
            case MKTAG('f','r','m','a'):
                printf("in frma\n");
                get_tag_from_buf(p, tag_buf);
                p += 4;
                printf("\t\t\t\t\t\t\t\tin %s\n", tag_buf);
                break;
            case MKTAG('s','c','h','m'):
                parse_schm(p, tmp_size - 8);
                p += tmp_size - 8;
                break;
            case MKTAG('s','c','h','i'):
                parse_schi(p, tmp_size - 8);
                p += tmp_size - 8;
                break;
            case MKTAG('m','p','4','a'):
                parse_mp4a(p, tmp_size - 8);
                p += tmp_size - 8;
                break;
            default:
                printf("unkown, [%s]\n", tag_buf);
                break;
        }

        context_size -= tmp_size;
    }

    return 0;
}

int parse_mp4a(unsigned char *in, int mp4a_size)
{
    printf("in mp4a mp4_size = [%d]\n", mp4a_size);
    unsigned char *p = in;
    uint32_t samplerate = 0;
    int32_t tmp_size = 0;
    unsigned char tmpbuf[5];
    uint32_t tag_name = 0;

    /* const unsigned int(8)[6] reserved = 0;
     * unsigned int(16) data_reference_index;*/
    p += 8;

    /* const unsigned int(32)[2] reserved = 0;
     * template unsigned int(16) channelcount = 2;
     * template unsigned int(16) samplesize = 16;
     * unsigned int(16) pre_defined = 0;
     * const unsigned int(16) reserved = 0 ;
     * */
    p += 8 + 2 + 2 + 2 + 2;

    /* template unsigned int(32) samplerate = { default samplerate of media}<<16; } */
    samplerate = get_int_from_buf(p) >> 16;
    p += 4;
    printf("samplerate = %u\n", samplerate);
    tmp_size = get_size_from_buf(p);
    p += 4;
    get_tag_from_buf(p, tmpbuf);
    p += 4;
    tag_name = get_int_from_buf(tmpbuf);
    printf("tag_name = [%x]\n", tag_name);
    switch (tag_name) {
        case MKTAG('e','s','d','s'):
            parse_esds(p, tmp_size - 8);
            break;
        default:
            printf("unkown mp4a, [%s]\n", tmpbuf);
            break;
    }



    return 0;
}


int parse_SPS_NALU(unsigned char *in, int sps_size)
{
    printf("in SPS NALU\n");
    unsigned char *p = in;

    printf("SPS p = [%x]\n", p[0]);


    return 0;
}

int parse_avcC(unsigned char *in, int avcc_size)
{
    printf("in avcC\n");

    unsigned char *p = in;
    int tmp_size = 0;
    unsigned char tag_buf[5];
    uint32_t tag_name = 0;
    struct AVCDecoderConfigurationRecord *avcc;
    int context_size = avcc_size;
    int i = 0;
    int j = 0;

    avcc = malloc(sizeof(struct AVCDecoderConfigurationRecord));
    if (!avcc)
        return -ENOMEM;

    /* unsigned int(8) configurationVersion = 1; */
    avcc->configurationVersion = *p;
    p++;
    /* unsigned int(8) AVCProfileIndication; */
    avcc->AVCProfileIndication = *p;
    p++;
    /* unsigned int(8) profile_compatibility; */
    avcc->profile_compatibility = *p;
    p++;
    /* unsigned int(8) AVCLevelIndication; */
    avcc->AVCLevelIndication = *p;
    p++;
    /* bit(6) reserved = ‘111111’b;
     * unsigned int(2) lengthSizeMinusOne;
     * */
    avcc->lengthSizeMinusOne = *p & 0x03;
    p++;
    /* bit(3) reserved = ‘111’b;
     * unsigned int(5) numOfSequenceParameterSets;
     * */
    avcc->numOfSequenceParameterSets = *p & 0x1F;
    p++;

    for (i = 0; i < avcc->numOfSequenceParameterSets; i++) {
        /* unsigned int(16) sequenceParameterSetLength ; */
        avcc->sequenceParameterSetLength = get_short_from_buf(p);
        p += 2;
        if (avcc->sequenceParameterSetLength > 0) {
            /* bit(8*sequenceParameterSetLength) sequenceParameterSetNALUnit; */
            avcc->sequenceParameterSetNALUnit = malloc(avcc->sequenceParameterSetLength);
            if (!avcc->sequenceParameterSetNALUnit)
                return -ENOMEM;
            memcpy(avcc->sequenceParameterSetNALUnit, p, avcc->sequenceParameterSetLength);
            p += avcc->sequenceParameterSetLength;
        }
    }
    /* unsigned int(8) numOfPictureParameterSets; */
    avcc->numOfPictureParameterSets = *p;
    p++;
    for (i = 0; i < avcc->numOfPictureParameterSets; i++) {
        /* unsigned int(16) pictureParameterSetLength; */
        avcc->pictureParameterSetLength = get_short_from_buf(p);
        p += 2;
        if (avcc->pictureParameterSetLength> 0) {
            /* bit(8*pictureParameterSetLength) pictureParameterSetNALUnit; */
            avcc->pictureParameterSetNALUnit = malloc(avcc->pictureParameterSetLength);
            if (!avcc->pictureParameterSetNALUnit)
                return -ENOMEM;
            memcpy(avcc->pictureParameterSetNALUnit, p, avcc->pictureParameterSetLength);
            p += avcc->pictureParameterSetLength;
        }
    }

    printf("\t\t\t\t\t\t\t");
    printf("avcc->configurationVersion = [%d]\n\t\t\t\t\t\t\t"
           "avcc->AVCProfileIndication = [%d]\n\t\t\t\t\t\t\t"
           "avcc->profile_compatibility = [%d]\n\t\t\t\t\t\t\t"
           "avcc->AVCLevelIndication = [%d]\n\t\t\t\t\t\t\t"
           "avcc->lengthSizeMinusOne = [%d]\n\t\t\t\t\t\t\t"
           "avcc->numOfSequenceParameterSets = [%d]\n\t\t\t\t\t\t\t"
           "avcc->numOfPictureParameterSets = [%d]\n",
           avcc->configurationVersion,
           avcc->AVCProfileIndication,
           avcc->profile_compatibility,
           avcc->AVCLevelIndication,
           avcc->lengthSizeMinusOne,
           avcc->numOfSequenceParameterSets,
           avcc->numOfPictureParameterSets);
    printf("\t\t\t\t\t\t\t");
    for (i = 0; i < avcc->numOfSequenceParameterSets; i++) {
        printf("avcc->sequenceParameterSetLength = [%d]\n\t\t\t\t\t\t\t"
               "avcc->sequenceParameterSetNALUnit  ", avcc->sequenceParameterSetLength);
//        printf("\t\t\t\t\t\t\t");
        printf("[");
        for (j = 0; j < avcc->sequenceParameterSetLength; j++) {
            printf("0x%02x ", avcc->sequenceParameterSetNALUnit[j]);
        }
        printf("]");
        parse_SPS_NALU(avcc->sequenceParameterSetNALUnit, avcc->sequenceParameterSetLength);
    }
    printf("\n\t\t\t\t\t\t\t");
    for (i = 0; i < avcc->numOfPictureParameterSets; i++) {
        printf("avcc->pictureParameterSetLength = [%d]\n\t\t\t\t\t\t\t"
               "avcc->pictureParameterSetNALUnit  ", avcc->pictureParameterSetLength);
//        printf("\t\t\t\t\t\t\t");
        printf("[");
        for (j = 0; j < avcc->pictureParameterSetLength; j++) {
            printf("0x%02x ", avcc->pictureParameterSetNALUnit[j]);
        }
        printf("]");
    }
    printf("\n");
    printf("\t\t\t\t\t\t\tavcC size = [%ld] [%d]\n", p - in, avcc_size);
    return 0;
}

int parse_fiel(unsigned char *buf, int size)
{
    printf("in fiel\n");
    unsigned char *p = buf;

    return 0;
}
int parse_chrm(unsigned char *buf, int size)
{
    printf("in chrm\n");
    unsigned char *p = buf;

    return 0;
}


int parse_encv(unsigned char *buf, int size, struct VisualSampleEntry *vse, int tagid)
{
    if (tagid == MKTAG('e','n','c','v'))
        printf("in encv\n");
    else
        printf("in avc1\n");
    unsigned char *p = buf;
    /* const unsigned int(8)[6] reserved = 0; */
    p += 6;
    /* unsigned int(16) data_reference_index; */
    vse->data_reference_index = get_short_from_buf(p);
    p += 2;
    /* unsigned int(16) pre_defined = 0; */
    /* const unsigned int(16) reserved = 0; */
    /* unsigned int(32)[3] pre_defined = 0; */
    p += 16;
    /* unsigned int(16) width; */
    vse->width = get_short_from_buf(p);
    p += 2;
    /* unsigned int(16) height; */
    vse->height = get_short_from_buf(p);
    p += 2;
    /* unsigned int(32) horizresolution = 0x00480000; */
    vse->horizresolution = get_int_from_buf(p);
    p += 4;
    /* unsigned int(32) vertresolution = 0x00480000; */
    vse->vertresolution = get_int_from_buf(p);
    p += 4;
    /* const unsigned int(32) reserved = 0; */
    p += 4;
    /* unsigned int(16) frame_count = 1; */
    vse->frame_count = get_short_from_buf(p);
    p += 2;
    /* string[32] compressorname; */
    strncpy((char *)vse->compressorname, (char *)p, 32);
    p += 32;
    /* template unsigned int(16) depth = 0x0018; */
    vse->depth = get_short_from_buf(p);
    p += 2;
    /* int(16) pre_defined = -1; */
    p += 2;
    printf("\t\t\t\t\t\t");
    printf("vse->width = [%d] vse->height = [%d]\n\t\t\t\t\t\t"
           "vse->horizresolution = [0x%08x]\n\t\t\t\t\t\t"
           "vse->vertresolution = [0x%08x]\n\t\t\t\t\t\t"
           "vse->frame_count = [%d]\n\t\t\t\t\t\t"
           "vse->compressorname = [%s]\n\t\t\t\t\t\t"
           "vse->depth = [0x%04x]\n",
           vse->width, vse->height,
           vse->horizresolution,
           vse->vertresolution,
           vse->frame_count,
           vse->compressorname,
           vse->depth);


    int context_size = size - (p - buf);
    int tmp_size = 0;
    unsigned char tmpbuf[5];
    uint32_t tag_name = 0;

    while (context_size > 0) {
        tmp_size = get_size_from_buf(p);
        p += 4;
        get_tag_from_buf(p, tmpbuf);
        p += 4;
        tag_name = get_int_from_buf(tmpbuf);
        printf("\t\t\t\t\t\t");
        switch (tag_name) {
            case MKTAG('c','o','l','r'):
                parse_colr(p, tmp_size - 8);
                break;
            case MKTAG('p','a','s','p'):
                parse_pasp(p, tmp_size - 8);
                break;
            case MKTAG('c','l','a','p'):
                parse_clap(p, tmp_size - 8);
                break;
            case MKTAG('s','i','n','f'):
                parse_sinf(p, tmp_size - 8);
                break;
            case MKTAG('a','v','c','C'):
                parse_avcC(p, tmp_size - 8);
                break;
            case MKTAG('f','i','e','l'):
                parse_fiel(p, tmp_size - 8);
                break;
            case MKTAG('c','h','r','m'):
                parse_chrm(p, tmp_size - 8);
                break;
            default:
                printf("unkown vide encv, [%s]\n", tmpbuf);
                break;
        }
        p += tmp_size;
        p -= 8;
        context_size -= tmp_size;
    }


    return p - buf;
}

int parse_enca(unsigned char *buf, int size, struct AudioSampleEntry *ase)
{
    printf("in enca size = [%d]\n", size);
    unsigned char *p = buf;
    int esds_size = 0;
    /* const unsigned int(8)[6] reserved = 0; */
    p += 6;
    /* unsigned int(16) data_reference_index; */
    ase->data_reference_index = get_short_from_buf(p);
    p += 2;

    /* const unsigned int(32)[2] reserved = 0; */
    p += 8;

    /* template unsigned int(16) channelcount = 2; */
    ase->channelcount = get_short_from_buf(p);
    p += 2;
    /* template unsigned int(16) samplesize = 16; */
    ase->samplesize = get_short_from_buf(p);
    p += 2;
    /* unsigned int(16) pre_defined = 0; */
    /* const unsigned int(16) reserved = 0 ; */
    p += 4;
    /* template unsigned int(32) samplerate = { default samplerate of media}<<16; */
    ase->samplerate = get_short_from_buf(p);
    p += 4;
    printf("\t\t\t\t\t\t");
    printf("ase->channelcount= [%d]\n\t\t\t\t\t\t"
           "ase->samplesize = [%d]\n\t\t\t\t\t\t"
           "ase->samplerate = [%d]\n",
           ase->channelcount,
           ase->samplesize,
           ase->samplerate);


    int context_size = size - (p - buf);
    int tmp_size = 0;
    unsigned char tmpbuf[5];
    uint32_t tag_name = 0;

    while (context_size > 0) {
        tmp_size = get_size_from_buf(p);
        p += 4;
        get_tag_from_buf(p, tmpbuf);
        p += 4;
        tag_name = get_int_from_buf(tmpbuf);
        printf("\t\t\t\t\t\t");
        switch (tag_name) {
            case MKTAG('e','s','d','s'):
                parse_esds(p, tmp_size - 8);
                break;
            case MKTAG('s','i','n','f'):
                parse_sinf(p, tmp_size - 8);
                break;
            case MKTAG('m','p','4','a'):
                parse_mp4a(p, tmp_size - 8);
                break;
            default:
                printf("unkown vide enca, [%s]\n", tmpbuf);
                break;
        }
        p += tmp_size;
        p -= 8;
        context_size -= tmp_size;
    }



    printf("\t\t\t\t\t\tenca size == [%ld]\n", p - buf);
    return p - buf;
}


int parse_stsd(int fd, int stsd_size, int handler_type)
{
    printf("in stsd size = [%d]\n", stsd_size);

    struct SampleDescriptionBox *stsd = NULL;
    struct VisualSampleEntry *vse = NULL;
    struct AudioSampleEntry *ase = NULL;
    struct HintSampleEntry *hse = NULL;
    int size = stsd_size - 4; /* aligned(8) class SampleDescriptionBox extends FullBox(‘stsd’, version, 0)  */
    char ext_version_flags[5];
    unsigned char codingname[5];
    int codingname_size = 0;
    unsigned int tag_name = 0;
    unsigned char *buf = malloc(stsd_size);
    unsigned char *p = NULL;
    int i = 0;

    if (!buf)
        return -ENOMEM;

    stsd = malloc(sizeof(struct SampleDescriptionBox));
    if (!stsd)
        return -ENOMEM;
    memset(stsd, 0, sizeof(struct SampleDescriptionBox));

    /* read version and flags of the extends */
    read(fd, ext_version_flags, 4);
    /* read stsd context */
    read(fd, buf, size);
    p = buf;

    /* unsigned int(32) entry_count; */
    stsd->entry_count = get_int_from_buf(p);
    p += 4;
    printf("stsd->entry_count = [%d]\n", stsd->entry_count);

    for (i = 0; i < stsd->entry_count; i++) {
        printf("\t\t\t\t\t");
        switch (handler_type) {
            case MKTAG('v','i','d','e'):
                stsd->vse = malloc(sizeof(struct VisualSampleEntry));
                vse = stsd->vse;
                if (!vse) {
                    printf("alloc stsd VisualSampleEntry error\n");
                    free(stsd);
                    free(buf);
                    return -ENOMEM;
                }

                codingname_size = get_size_from_buf(p);
                p += 4;
                get_tag_from_buf(p, codingname);
                p += 4;
                tag_name = get_int_from_buf(codingname);
                switch (tag_name) {
                    case MKTAG('e','n','c','v'):
                        parse_encv(p, codingname_size - 8, vse, tag_name);
                        break;
                    case MKTAG('a','v','c','1'):
                        parse_encv(p, codingname_size - 8, vse, tag_name);
                        break;
                    default:
                        printf("unkown vide, [%s]\n", codingname);
                        break;
                }
                break;
            case MKTAG('s','o','u','n'):
                stsd->ase = malloc(sizeof(struct AudioSampleEntry));
                ase = stsd->ase;
                if (!ase) {
                    printf("alloc stsd AudioSampleEntry error\n");
                    free(stsd);
                    free(buf);
                    return -ENOMEM;
                }

                codingname_size = get_size_from_buf(p);
                p += 4;
                get_tag_from_buf(p, codingname);
                p += 4;
                tag_name = get_int_from_buf(codingname);
                switch (tag_name) {
                    case MKTAG('e','n','c','a'):
                        parse_enca(p, codingname_size - 8, ase);
                        break;
                    case MKTAG('e','s','d','s'):
                        parse_esds(p, codingname_size - 8);
                        break;
                    case MKTAG('m','p','4','a'):
                        parse_mp4a(p, codingname_size - 8);
                        break;
                    default:
                        printf("unkown soun codingname = [%s] [%x %x]\n", codingname, codingname[0], codingname[1]);
                        break;
                }

                break;
            case MKTAG('h','i','n','t'):
                stsd->hse = malloc(sizeof(struct HintSampleEntry));
                hse = stsd->hse;
                if (!hse) {
                    printf("alloc stsd HintSampleEntry error\n");
                    free(stsd);
                    free(buf);
                    return -ENOMEM;
                }
                break;
            case MKTAG('m','e','t','a'):
                printf("stsd meta\n");
                break;
            case MKTAG('e','s','d','s'):
                printf("stsd esds\n");
                //                parse_esds(p, codingname_size - 8);
                break;
            default:
                printf("unkown stsd handler_type\n");
                break;
        }
        p += codingname_size - 8;
    }


    if (p - buf != size)
        return -EINVAL;


    return 0;
}

int parse_stts(int fd, int stts_size)
{
    printf("in stts\n");
    struct TimeToSampleBox *stts;
    int size = stts_size - 4; /* aligned(8) class TimeToSampleBox extends FullBox(‘stts’, version, 0)  */
    char ext_version_flags[5];
    unsigned char *buf = malloc(stts_size);
    unsigned char *p = buf;
    int i = 0;
    if (!buf)
        return -ENOMEM;

    stts = malloc(sizeof(struct TimeToSampleBox));
    /* read version and flags of the extends */
    read(fd, ext_version_flags, 4);
    read(fd, buf, size);
    stts->entry_count = get_int_from_buf(p);
    p += 4;
    for (i = 0; i < stts->entry_count; i++) {
        stts->sample_count = get_int_from_buf(p);
        p += 4;
        stts->sample_delta = get_int_from_buf(p);
        p += 4;
    }

    printf("\t\t\t\t\tstts->entry_count = [%d]\n", stts->entry_count);
    for (i = 0; i < stts->entry_count; i++) {
//        printf("\t\t\t\t\tstts->sample_count = [%d] stts->sample_delta = [%d]\n",
//               stts->sample_count, stts->sample_delta);
    }
    printf("\n");

    return 0;
}

int parse_ctts(int fd, int ctts_size)
{
    printf("in ctts\n");
    struct CompositionOffsetBox *ctts;
    int size = ctts_size - 4; /* aligned(8) class CompositionOffsetBox extends FullBox(‘ctts’, version, 0)  */
    char ext_version_flags[5];
    unsigned char *buf = malloc(ctts_size);
    unsigned char *p = buf;
    int i = 0;
    if (!buf)
        return -ENOMEM;

    ctts = malloc(sizeof(struct CompositionOffsetBox));
    /* read version and flags of the extends */
    read(fd, ext_version_flags, 4);
    read(fd, buf, size);
    ctts->entry_count = get_int_from_buf(p);
    p += 4;
    if (ext_version_flags[0] == 0) {
        for (i = 0; i < ctts->entry_count; i++) {
            ctts->sample_count = get_int_from_buf(p);
            p += 4;
            ctts->sample_offset = get_int_from_buf(p);
            p += 4;
        }
    } else {
        for (i = 0; i < ctts->entry_count; i++) {
            ctts->sample_count = get_int_from_buf(p);
            p += 4;
            ctts->sample_offset = get_int_from_buf(p);
            p += 4;
        }
    }

    printf("\t\t\t\t\tctts->entry_count = [%d]\n", ctts->entry_count);
    for (i = 0; i < ctts->entry_count; i++) {
//        printf("\t\t\t\t\tctts->sample_count = [%d] ctts->sample_offset = [%d]\n",
//               ctts->sample_count, ctts->sample_offset);
    }
    printf("\n");

    return 0;

}

int parse_cslg(int fd, int size)
{
    printf("in cslg\n");
    unsigned char *buf1 = malloc(size);
    if (!buf1)
        return -ENOMEM;
    read(fd, buf1, size);
    return 0;
}

int parse_stsc(int fd, int stsc_size)
{
    printf("in stsc\n");
    struct SampleToChunkBox *stsc;
    int size = stsc_size - 4; /* aligned(8) class SampleToChunkBox extends FullBox(‘stsc’, version, 0)  */
    char ext_version_flags[5];
    unsigned char *buf = malloc(stsc_size);
    unsigned char *p = buf;
    int i = 0;
    if (!buf)
        return -ENOMEM;

    stsc = malloc(sizeof(struct SampleToChunkBox));
    /* read version and flags of the extends */
    read(fd, ext_version_flags, 4);
    read(fd, buf, size);
    stsc->entry_count = get_int_from_buf(p);
    p += 4;
    for (i = 0; i < stsc->entry_count; i++) {
        stsc->first_chunk = get_int_from_buf(p);
        p += 4;
        stsc->samples_per_chunk = get_int_from_buf(p);
        p += 4;
        stsc->sample_description_index = get_int_from_buf(p);
        p += 4;
    }

    printf("\t\t\t\t\tstsc->entry_count = [%d]\n", stsc->entry_count);
    for (i = 0; i < stsc->entry_count; i++) {
//        printf("\t\t\t\t\tstsc->first_chunk = [%d] stsc->samples_per_chunk = [%d] stsc->sample_description_index = [%d]\n",
//               stsc->first_chunk, stsc->samples_per_chunk, stsc->sample_description_index);
    }
    printf("\n");

    return 0;
}

int parse_stsz(int fd, int stsz_size)
{
    printf("in stsz\n");
    struct SampleSizeBox *stsz;
    int size = stsz_size - 4; /* aligned(8) class SampleSizeBox extends FullBox(‘stsz’, version, 0)  */
    char ext_version_flags[5];
    unsigned char *buf = malloc(stsz_size);
    unsigned char *p = buf;
    int i = 0;
    if (!buf)
        return -ENOMEM;

    stsz = malloc(sizeof(struct SampleSizeBox));
    /* read version and flags of the extends */
    read(fd, ext_version_flags, 4);
    read(fd, buf, size);
    stsz->sample_size = get_int_from_buf(p);
    p += 4;
    stsz->sample_count = get_int_from_buf(p);
    p += 4;
    if (stsz->sample_size == 0) {
        for (i = 0; i < stsz->sample_count; i++) {
            stsz->entry_size = get_int_from_buf(p);
            p += 4;
        }
    }

    printf("\t\t\t\t\tstsz->sample_size = [%d] stsz->sample_count = [%d]\n", stsz->sample_size, stsz->sample_count);
    if (stsz->sample_size == 0) {
        for (i = 0; i < stsz->sample_count; i++) {
           // printf("\t\t\t\t\tstsz->entry_size = [%d]\n", stsz->entry_size);
        }
    }
    printf("\n");

    return 0;
}

int parse_stz2(int fd, int size)
{
    printf("in stz2\n");
    unsigned char *buf1 = malloc(size);
    if (!buf1)
        return -ENOMEM;
    read(fd, buf1, size);
    return 0;
}

int parse_stco(int fd, int stco_size)
{
    printf("in stco\n");
    struct ChunkOffsetBox *stco;
    int size = stco_size - 4; /* aligned(8) class ChunkOffsetBox extends FullBox(‘stco’, version, 0)  */
    char ext_version_flags[5];
    unsigned char *buf = malloc(stco_size);
    unsigned char *p = buf;
    int i = 0;
    if (!buf)
        return -ENOMEM;

    stco = malloc(sizeof(struct ChunkOffsetBox));
    /* read version and flags of the extends */
    read(fd, ext_version_flags, 4);
    read(fd, buf, size);
    stco->entry_count = get_int_from_buf(p);
    p += 4;
    for (i = 0; i < stco->entry_count; i++) {
        stco->chunk_offset = get_int_from_buf(p);
        p += 4;
    }

    printf("\t\t\t\t\tstco->entry_count = [%d]\n", stco->entry_count);
    for (i = 0; i < stco->entry_count; i++) {
    //    printf("\t\t\t\t\tstco->chunk_offset = [%d]\n", stco->chunk_offset);
    }
    printf("\n");

    return 0;
}

int parse_co64(int fd, int size)
{
    printf("in co64\n");
    unsigned char *buf1 = malloc(size);
    if (!buf1)
        return -ENOMEM;
    read(fd, buf1, size);
    return 0;
}

int parse_stss(int fd, int stss_size)
{
    printf("in stss\n");
    struct SyncSampleBox *stss;
    int size = stss_size - 4; /* aligned(8) class SyncSampleBox extends FullBox(‘stss’, version, 0)  */
    char ext_version_flags[5];
    unsigned char *buf = malloc(stss_size);
    unsigned char *p = buf;
    int i = 0;
    if (!buf)
        return -ENOMEM;

    stss = malloc(sizeof(struct SyncSampleBox));
    /* read version and flags of the extends */
    read(fd, ext_version_flags, 4);
    read(fd, buf, size);
    stss->entry_count = get_int_from_buf(p);
    p += 4;
    for (i = 0; i < stss->entry_count; i++) {
        stss->sample_number = get_int_from_buf(p);
        p += 4;
    }

    printf("\t\t\t\t\tstss->entry_count = [%d]\n", stss->entry_count);
    for (i = 0; i < stss->entry_count; i++) {
        printf("\t\t\t\t\tstss->sample_number = [%d]\n", stss->sample_number);
    }
    printf("\n");

    return 0;
}

int parse_stsh(int fd, int size)
{
    printf("in stsh\n");
    unsigned char *buf1 = malloc(size);
    if (!buf1)
        return -ENOMEM;
    read(fd, buf1, size);
    return 0;
}

int parse_padb(int fd, int size)
{
    printf("in padb\n");
    unsigned char *buf1 = malloc(size);
    if (!buf1)
        return -ENOMEM;
    read(fd, buf1, size);
    return 0;
}

int parse_stdp(int fd, int size)
{
    printf("in stdp\n");
    unsigned char *buf1 = malloc(size);
    if (!buf1)
        return -ENOMEM;
    read(fd, buf1, size);
    return 0;
}

int parse_sdtp(int fd, int size)
{
    printf("in sdtp\n");
    unsigned char *buf1 = malloc(size);
    if (!buf1)
        return -ENOMEM;
    read(fd, buf1, size);
    return 0;
}

int parse_sbgp(int fd, int size)
{
    printf("in sbgp\n");
    unsigned char *buf1 = malloc(size);
    if (!buf1)
        return -ENOMEM;
    read(fd, buf1, size);
    return 0;
}

int parse_sgpd(int fd, int size)
{
    printf("in sgpd\n");
    unsigned char *buf1 = malloc(size);
    if (!buf1)
        return -ENOMEM;
    read(fd, buf1, size);
    return 0;
}

int parse_subs(int fd, int size)
{
    printf("in subs\n");
    unsigned char *buf1 = malloc(size);
    if (!buf1)
        return -ENOMEM;
    read(fd, buf1, size);
    return 0;
}

int parse_saiz(int fd, int saiz_size)
{
    printf("in saiz\n");
    struct SampleAuxiliaryInformationSizesBox *saiz;;
    int size = saiz_size - 4;  /* aligned(8) class SampleAuxiliaryInformationSizesBox extends FullBox(‘saiz’, version = 0, flags) */
    char ext_version_flags[5];
    unsigned char *buf = malloc(saiz_size);
    unsigned char *p = buf;
    int i = 0;
    if (!buf)
        return -ENOMEM;

    saiz = malloc(sizeof(struct SampleAuxiliaryInformationSizesBox));
    /* read version and flags of the extends */
    read(fd, ext_version_flags, 4);
    read(fd, buf, size);

    if (ext_version_flags[1] == 0 && ext_version_flags[2] == 0 && ext_version_flags[3] == 1) {
        saiz->aux_info_type = get_int_from_buf(p);
        p += 4;
        saiz->aux_info_type_parameter = get_int_from_buf(p);
        p += 4;
    }

    saiz->default_sample_info_size = *p;
    p++;

    saiz->sample_count = get_int_from_buf(p);
    p += 4;

    printf("\t\t\t\t\tsaiz->default_sample_info_size = [%d] saiz->sample_count = [%d]\n", saiz->default_sample_info_size, saiz->sample_count);
    if (saiz->default_sample_info_size == 0) {
        saiz->sample_info_size = malloc(saiz->sample_count);
        memcpy(saiz->sample_info_size, p, saiz->sample_count);
        for (i = 0; i < saiz->sample_count; i++) {
            printf("\t\t\t\t\tsaiz->sample_info_size = [%d]\n", saiz->sample_info_size[i]);
        }
    }

    return 0;
}

int parse_saio(int fd, int saio_size)
{
    printf("in saio\n");
    struct SampleAuxiliaryInformationOffsetsBox *saio;
    int size = saio_size - 4;  /* aligned(8) class SampleAuxiliaryInformationOffsetsBox extends FullBox(‘saio’, version = 0, flags) */
    char ext_version_flags[5];
    unsigned char *buf = malloc(saio_size);
    unsigned char *p = buf;
    int i = 0;
    if (!buf)
        return -ENOMEM;

    saio = malloc(sizeof(struct SampleAuxiliaryInformationOffsetsBox));
    /* read version and flags of the extends */
    read(fd, ext_version_flags, 4);
    read(fd, buf, size);

    if (ext_version_flags[1] == 0 && ext_version_flags[2] == 0 && ext_version_flags[3] == 1) {
        saio->aux_info_type = get_int_from_buf(p);
        p += 4;
        saio->aux_info_type_parameter = get_int_from_buf(p);
        p += 4;
    }

    saio->entry_count = get_int_from_buf(p);
    p += 4;

    printf("\t\t\t\t\tsaio->entry_count = [%d]\n", saio->entry_count);
    if (ext_version_flags[0] == 0) {
        saio->offset = malloc(saio->entry_count * sizeof(int32_t));
        for(i = 0; i < saio->entry_count; i ++) {
            saio->offset[i] = get_int_from_buf(p);
            p += 4;
        }
        for (i = 0; i < saio->entry_count; i++) {
            printf("\t\t\t\t\tsaio->offset = [0x%08llx]\n", saio->offset[i]);
        }
    } else {
        saio->offset = malloc(saio->entry_count * sizeof(int64_t));
        for(i = 0; i < saio->entry_count; i ++) {
            saio->offset[i] = get_long_from_buf(p);
            p += 8;
        }
        for (i = 0; i < saio->entry_count; i++) {
            printf("\t\t\t\t\tsaio->offset = [0x%08llx]\n", saio->offset[i]);
        }
    }

    return 0;
}

int parse_senc(int fd, int senc_size)
{
    printf("in senc\n");
    struct SampleEncryptionBox *senc;
    int size = senc_size - 4;  /* aligned(8) class SampleEncryptionBox extends FullBox(‘senc’, version = 0, flags) */
    char ext_version_flags[5];
    unsigned char *buf = malloc(senc_size);
    unsigned char *p = buf;
    int i = 0;
    int j = 0;
    if (!buf)
        return -ENOMEM;

    senc = malloc(sizeof(struct SampleEncryptionBox));
    /* read version and flags of the extends */
    read(fd, ext_version_flags, 4);
    read(fd, buf, size);

    senc->sample_count = get_int_from_buf(p);
    p += 4;

    printf("\t\t\tsenc->sample_count = [%2d]\n", senc->sample_count);
    for (i = 0; i < senc->sample_count; i++) {
        senc->sencsamples[i].InitializationVector = get_long_from_buf(p);
        p += 8;
        printf("\t\t\tsenc->sencsamples[%2d].InitializationVector = [0x%16llx]", i, senc->sencsamples[i].InitializationVector);
        if (ext_version_flags[1] == 0 && ext_version_flags[2] == 0 && ext_version_flags[3] == 2) {
            senc->sencsamples[i].subsample_count = get_short_from_buf(p);
            p += 2;
            printf("senc->sencsamples[%2d].subsample_count = [%d]\n", i, senc->sencsamples[i].subsample_count);
            for (j = 0; j < senc->sencsamples[i].subsample_count; j++) {
                senc->sencsamples[i].subsencs[j].BytesOfClearData = get_short_from_buf(p);
                p += 2;
                senc->sencsamples[i].subsencs[j].BytesOfProtectedData = get_int_from_buf(p);
                p += 4;
                printf("\t\t\tsenc->sencsamples[%2d].subsencs[%2d].BytesOfClearData = [0x%04d], "
                       "senc->sencsamples[%2d].subsencs[%2d].BytesOfProtectedData = [0x%08d]\n",
                       i, j,
                       senc->sencsamples[i].subsencs[j].BytesOfClearData,
                       i, j,
                       senc->sencsamples[i].subsencs[j].BytesOfProtectedData);
            }
        }
    }

    return 0;
}


int parse_stbl(int fd, int stbl_size, int handler_type)
{
    int size = stbl_size;
    int tag_size = 0;
    unsigned char buf[5];
    unsigned int tag_name = 0;

    printf("in stbl\n");
    while(size > 0) {
        tag_size = get_size(fd);
        if (tag_size <= 0) {
            break;
        }
        size -= tag_size;
        memset(buf, 0, sizeof(buf));
        get_tag_from_fd(fd, buf);
        tag_name = get_int_from_buf(buf);
        printf("\t\t\t\t");
        switch (tag_name) {
            case MKTAG('s','t','s','d'):
                parse_stsd(fd, tag_size - 8, handler_type);
                break;
            case MKTAG('s','t','t','s'):
                parse_stts(fd, tag_size - 8);
                break;
            case MKTAG('c','t','t','s'):
                parse_ctts(fd, tag_size - 8);
                break;
            case MKTAG('c','s','l','g'):
                parse_cslg(fd, tag_size - 8);
                break;
            case MKTAG('s','t','s','c'):
                parse_stsc(fd, tag_size - 8);
                break;
            case MKTAG('s','t','s','z'):
                parse_stsz(fd, tag_size - 8);
                break;
            case MKTAG('s','t','z','2'):
                parse_stz2(fd, tag_size - 8);
                break;
            case MKTAG('s','t','c','o'):
                parse_stco(fd, tag_size - 8);
                break;
            case MKTAG('c','o','6','4'):
                parse_co64(fd, tag_size - 8);
                break;
            case MKTAG('s','t','s','s'):
                parse_stss(fd, tag_size - 8);
                break;
            case MKTAG('s','t','s','h'):
                parse_stsh(fd, tag_size - 8);
                break;
            case MKTAG('p','a','d','b'):
                parse_padb(fd, tag_size - 8);
                break;
            case MKTAG('s','t','d','p'):
                parse_stdp(fd, tag_size - 8);
                break;
            case MKTAG('s','d','t','p'):
                parse_sdtp(fd, tag_size - 8);
                break;
            case MKTAG('s','b','g','p'):
                parse_sbgp(fd, tag_size - 8);
                break;
            case MKTAG('s','g','p','d'):
                parse_sgpd(fd, tag_size - 8);
                break;
            case MKTAG('s','u','b','s'):
                parse_subs(fd, tag_size - 8);
                break;
            case MKTAG('s','a','i','z'):
                parse_saiz(fd, tag_size - 8);
                break;
            case MKTAG('s','a','i','o'):
                parse_saio(fd, tag_size - 8);
                break;
            default:
                break;
        }
    }

    return 0;
}

int parse_minf(int fd, int minf_size, int handler_type)
{

    int size = minf_size;
    int tag_size = 0;
    unsigned char buf[5];
    unsigned int tag_name = 0;

    printf("in minf\n");
    while(size > 0) {
        tag_size = get_size(fd);
        if (tag_size <= 0) {
            break;
        }
        size -= tag_size;
        memset(buf, 0, sizeof(buf));
        get_tag_from_fd(fd, buf);
        tag_name = get_int_from_buf(buf);
        printf("\t\t\t");
        switch (tag_name) {
            case MKTAG('v','m','h','d'):
                parse_vmhd(fd, tag_size - 8);
                break;
            case MKTAG('s','m','h','d'):
                parse_smhd(fd, tag_size - 8);
                break;
            case MKTAG('h','m','h','d'):
                parse_hmhd(fd, tag_size - 8);
                break;
            case MKTAG('n','m','h','d'):
                parse_nmhd(fd, tag_size - 8);
                break;
            case MKTAG('d','i','n','f'):
                parse_dinf(fd, tag_size - 8);
                break;
            case MKTAG('s','t','b','l'):
                parse_stbl(fd, tag_size - 8, handler_type);
                break;
            default:
                break;
        }
    }

    return 0;
}


int parse_mdia(int fd, int mdia_size)
{

    int size = mdia_size;
    int tag_size = 0;
    int32_t handler_type = 0;
    unsigned char buf[5];
    unsigned int tag_name = 0;

    printf("in mdia\n");
    while(size > 0) {
        tag_size = get_size(fd);
        if (tag_size <= 0) {
            break;
        }
        size -= tag_size;
        memset(buf, 0, sizeof(buf));
        get_tag_from_fd(fd, buf);
        tag_name = get_int_from_buf(buf);
        printf("\t\t");
        switch (tag_name) {
            case MKTAG('m','d','h','d'):
                parse_mdhd(fd, tag_size - 8);
                break;
            case MKTAG('h','d','l','r'):
                parse_hdlr(fd, tag_size - 8, &handler_type);
                break;
            case MKTAG('m','i','n','f'):
                parse_minf(fd, tag_size - 8, handler_type);
                break;
            default:
                break;
        }
    }

    return 0;
}

int parse_meta(int fd, int meta_size)
{
    int size = meta_size;
    int tag_size = 0;
    int32_t handler_type = 0;
    unsigned char buf[5];
    unsigned int tag_name = 0;
    char ext_version_flags[5];

    printf("in meta size = %d\n", meta_size);


    read(fd, ext_version_flags, 4);
    while(size > 0) {
        tag_size = get_size(fd);
        if (tag_size <= 0) {
            break;
        }
        size -= tag_size;
        memset(buf, 0, sizeof(buf));
        get_tag_from_fd(fd, buf);
        tag_name = get_int_from_buf(buf);
        printf("\t");
        switch (tag_name) {
            case MKTAG('I','D','3','2'):
                parse_ID32(fd, tag_size - 8);
                break;
            case MKTAG('h','d','l','r'):
                parse_hdlr(fd, tag_size - 8, &handler_type);
                break;
            case MKTAG('d','i','n','f'):
                parse_dinf(fd, tag_size - 8);
                break;
            case MKTAG('t','r','a','k'):
                parse_trak(fd, tag_size - 8);
                break;
            default:
                printf("buf = [%s]\n", buf);
                break;
        }
    }

    return 0;
}

/* This is a container box for a single track of a presentation. A presentation consists of one or more tracks.
 * Each track is independent of the other tracks in the presentation and carries its own temporal and spatial information.
 * Each track will contain its associated Media Box. Tracks are used for two purposes:
 *
 *  (a) to contain media data (media tracks) and
 *  (b) to contain packetization information for streaming protocols (hint tracks).
 *
 *  There shall be at least one media track within an ISO file,
 *  and all the media tracks that contributed to the hint tracks shall remain in the file,
 *  even if the media data within them is not referenced by the hint tracks;
 *  after deleting all hint tracks, the entire un-hinted presentation shall remain.
 * */
int parse_trak(int fd, int trak_size)
{
    int size = trak_size;
    int tag_size = 0;
    unsigned char buf[5];
    unsigned int tag_name = 0;

    printf("in trak\n");
    while(size > 0) {
        tag_size = get_size(fd);
        if (tag_size <= 0) {
            break;
        }
        size -= tag_size;
        memset(buf, 0, sizeof(buf));
        get_tag_from_fd(fd, buf);
        tag_name = get_int_from_buf(buf);
        printf("\t");
        switch (tag_name) {
            case MKTAG('t','k','h','d'):
                parse_tkhd(fd, tag_size - 8);
                break;
            case MKTAG('t','r','e','f'):
                parse_tref(fd, tag_size - 8);
                break;
            case MKTAG('t','r','g','r'):
                parse_trgr(fd, tag_size - 8);
                break;
            case MKTAG('e','d','t','s'):
                parse_edts(fd, tag_size - 8);
                break;
            case MKTAG('u','d','t','a'):
                parse_udta(fd, tag_size - 8);
                break;
            case MKTAG('m','d','i','a'):
                parse_mdia(fd, tag_size - 8);
                break;
            default:
                break;
        }
    }

    return 0;
}
int parse_mehd(int fd, int size)
{
    printf("in mehd\n");
    unsigned char *buf1 = malloc(size);
    if (!buf1)
        return -ENOMEM;
    read(fd, buf1, size);
    return 0;
}

int parse_trex(int fd, int size)
{
    printf("in trex\n");
    unsigned char *buf1 = malloc(size);
    if (!buf1)
        return -ENOMEM;
    read(fd, buf1, size);
    return 0;
}

int parse_leva(int fd, int size)
{
    printf("in leva\n");
    unsigned char *buf1 = malloc(size);
    if (!buf1)
        return -ENOMEM;
    read(fd, buf1, size);
    return 0;
}

int parse_mvex(int fd, int mvex_size)
{
    int size = mvex_size;
    int tag_size = 0;
    unsigned char buf[5];
    unsigned int tag_name = 0;

    printf("in mvex\n");
    while(size > 0) {
        tag_size = get_size(fd);
        if (tag_size <= 0) {
            break;
        }
        size -= tag_size;
        memset(buf, 0, sizeof(buf));
        get_tag_from_fd(fd, buf);
        tag_name = get_int_from_buf(buf);
        printf("\t");
        switch (tag_name) {
            case MKTAG('m','e','h','d'):
                parse_mehd(fd, tag_size - 8);
                break;
            case MKTAG('t','r','e','x'):
                parse_trex(fd, tag_size - 8);
                break;
            case MKTAG('l','e','v','a'):
                parse_leva(fd, tag_size - 8);
                break;
            default:
                break;
        }
    }

    return 0;
}

void parse_moov(int fd, int moov_size)
{
    int size = moov_size;
    int tag_size = 0;
    unsigned char buf[5];
    unsigned int tag_name = 0;

    printf("in moov\n");
    while(size > 0) {
        tag_size = get_size(fd);
        if (tag_size <= 0) {
            break;
        }
        size -= tag_size;
        memset(buf, 0, sizeof(buf));
        get_tag_from_fd(fd, buf);
        tag_name = get_int_from_buf(buf);
        switch (tag_name) {
            case MKTAG('m','v','h','d'):
                parse_mvhd(fd, tag_size - 8);
                break;
            case MKTAG('m','e','t','a'):
                parse_meta(fd, tag_size - 8);
                break;
            case MKTAG('t','r','a','k'):
                parse_trak(fd, tag_size - 8);
                break;
            case MKTAG('m','v','e','x'):
                parse_mvex(fd, tag_size - 8);
                break;
            default:
                break;
        }
    }

    return;
}

int parse_ftyp(int fd, int size)
{
    int ret = 0;
    int tag_size = size;
    int remain_size = 0;
    int tag = 0;
    int i = 0;
    unsigned char *buf = malloc(sizeof(unsigned char) * size);
    unsigned char *p = buf;
    struct filetypebox *ftyp = NULL;

    fprintf(stderr, "in ftyp\n");
    ftyp = malloc(sizeof(struct filetypebox));
    if (!ftyp) {
        fprintf(stderr, "alloc ftyp error\n");
        return -ENOMEM;
    }

    tags_root = malloc(sizeof(struct tag_context));
    if (!tags_root) {
        fprintf(stderr, "alloc tags root failed\n");
        return -ENOMEM;
    }

    ret = read(fd, buf, size);

    memset(ftyp, 0, sizeof(struct filetypebox));
    get_tag_from_buf(p, ftyp->major_brand);
    p += 4;

    ftyp->minor_version = get_int_from_buf(p);
    p += 4;

    remain_size = tag_size - (p - buf);
    ftyp->compatible_brands = malloc(remain_size + 1);
    memset(ftyp->compatible_brands, 0, remain_size + 1);
    memcpy(ftyp->compatible_brands, p, remain_size);

    fprintf(stderr, "ftyp , major_brand = [%s], minor_version = [%d], compatible_brands = [%s]\n",
            ftyp->major_brand, ftyp->minor_version, ftyp->compatible_brands);

    if (buf) {
        free(buf);
    }

    return 0;
}

int parse_mfhd(int fd, int mfhd_size)
{
    printf("in mfhd\n");
    int size = mfhd_size - 4; /* aligned(8) class MovieFragmentHeaderBox extends FullBox(‘mfhd’, version, 0)  */
    struct MovieFragmentHeaderBox *mfhd;
    char ext_version_flags[5];
    unsigned int tag_name = 0;
    unsigned char *buf = malloc(mfhd_size);
    unsigned char *p = NULL;
    int i = 0;

    if (!buf)
        return -ENOMEM;

    printf("\t\t");
    mfhd = malloc(sizeof(struct MovieFragmentHeaderBox));
    if (!mfhd) {
        printf("Alloc MovieFragmentHeaderBox error\n");
        return -ENOMEM;
    }
    /* read version and flags of the extends */
    read(fd, ext_version_flags, 4);
    /* read mfhd context */
    read(fd, buf, size);
    p = buf;
    mfhd->sequence_number = get_int_from_buf(p);
    p += 4;

    printf("mfhd->sequence_number = [%d]\n", mfhd->sequence_number);
    if (p - buf != size)
        return -EINVAL;

    return 0;
}

int parse_tfhd(int fd, int tfhd_size)
{
    printf("in tfhd\n");
    int size = tfhd_size - 4; /* aligned(8) class TrackFragmentHeaderBox extends FullBox(‘tfhd’, version, 0)  */
    struct TrackFragmentHeaderBox *tfhd;
    char ext_version_flags[5];
    unsigned int tag_name = 0;
    unsigned char *buf = malloc(tfhd_size);
    unsigned char *p = NULL;
    int i = 0;

    if (!buf)
        return -ENOMEM;

    tfhd = malloc(sizeof(struct TrackFragmentHeaderBox));
    if (!tfhd) {
        printf("Alloc TrackFragmentHeaderBox error\n");
        return -ENOMEM;
    }
    /* read version and flags of the extends */
    read(fd, ext_version_flags, 4);
    /* read tfhd context */
    read(fd, buf, size);
    p = buf;
    tfhd->track_ID = get_int_from_buf(p);
    p += 4;
    printf("\t\t\ttfhd->track_ID = [%d]\n", tfhd->track_ID);
    if (ext_version_flags[3] & 0x01) {
        tfhd->base_data_offset = get_long_from_buf(p);
        printf("\t\t\ttfhd->base_data_offset = [%llu]\n", tfhd->base_data_offset);
        p += 8;
    }
    if (ext_version_flags[3] & 0x02) {
        tfhd->sample_description_index = get_int_from_buf(p);
        printf("\t\t\ttfhd->sample_description_index = [%d]\n", tfhd->sample_description_index);
        p += 4;
    }
    if (ext_version_flags[3] & 0x08) {
        tfhd->default_sample_duration = get_int_from_buf(p);
        printf("\t\t\ttfhd->default_sample_duration = [%d]\n", tfhd->default_sample_duration);
        p += 4;
    }
    if (ext_version_flags[3] & 0x10) {
        tfhd->default_sample_size = get_int_from_buf(p);
        printf("\t\t\ttfhd->default_sample_size = [%d]\n", tfhd->default_sample_size);
        p += 4;
    }
    if (ext_version_flags[3] & 0x20) {
        tfhd->default_sample_flags = get_int_from_buf(p);
        printf("\t\t\ttfhd->default_sample_flags = [%d]\n", tfhd->default_sample_flags);
        p += 4;
    }


    if (p - buf != size)
        return -EINVAL;

    return 0;
}

int parse_trun(int fd, int trun_size)
{
    printf("in trun\n");
    struct TrackRunBox *trun;
    int size = trun_size - 4; /* aligned(8) class TrackRunBox extends FullBox(‘trun’, version, 0)  */
    char ext_version_flags[5];
    unsigned char *buf = malloc(trun_size);
    unsigned char *p = buf;
    int i = 0;
    if (!buf)
        return -ENOMEM;

    trun = malloc(sizeof(struct TrackRunBox));
    /* read version and flags of the extends */
    read(fd, ext_version_flags, 4);
    read(fd, buf, size);
    /* unsigned int(32)  sample_count; */
    trun->sample_count = get_int_from_buf(p);
    p += 4;
    printf("\t\t\ttrun->sample_count = [%d]\n", trun->sample_count);
    if (ext_version_flags[3] & 0x01) {
        /* signed int(32) data_offset; */
        trun->data_offset = get_int_from_buf(p);
        p += 4;
        printf("\t\t\ttrun->data_offset = [%d]\n", trun->data_offset);
    }
    if (ext_version_flags[3] & 0x04) {
        /* unsigned int(32)  first_sample_flags */
        trun->first_sample_flags = get_int_from_buf(p);
        p += 4;
        printf("\t\t\ttrun->first_sample_flags = [%d]\n", trun->first_sample_flags);
    }

    for (i = 0; i < trun->sample_count; i++) {
        if (ext_version_flags[2] & 0x01) {
            /* unsigned int(32)  sample_duration; */
            trun->trunss[i].sample_duration = get_int_from_buf(p);
            p += 4;
            printf("\t\t\t\ttrun->trunss[i].sample_duration = [%d]\n", trun->trunss[i].sample_duration);
        }
        if (ext_version_flags[2] & 0x02) {
            /* unsigned int(32)  sample_size; */
            trun->trunss[i].sample_size = get_int_from_buf(p);
            p += 4;
            printf("\t\t\t\ttrun->trunss[i].sample_size = [%d]\n", trun->trunss[i].sample_size);
        }
        if (ext_version_flags[2] & 0x04) {
            /* unsigned int(32)  sample_flags; */
            trun->trunss[i].sample_flags = get_int_from_buf(p);
            p += 4;
            printf("\t\t\t\ttrun->trunss[i].sample_flags = [%d]\n", trun->trunss[i].sample_flags);
        }
        if (ext_version_flags[2] & 0x04) {
            /* if (version == 0)
            *  unsigned int(32) sample_composition_time_offset;
            * else
            *  signed int(32) sample_composition_time_offset;
            * */
            trun->trunss[i].sample_composition_time_offset = get_int_from_buf(p);
            p += 4;
            printf("\t\t\t\ttrun->trunss[i].sample_composition_time_offset = [%d]\n", trun->trunss[i].sample_composition_time_offset);
        }
    }

    printf("\n");

    return 0;
}

int parse_tfdt(int fd, int tfdt_size)
{
    printf("in tfdt\n");
    struct TrackFragmentBaseMediaDecodeTimeBox *tfdt;
    int size = tfdt_size - 4; /* aligned(8) class TrackFragmentBaseMediaDecodeTimeBox extends FullBox(‘tfdt’, version, 0)  */
    char ext_version_flags[5];
    unsigned char *buf = malloc(tfdt_size);
    unsigned char *p = buf;
    int i = 0;
    if (!buf)
        return -ENOMEM;

    tfdt = malloc(sizeof(struct TrackFragmentBaseMediaDecodeTimeBox));
    /* read version and flags of the extends */
    read(fd, ext_version_flags, 4);
    read(fd, buf, size);
    printf("\t\t\t");
    if (ext_version_flags[0] == 0) {
        /* unsigned int(64)  baseMediaDecodeTime; */
        tfdt->baseMediaDecodeTime = get_int_from_buf(p);
        p += 4;
    } else {
        tfdt->baseMediaDecodeTime = get_long_from_buf(p);
        p += 8;
    }
    printf("tfdt->baseMediaDecodeTime = [%llu]\n", tfdt->baseMediaDecodeTime);
    return 0;
}

int parse_traf(int fd, int traf_size)
{
    printf("in traf\n");
    int size = traf_size;
    int tag_size = 0;
    unsigned char buf[5];
    unsigned int tag_name = 0;

    while(size > 0) {
        tag_size = get_size(fd);
        if (tag_size <= 0) {
            break;
        }
        size -= tag_size;
        memset(buf, 0, sizeof(buf));
        get_tag_from_fd(fd, buf);
        tag_name = get_int_from_buf(buf);
        printf("\t\t");
        switch (tag_name) {
            case MKTAG('t','f','h','d'):
                parse_tfhd(fd, tag_size - 8);
                break;
            case MKTAG('t','r','u','n'):
                parse_trun(fd, tag_size - 8);
                break;
            case MKTAG('s','b','g','p'):
                parse_sbgp(fd, tag_size - 8);
                break;
            case MKTAG('s','g','p','d'):
                parse_sgpd(fd, tag_size - 8);
                break;
            case MKTAG('s','u','b','s'):
                parse_subs(fd, tag_size - 8);
                break;
            case MKTAG('s','a','i','z'):
                parse_saiz(fd, tag_size - 8);
                break;
            case MKTAG('s','a','i','o'):
                parse_saio(fd, tag_size - 8);
                break;
            case MKTAG('s','e','n','c'):
                parse_senc(fd, tag_size - 8);
                break;
            case MKTAG('t','f','d','t'):
                parse_tfdt(fd, tag_size - 8);
                break;
            default:
                printf("unknow\n");
                break;
        }
    }

    return 0;
}

void parse_moof(int fd, int moof_size)
{
    int size = moof_size;
    int tag_size = 0;
    unsigned char buf[5];
    unsigned int tag_name = 0;

    printf("in moof\n");
    while(size > 0) {
        tag_size = get_size(fd);
        if (tag_size <= 0) {
            break;
        }
        size -= tag_size;
        memset(buf, 0, sizeof(buf));
        get_tag_from_fd(fd, buf);
        tag_name = get_int_from_buf(buf);
        printf("\t");
        switch (tag_name) {
            case MKTAG('m','f','h','d'):
                parse_mfhd(fd, tag_size - 8);
                break;
            case MKTAG('t','r','a','f'):
                parse_traf(fd, tag_size - 8);
                break;
            default:
                printf("unknow\n");
                break;
        }
    }

    return;
}

void get_root_data(int fd)
{
    unsigned char buf[5];
    unsigned int tag_size = 0;
    unsigned int cur_pos = lseek(fd, 0, SEEK_CUR);
    unsigned int filesize = lseek(fd, 0, SEEK_END);
    cur_pos = lseek(fd, cur_pos, SEEK_SET);
    fprintf(stderr, "filesize = [%d]\n", filesize);

    while (cur_pos < filesize) {
        cur_pos = lseek(fd, 0, SEEK_CUR);
        if (cur_pos >= filesize)
            break;
        tag_size = get_size(fd);
        memset(buf, 0, sizeof(buf));
        get_tag_from_fd(fd, buf);
        if (!strcmp((char *)buf, "moov")) {
            printf("in moov\n");
            parse_moov(fd, tag_size - 8);
        } else if (!strcmp((char *)buf, "mdat")) {
            printf("in mdat %u\n", tag_size - 8);
        } else if (!strcmp((char *)buf, "moof")) {
            printf("in moof\n");
//            parse_moof(fd, tag_size - 8);
        } else if (!strcmp((char *)buf, "ftyp")) {
            printf("in ftyp\n");
            parse_ftyp(fd, tag_size - 8);
        }

        lseek(fd, cur_pos + tag_size, SEEK_SET);
        if (cur_pos + tag_size > filesize) {
            printf("cur_pos = %d + tag_size = %d >= filesize\n", cur_pos, tag_size);
            break;
        }
    }

}

int main(int argc, char *argv[])
{
    int32_t fd = 0;
    int32_t ret = 0;
    uint32_t size = 0;
    uint32_t tag = 0;
    int i = 0;
    char *filename = argv[1];

    if (!filename) {
        fprintf(stderr, "use ./%s <filename>\n", argv[0]);
        exit(1);
    }
    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Cannot open file [%s]\n", filename);
        return -ENOENT;
    }

    get_root_data(fd);


    close(fd);
}
