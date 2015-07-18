#include "vqueue.h"
#include "stdio.h"
#include "errno.h"
#include "string.h"

const char * test_str = "Hello, world!";
size_t test_str_length = sizeof(test_str);

int main(void){
    vq_t * vq = vq_init("/vqtest3", 20);
    if(vq == NULL){
        printf("Error creating vqueue: %s\n", strerror(errno));
        return -1;
    }

    int t = vq_write(vq, test_str, test_str_length);

    if(t == test_str_length)
        printf("Return value correct.\n");

    if(vq->length == test_str_length)
        printf("Length set correctly.\n");

    if(memcmp(vq->_buffer_start, test_str, test_str_length) == 0)
        printf("Reading from start succeeded.\n");

    if(memcmp(vq->_buffer_middle, test_str, test_str_length) == 0)
        printf("Reading from middle succeeded.\n");

    printf("Done.\n");

    return 0;
}
