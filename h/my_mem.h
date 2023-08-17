#ifndef MY_MEM
#define MY_MEM

#include "../lib/hw.h"


struct mem_block{
    size_t size;
    struct mem_block* prev;
    struct mem_block* next;
};

void* __mem_alloc(size_t bytes);

int __mem_free(void* allocated_address);


#endif // MY_MEM