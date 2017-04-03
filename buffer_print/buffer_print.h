#ifndef __BUFFER_PRINT_H__
#define __BUFFER_PRINT_H__

#define BPRINT_SIZE_UNLIMITED  ((unsigned)-1)
#define BPRINT_SIZE_AUTOMATIC  1
#define BPRINT_SIZE_COUNT_ONLY 0
#define MIN(a,b) ((a) > (b) ? (b) : (a))
#define bprint_room(buf)   ((buf)->size - MIN((buf)->len, (buf)->size))
#define bprint_is_allocated(buf) ((buf)->str != (buf)->reserved_internal_buffer)
#define PAD_STRUCTURE(name, size, ...) \
    struct pad_helper_##name { __VA_ARGS__ }; \
    typedef struct name { \
        __VA_ARGS__ \
        char reserved_padding[size - sizeof(struct pad_helper_##name)]; \
    } name;

PAD_STRUCTURE(BPrint, 1024,
    char *str;         /**< string so far */
    unsigned len;      /**< length so far */
    unsigned size;     /**< allocated memory */
    unsigned size_max; /**< maximum allocated memory */
    char reserved_internal_buffer[1];
)


void bprint_init(BPrint *buf, unsigned size_init, unsigned size_max);
void bprint_append_data(BPrint *buf, const char *data, unsigned size);
int bprint_finalize(BPrint *buf, char **ret_str);
int bprint_is_complete(const BPrint *buf);

#endif
