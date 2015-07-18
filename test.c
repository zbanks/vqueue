#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

int main(void){
    size_t size = getpagesize();

    int fd = shm_open("/shmtest", O_RDWR | O_CREAT, 0644);
    if(fd < 0) return;
    if (ftruncate(fd, size) == -1) return;

    char * start = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(start == NULL) return;
    char * middle = mmap(start + size, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0);
    if(middle == NULL) return;

    printf("setup complete start=%p middle=%p\n", start, middle);

    memcpy(start, "6789", 5);
    memcpy(start + size - 5, "12345", 5);

    printf("str: %s\n", start + size - 5);

    return;
}
