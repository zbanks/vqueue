#include "vqueue.h"
#include "stdio.h"
#include "errno.h"
#include "string.h"
#include "assert.h"

const char * test_str = "Hello, world!";
size_t test_str_length = sizeof(test_str);

static void example() {
    vq_t * vq = vq_init("/vqexample", 4096);
    int r = 0;
    (void) (r);

    // Write 2000 bytes
    char buffer[2000] = "Test data! Test data!";
    r = vq_write(vq, buffer, 2000);
    assert(r == 2000);
    
    // Read & verify data
    char buffer_check[2000];
    r = vq_read(vq, buffer_check, 2000);
    assert(r == 2000);
    assert(memcmp(buffer_check, buffer, 2000) == 0);

    for(int i = 0; i < 100; i++){
        // Write more data
        r = vq_write(vq, buffer, 2000);
        assert(r == 2000);
        r = vq_write(vq, buffer, 2000);
        assert(r == 2000);

        // Read & verify data, spanning the gap
        r = vq_read(vq, buffer_check, 2000);
        assert(r == 2000);
        assert(memcmp(buffer_check, buffer, 2000) == 0);

        r = vq_read(vq, buffer_check, 2000);
        assert(r == 2000);
        assert(memcmp(buffer_check, buffer, 2000) == 0);
    }

    vq_destroy(vq);

    printf("Successfully ran example!\n");
}

int main(void){
    vq_t * vq = vq_init("/vqtest", 20);
    if(vq == NULL){
        printf("Error creating vqueue: %s\n", strerror(errno));
        return -1;
    }

    size_t t = vq_write(vq, test_str, test_str_length);

    if(t == test_str_length)
        printf("Return value correct.\n");

    if(vq->length == test_str_length)
        printf("Length set correctly.\n");

    if(memcmp(vq->_buffer_start, test_str, test_str_length) == 0)
        printf("Reading from start succeeded.\n");

    if(memcmp(vq->_buffer_middle, test_str, test_str_length) == 0)
        printf("Reading from middle succeeded.\n");

    vq_destroy(vq);

    example();

    return 0;
}
