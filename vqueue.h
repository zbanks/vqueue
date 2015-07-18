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

// --  "Zero-copy" write methods --
// vq_zcw_start: Get a buffer to write data into.
// - *vq: pointer to vqueue instance
// - **write_ptr: pointer to buffer pointer, populated with location to write data into
// return value: maximum amount that can be written into *write_ptr, 0 on error.
size_t vq_zcw_start(vq_t * vq, void ** write_ptr);

// vq_zcw_end: Call after writing to buffer from vq_zcw_start.
// - *vq: pointer to vqueue instance
// - length: amount of data written
// If length + vq->length > capacity, then data was silently overwritten
void vq_zcw_end(vq_t * vq, int length);

// vq_write: "Normal" write interface
size_t vq_write(vq_t * vq, const void * data, size_t length);

size_t vq_read(vq_t * vq, void * read_ptr, size_t length);


#endif
