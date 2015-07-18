#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "vqueue.h"

vq_t * vq_init(const char * name, size_t min_capacity) {
    vq_t * vq = calloc(1, sizeof(*vq));
    if(vq == NULL) goto error;

    // Round up capacity to multiple of pagesize
    size_t pagesize = sysconf(_SC_PAGE_SIZE);
    vq->capacity = ((min_capacity + pagesize - 1) & ~(pagesize - 1));

    //TODO: remove me
    shm_unlink(name);

    // Set up shared memory fd
    vq->_fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0644);
    if(vq->_fd < 0) goto error;
    if(ftruncate(vq->_fd, vq->capacity) != 0) goto error;

    // Create double mapping
    vq->_buffer_start = mmap(NULL, vq->capacity, 
            PROT_READ | PROT_WRITE, MAP_SHARED, vq->_fd, 0);
    if(vq->_buffer_start == NULL) goto error;

    vq->_buffer_middle = mmap(vq->_buffer_start + vq->capacity, vq->capacity, 
            PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, vq->_fd, 0);
    if(vq->_buffer_middle == NULL) goto error;

    vq->buffer = vq->_buffer_start;
    vq->length = 0;
    return vq;

error:
    if(vq) {
        //if(vq->_fd > 0) TODO

        if(vq->_buffer_start)
            munmap(vq->_buffer_start, vq->capacity);

        if(vq->_buffer_middle)
            munmap(vq->_buffer_middle, vq->capacity);

        free(vq);
    }
    return NULL;
}

void vq_destroy(vq_t * vq) {
    //vq->fd TODO
    munmap(vq->_buffer_start, vq->capacity);
    munmap(vq->_buffer_middle, vq->capacity);
    free(vq);
}

size_t vq_zcw_start(vq_t * vq, void ** write_ptr) {
    // Calculate where to start writing
    unsigned char * buffer_end = vq->buffer + vq->length;
    if((void *) buffer_end >= vq->_buffer_middle){
        buffer_end -= vq->capacity;
    }

    if(write_ptr == NULL) return 0;
    *write_ptr = buffer_end;

    // Return the amount of free space
    return vq->capacity - vq->length;
}

void vq_zcw_end(vq_t * vq, int length) {
    vq->buffer += length;
    if((void *) vq->buffer >= vq->_buffer_middle){
        vq->buffer -= vq->capacity;
    }

    // If vq->length is now > vq->capacity, data was overwritten
    // But there's nothing we can do about it now.
    vq->length += length;
}

size_t vq_write(vq_t * vq, const void * data, size_t length) {
    void * write_ptr;
    size_t size = vq_zcw_start(vq, &write_ptr);

    if(size == 0)
        return 0;

    if(length < size)
        size = length;

    memmove(write_ptr, data, size);
    vq_zcw_end(vq, size);
    
    return size;
}

size_t vq_read(vq_t * vq, void * read_ptr, size_t length){
    if(length > vq->length)
        length = vq->length;

    if(read_ptr != NULL){
        memmove(read_ptr, vq->buffer, length);
    }

    vq->length -= length;
    vq->buffer += length;

    if((void *) vq->buffer >= vq->_buffer_middle){
        vq->buffer -= vq->capacity;
    }

    return length;
}
