#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <limits.h>
#include "buffer_print.h"

static int bprint_alloc(BPrint *buf, unsigned room)
{
    char *old, *new;
    unsigned minsz, newsz;

    if (buf->size == buf->size_max) {
        return -EIO;
    }
    if (!bprint_is_complete(buf)) {
        return -EINVAL;
    }

    minsz = buf->len + 1 + MIN(UINT_MAX - buf->len - 1, room);
    newsz = buf->size > buf->size_max / 2 ? buf->size_max : buf->size * 2;

    if (newsz < minsz) {
        newsz = MIN(buf->size_max, minsz);
    }

    old = bprint_is_allocated(buf) ? buf->str : NULL;
    new = realloc(old, newsz);
    if (!new) {
        return -ENOMEM;
    }

    if (!old) {
        memcpy(new, buf->str, buf->len + 1);
    }
    buf->str = new;
    buf->size = newsz;

    return 0;
}

static void bprint_grow(BPrint *buf, unsigned extra_len)
{
    extra_len = MIN(extra_len, UINT_MAX - 5 - buf->len);
    buf->len += extra_len;
    if (buf->size) {
        buf->str[MIN(buf->len, buf->size - 1)] = 0;
    }
}

void bprint_init(BPrint *buf, unsigned size_init, unsigned size_max)
{
    unsigned size_auto = (char *)buf + sizeof(*buf) - buf->reserved_internal_buffer;
    if (size_max == 1) {
        size_max = size_auto;
    }

    buf->str = buf->reserved_internal_buffer;
    buf->len = 0;
    buf->size = MIN(size_auto, size_max);
    buf->size_max = size_max;
    *buf->str = 0;
    if (size_init > buf->size) {
        bprint_alloc(buf, size_init - 1);
    }
}

void bprint_append_data(BPrint *buf, const char *data, unsigned size)
{
    unsigned room, real_n;

    while (1) {
        room = bprint_room(buf);
        if (size < room) {
            break;
        }
        if (bprint_alloc(buf, size)) {
            break;
        }
    }
    if (room) {
        real_n = MIN(size, room - 1);
        memcpy(buf->str + buf->len, data, real_n);
    }
    bprint_grow(buf, size);
}

int bprint_finalize(BPrint *buf, char **ret_str)
{
    unsigned real_size = MIN(buf->len + 1, buf->size);
    char *str;
    int ret = 0;

    if (ret_str) {
        if (bprint_is_allocated(buf)) {
            str = realloc(buf->str, real_size);
            if (!str)
                str = buf->str;
            buf->str = NULL;
        } else {
            str = malloc(real_size);
            if (str)
                memcpy(str, buf->str, real_size);
            else
                ret = -ENOMEM;
        }
        *ret_str = str;
    } else {
        if (bprint_is_allocated(buf))
            free(&buf->str);
    }
    buf->size = real_size;
    return ret;
}

int bprint_is_complete(const BPrint *buf)
{
    return buf->len < buf->size;
}


