#ifndef __VQUEUE_H__
#define __VQUEUE_H__

#include <stddef.h>

typedef struct {
    size_t capacity;
    size_t length;
    unsigned char * buffer;

    // "Private" members
    int _fd;
    void * _buffer_start;
    void * _buffer_middle;
} vq_t;

vq_t * vq_init(const char * name, size_t min_capacity);
void vq_destroy(vq_t * vq);

size_t vq_zcw_start(vq_t * vq, void ** write_ptr);
void vq_zcw_end(vq_t * vq, int length);

size_t vq_write(vq_t * vq, const void * data, size_t length);

static inline size_t vq_read(vq_t * vq, void ** read_ptr){
    if(read_ptr == NULL) return 0;

    *read_ptr = vq->buffer;
    return vq->length;
}


#endif
