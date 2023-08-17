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

#define NULL 0


static struct mem_block* free_mem_head = NULL;
static struct mem_block* used_mem_head = NULL;
static int beginning = 0;

void free_insert(struct mem_block* block);
void free_remove(struct mem_block* block);

void used_insert(struct mem_block* block);
void used_remove(struct mem_block* block);


void
free_insert(struct mem_block* block)
{
    if ((char*)block < (char*)HEAP_START_ADDR || (char*)block >= (char*)HEAP_END_ADDR)
        return;

    if (free_mem_head == NULL)
    {
        free_mem_head = block;
    }
    else if (block < free_mem_head)
    {
        block->prev = NULL;
        block->next = free_mem_head;
        free_mem_head->prev = block;

        free_mem_head = block;
    }
    else
    {
        struct mem_block* prev_tmp = free_mem_head;
        struct mem_block* tmp      = free_mem_head->next;

        while (tmp != NULL && (char*)tmp < (char*)block)
        {
            prev_tmp = tmp;
            tmp = tmp->next;
        }

        prev_tmp->next = block;
        block->prev = prev_tmp;
        block->next = tmp;

        if (tmp != NULL)
            tmp->prev = block;
    }
}


void
free_remove(struct mem_block* block)
{
    if (free_mem_head == NULL || block == NULL)
        return;

    if (block == free_mem_head)
    {
        free_mem_head = free_mem_head->next;

        if (free_mem_head != NULL)
            free_mem_head->prev = NULL;
    }
    else
    {
        block->prev->next = block->next;
        if (block->next)
            block->next->prev = block->prev;
    }

    // Unlink
    block->prev = NULL;
    block->next = NULL;
}


void
used_insert(struct mem_block* block)
{
    if ((char*)block < (char*)HEAP_START_ADDR || (char*)block >= (char*)HEAP_END_ADDR)
        return;

    if (used_mem_head == NULL)
    {
        used_mem_head = block;
    }
    else if (block < used_mem_head)
    {
        block->prev = NULL;
        block->next = used_mem_head;
        used_mem_head->prev = block;

        used_mem_head = block;
    }
    else
    {
        struct mem_block* prev_tmp = used_mem_head;
        struct mem_block* tmp      = used_mem_head->next;

        while (tmp != NULL && (char*)tmp < (char*)block)
        {
            prev_tmp = tmp;
            tmp = tmp->next;
        }

        prev_tmp->next = block;
        block->prev = prev_tmp;
        block->next = tmp;

        if (tmp != NULL)
            tmp->prev = block;
    }
}


void
used_remove(struct mem_block* block)
{
    if (used_mem_head == NULL || block == NULL)
        return;

    if (block == used_mem_head)
    {
        used_mem_head = used_mem_head->next;

        if (used_mem_head != NULL)
            used_mem_head->prev = NULL;
    }
    else
    {
        block->prev->next = block->next;
        if (block->next)
            block->next->prev = block->prev;
    }

    // Unlink
    block->prev = NULL;
    block->next = NULL;
}




void*
__mem_alloc(size_t bytes)
{
    // Initialize if necessary(This is done only once)
    if (free_mem_head == NULL && beginning == 0)
    {
        beginning = 1;
        free_mem_head = (struct mem_block*) HEAP_START_ADDR;
        free_mem_head->size = ((uint64)HEAP_END_ADDR - (uint64)HEAP_START_ADDR) - sizeof(struct mem_block);

        free_mem_head->next = NULL;
        free_mem_head->prev = NULL;
    }

    size_t blocks = (bytes % MEM_BLOCK_SIZE > 0) ? (bytes / MEM_BLOCK_SIZE + 1) : (bytes / MEM_BLOCK_SIZE);

    size_t allocate = blocks * MEM_BLOCK_SIZE; // Now this is in bytes

    for (struct mem_block* curr = free_mem_head; curr != NULL; curr = curr->next)
    {
        /* FIRST FIT */
        if (curr->size < allocate)
            continue;
        else if (curr->size == allocate)
        {
            free_remove(curr);
            used_insert(curr);

            return (void*)((char*)curr + sizeof(struct mem_block));
        }
        else if (curr->size > allocate)
        {
            free_remove(curr);

            size_t leftover_allocatable_size = curr->size - allocate; // Leftover
            leftover_allocatable_size -= sizeof(struct mem_block);

            struct mem_block* leftover = (struct mem_block*) ((char*)curr + sizeof(struct mem_block) + allocate);
            leftover->prev = NULL;
            leftover->next = NULL;
            leftover->size = leftover_allocatable_size;

            curr->size = allocate;

            free_insert(leftover);
            used_insert(curr);

            return (void*)((char*)curr + sizeof(struct mem_block));
        }
    }

    // There is not enough space
    return NULL;
}



int
__mem_free(void* allocated_address)
{
    /* Nothing to free */
    if (allocated_address == NULL || used_mem_head == NULL)
        return -1;

    if ((char*)allocated_address < (char*)HEAP_START_ADDR || (char*)allocated_address > (char*)HEAP_END_ADDR)
        return -1;

    // Because we keep the list sorted
    if ((char*)allocated_address < (char*)used_mem_head)
        return -1;

    struct mem_block* block_to_free = (struct mem_block*)((char*)allocated_address - sizeof(struct mem_block));

    used_remove(block_to_free);
    free_insert(block_to_free);

    /* Merge */
    struct mem_block* prev = block_to_free->prev;
    struct mem_block* next = block_to_free->next;

    /* Merge with Left */
    if (prev != NULL && ((char*)prev + sizeof(struct mem_block) + prev->size) >= (char*)block_to_free)
    {
        prev->size += sizeof(struct mem_block) + block_to_free->size;
        prev->next = block_to_free->next;

        if (next != NULL)
            next->prev = prev;

        /* Merge with Right as well */
        if (next != NULL && ((char*)prev + sizeof(struct mem_block) + prev->size) >= (char*)next)
        {
            prev->size += sizeof(struct mem_block) + next->size;
            prev->next = next->next;

            if (next->next != NULL)
                next->next->prev = prev;
        }
    }
        /* Merge only with Right */
    else if (next != NULL && ((char*)block_to_free + sizeof(struct mem_block) + block_to_free->size) >= (char*)next)
    {
        block_to_free->size += sizeof(struct mem_block) + next->size;
        block_to_free->next = next->next;

        if (next->next != NULL)
            next->next->prev = block_to_free;
    }

    return 0;
}