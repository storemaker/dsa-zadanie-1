#include <stdio.h>

#define MAX_SIZE 20

void *p_memory_start;

void memory_init(void *ptr, unsigned int size)
{
    if (size > MAX_SIZE)
        return;
    
    // initialize array with char values
    for (int i = 0; i < size; i++)
        *((char*)ptr+i) = -1;
    
    // set global pointer to memory array
    p_memory_start = ptr;
    
    // set main header with max allocateable bytes
    *((int*)ptr) = size - sizeof(int) - 1;
    *((char*)(ptr + size - 1)) = 'E';
}

void memory_set_allocated_bytes(unsigned int size, int offset)
{
    void *p_memory_temp = p_memory_start + offset;
    *(int*)p_memory_temp = size;
    p_memory_temp += sizeof(int);
    
    for (int i = 0; i < size; i++)
        *((char*)(p_memory_temp + i)) = -2;
}

void *memory_alloc(unsigned int size, int debug)
{
    void *p_memory_temp = p_memory_start + sizeof(int), *p_memory_header = p_memory_start, *block_pointer;
    int found_size = 0, counter = 1, max_allocateable_size = *(int*)p_memory_start;
    
    // want to allocate more than max allocateable bytes
    // if (max_allocateable_size < size + sizeof(int))
    //     return NULL;
    
    // find the free block which is big enough
    while (found_size != size && counter < size)
    {
        counter++;
        
        if (debug)
            printf("%d %p\n", *(int*)(p_memory_temp), p_memory_temp);
        
        // found unallocated block but i don't know his size
        if (*(int*)(p_memory_temp) == -1) {
            found_size++;
        }
        // found header
        else if (*(int*)(p_memory_temp) != 69) {
            found_size = 0;
            int current_header = *((int*)p_memory_temp);
            //if(debug)
            p_memory_temp += current_header + sizeof(int);
            continue;
        } else if (*(char*)(p_memory_temp) == 69) {
            break;
        }
        p_memory_temp++;
    }
    
    // did not found block large enough
    if (found_size < size)
        return NULL;
    
    *((int*)p_memory_header) -= size + sizeof(int);
    
    // traverse back to start of first allocateable block
    p_memory_temp -= found_size;
    
    // set size of new allocated block
    *(int*)p_memory_temp = size;
    
    // move pointer forward by size of header
    p_memory_temp += sizeof(int);
    block_pointer = p_memory_temp;
    
    for (int i = 0; i < size; i++)
        *((char*)(p_memory_temp + i)) = -2;
    
    return block_pointer;
}

int main(int argc, const char * argv[]) {
    char memory_array[MAX_SIZE];
    
    memory_init(memory_array, MAX_SIZE);
    void *PICOVINA = memory_alloc(2, 0);
    //memory_set_allocated_bytes(10, 18);
    void *PICOVINA2 = memory_alloc(3, 0);
    void *PICOVINA3 = memory_alloc(4, 1);
    
    return 0;
}


