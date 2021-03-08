#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define MAX_SIZE 1000
#define ALLOCATED_WORD 10
#define UNALLOCATED_WORD 20

void *p_memory_start;

void memory_init(void *ptr, unsigned int size)
{
    
    
    // initialize array with char values
    for (int i = 0; i < size; i++)
        *((char*)ptr+i) = UNALLOCATED_WORD;
    
    // set global pointer to memory array
    p_memory_start = ptr;
    
    // set main headers with max allocateable bytes
    *((int*)ptr) = size - 3 * sizeof(int);
    *((int*)(ptr+sizeof(int))) = size - 3 * sizeof(int);
    
    printf("MAX ALLOCATEABLE BYTES: %d\n", *((int*)ptr));
}

void memory_set_allocated_bytes(unsigned int size, int offset)
{
    int max_allocateable_size = *(int*)p_memory_start;
    void *p_memory_temp = p_memory_start + offset, *p_memory_header = p_memory_start;
    *(int*)p_memory_header = max_allocateable_size - size - 2*sizeof(int);
    
    // zapis prvej hlavicky
    *(int*)p_memory_temp = size * -1;
    
    // posunutie sa na payload bloku
    p_memory_temp += sizeof(int);
    
    // zapis payloadu
    for (int i = 0; i < size; i++)
        *((char*)(p_memory_temp + i)) = ALLOCATED_WORD;
    
    // posunutie pointera pre zapis footeru bloku
    p_memory_temp += size;
    
    // zapis velkosti bloku do footera
    *(int*)p_memory_temp = size * -1;
}

void memory_set_unallocated_bytes(unsigned int size, int offset)
{
    #define MAX_SIZE size;
    int max_allocateable_size = *(int*)p_memory_start;
    void *p_memory_temp = p_memory_start + offset, *p_memory_header = p_memory_start;
    
    // zapis prvej hlavicky
    *(int*)p_memory_temp = size;
    
    // posunutie sa na payload bloku
    p_memory_temp += sizeof(int);
    
    // zapis payloadu
    for (int i = 0; i < size; i++)
        *((char*)(p_memory_temp + i)) = UNALLOCATED_WORD;
    
    // posunutie pointera pre zapis footeru bloku
    p_memory_temp += size;
    
    // zapis velkosti bloku do footera
    *(int*)p_memory_temp = size;
}

void *memory_alloc(unsigned int size)
{
    void *p_memory_temp = p_memory_start + sizeof(int), *p_memory_header = p_memory_start, *p_block = NULL, *p_end_pointer = p_memory_start + MAX_SIZE - 1;
    int max_allocateable_size = *(int*)p_memory_start;
    
    //*(int*)p_memory_temp = size;
    
    if (max_allocateable_size < size + 2*sizeof(int))
        return NULL;
    
    while (p_memory_temp < p_end_pointer) {
        
        // nasiel som volny blok
        if (*(int*)p_memory_temp > 0 && 2*sizeof(int) + size <= *(int*)p_memory_temp) {
            int block_size = *(int*)p_memory_temp;
            // situacia kedy mam volny blok, ale za nim je alokovany blok
           /* int next_block_allocated = *(int*)(p_memory_temp + -1*(*(int*)(p_memory_temp)) + 2*sizeof(int));
            if (next_block_allocated < 0)
            {
                //int block_size = (size + (*(int*)p_memory_temp - size - 2*sizeof(int)));
                
                */
                // je tam miesto aj pre dalsi free blok
                if (block_size - size - sizeof(int) >= 2*sizeof(int)) {
                    
                    *(int*)p_memory_header = max_allocateable_size - size - 2*sizeof(int);
                    
                    *(int*)p_memory_temp = size * -1;
                    p_memory_temp += sizeof(int);
                    p_block = p_memory_temp;
                    for (int i = 0; i < size; i++)
                        *((char*)(p_memory_temp + i)) = ALLOCATED_WORD;
                    p_memory_temp += size;
                    *(int*)p_memory_temp = size * -1;
                    
                    // zostavajuci free block
                    int free_block_size = block_size - size - 2*sizeof(int);
                    p_memory_temp += sizeof(int);
                    *(int*)p_memory_temp = free_block_size;
                    p_memory_temp += sizeof(int);
                    for (int i = 0; i < free_block_size; i++)
                        *((char*)(p_memory_temp + i)) = UNALLOCATED_WORD;
                    p_memory_temp += free_block_size;
                    *(int*)p_memory_temp = free_block_size;
                    
                    //return p_block;
                }
                // mam dostatocne miesto na alokaciu, ale nezmesti sa uz ziadna hlavicka
                // za tento blok, ostalo by iba prazdne miesto
                // alokujem uzivatelovi viac pamate ako realne potrebuje
                else {
                    *(int*)p_memory_temp = block_size * -1;
                    p_memory_temp += sizeof(int);
                    p_block = p_memory_temp;
                    for (int i = 0; i < block_size; i++)
                        *((char*)(p_memory_temp + i)) = ALLOCATED_WORD;
                    p_memory_temp += block_size;
                    *(int*)p_memory_temp = block_size * -1;
                    *(int*)p_memory_header = max_allocateable_size - block_size - 2*sizeof(int);
                    //return p_block;
                }
                
                return p_block;
            /*}
            else {
                *(int*)p_memory_temp = size * -1;
                p_memory_temp += sizeof(int);
                p_block = p_memory_temp;
                for (int i = 0; i < size; i++)
                    *((char*)(p_memory_temp + i)) = ALLOCATED_WORD;
                p_memory_temp += size;
                *(int*)p_memory_temp = size * -1;
                *(int*)p_memory_header = max_allocateable_size - size - 2*sizeof(int);
                
                int free_block_size = block_size - size - 2*sizeof(int);
                p_memory_temp += sizeof(int);
                *(int*)p_memory_temp = free_block_size;
                p_memory_temp += sizeof(int);
                for (int i = 0; i < free_block_size; i++)
                    *((char*)(p_memory_temp + i)) = UNALLOCATED_WORD;
                p_memory_temp += free_block_size;
                *(int*)p_memory_temp = free_block_size;
                
                return p_block;
            }*/
        }
        else if (*(int*)p_memory_temp < 0) {
            p_memory_temp += -(*(int*)p_memory_temp) + 2*sizeof(int);
        }
        else if (*(int*)p_memory_temp > 0) {
            p_memory_temp += (*(int*)p_memory_temp) + 2*sizeof(int);
        }
    }
    
    return NULL;
}

int memory_free(void *ptr)
{
    void *p_memory_temp = ptr - sizeof(int),
        *p_memory_previous_footer = NULL,
        *p_memory_previous_header = NULL,
        *p_memory_next_header = NULL,
        *p_memory_next_footer = NULL;
    
    int previous_allocated = 0, next_allocated = 0, total_size = 0;
    
    if (p_memory_temp - 2*sizeof(int) > p_memory_start) {
        p_memory_previous_footer = ptr - 2*sizeof(int);
        p_memory_previous_header = p_memory_previous_footer - *(int*)p_memory_previous_footer - sizeof(int);
    }
    
    if (ptr + (*(int*)p_memory_temp * -1) + 2*sizeof(int) < p_memory_start + MAX_SIZE) {
        p_memory_next_header = ptr + (-1 * *(int*)p_memory_temp) + sizeof(int);
        p_memory_next_footer = p_memory_next_header + *(int*)p_memory_next_header + sizeof(int);
    }
    
    
    // hore alokovany alebo free?
    if (p_memory_previous_footer != NULL && *(int*)p_memory_previous_footer < 0) {
        previous_allocated = 1;
    } else {
        previous_allocated = (p_memory_previous_footer == NULL) ? 1 : 0;
    }
    
    // dole alokovany alebo free?
    if (p_memory_next_header != NULL && *(int*)p_memory_next_header < 0) {
        next_allocated = 1;
    } else {
        next_allocated = (p_memory_next_header == NULL) ? 1 : 0;
    }
    
    // hore aj dole su free
    if (!previous_allocated && !next_allocated) {
        total_size = -1**(int*)p_memory_temp + *(int*)p_memory_previous_header + *(int*)p_memory_next_header + 4*sizeof(int);
        *(int*)p_memory_start += total_size;
        
        // vymazanie sucasneho footera - WORKS!
        for (int i = 0; i < sizeof(int); i++)
            *(char*)( (ptr + -1**(int*)p_memory_temp) + i) = UNALLOCATED_WORD;
        
        // vymazanie sucasnych alokovanych slov -
        for (int i = 0; i < -1**(int*)p_memory_temp; i++)
            *(char*)(p_memory_temp + sizeof(int) + i) = UNALLOCATED_WORD;
        
        // vymazanie sucasneho headera
        for (int i = 0; i < sizeof(int); i++)
            *(char*)(p_memory_temp + i) = UNALLOCATED_WORD;
        
        // vymazanie predosleho footera
        for (int i = 0; i < sizeof(int); i++)
            *(char*)( p_memory_previous_footer + i) = UNALLOCATED_WORD;
        
        // vymazanie dalsej hlavicky
        for (int i = 0; i < sizeof(int); i++)
            *(char*)( p_memory_next_header + i) = UNALLOCATED_WORD;
        
        *(int*)p_memory_previous_header = total_size;
        *(int*)p_memory_next_footer = total_size;
        
        return 0;
    }
    
    // hore free, dole alokovane - DONE
    if (!previous_allocated && next_allocated) {
        total_size = -1**(int*)p_memory_temp + *(int*)p_memory_previous_header + 2*sizeof(int);
        *(int*)p_memory_start += total_size;
        
        *(int*)p_memory_previous_header = total_size;
        *(int*)((ptr + (-1 * *(int*)p_memory_temp))) = total_size;
        
        // vymazanie sucasneho headera
        for (int i = 0; i < sizeof(int); i++)
            *(char*)(p_memory_temp + i) = UNALLOCATED_WORD;
        
        // vymazanie predosleho footera
        for (int i = 0; i < sizeof(int); i++)
            *(char*)( p_memory_previous_footer + i) = UNALLOCATED_WORD;
        
        // vymazanie alokovanych slov
        for (int i = 0; i < total_size; i++)
            *(char*)((p_memory_previous_header + sizeof(int)) + i) = UNALLOCATED_WORD;
        
        return 0;
    }
    
    // hore alokovane, dole free - DONE
    if (previous_allocated && !next_allocated) {
        *(int*)p_memory_start += ((-1) * *(int*)p_memory_temp + 2*sizeof(int));
    
        // vymazanie sucasneho footera
        for (int i = 0; i < sizeof(int); i++)
            *(char*)( (ptr + (-1)**(int*)p_memory_temp) + i) = UNALLOCATED_WORD;
        
        // vymazanie dalsej hlavicky
        for (int i = 0; i < sizeof(int); i++)
            *(char*)( p_memory_next_header + i) = UNALLOCATED_WORD;
        
        // vymazanie alokovanych slov
        for (int i = 0; i < -1**(int*)p_memory_temp; i++)
            *(char*)(ptr + i) = UNALLOCATED_WORD;
        
        *(int*)p_memory_temp = *(int*)p_memory_start;
        *(int*)(p_memory_next_footer) = *(int*)p_memory_start;
        
        return 0;
    }
    
    // hore aj dole alokovane - DONE
    if (previous_allocated && next_allocated) {
        *(int*)p_memory_temp *= -1;
        *(int*)(p_memory_temp + *(int*)p_memory_temp + sizeof(int)) *= -1;
        
        *(int*)p_memory_start += *(int*)p_memory_temp + 2*sizeof(int);
        
        for (int i = 0; i < *(int*)p_memory_temp; i++)
            *(char*)(p_memory_temp + sizeof(int) + i) = UNALLOCATED_WORD;
        
        return 0;
    }
    
    return 1;
}

int memory_check(void *ptr)
{
    if(ptr > p_memory_start || ptr < p_memory_start + MAX_SIZE)
        return (ptr == NULL) ? 0 : (*(int*)(ptr-sizeof(int)) < 0) ? 1 : 0;
    else
        return 0;
}

void z1_testovac(char *region, char **pointer, int minBlock, int maxBlock, int minMemory, int maxMemory, int testFragDefrag) {
    unsigned int allocated = 0;
    unsigned int mallocated = 0;
    unsigned int allocated_count = 0;
    unsigned int mallocated_count = 0;
    unsigned int i = 0;
    int random_memory = 0;
    int random = 0;
    memset(region, 0, 100000);
    srand(time(0));
    random_memory = (rand() % (maxMemory-minMemory+1)) + minMemory;
    memory_init(region + 500, random_memory);
    if (testFragDefrag) {
        do {
            pointer[i] = memory_alloc(8);
            if (pointer[i])
                i++;
        } while (pointer[i]);
        for (int j = 0; j < i; j++) {
            if (memory_check(pointer[j])) {
                memory_free(pointer[j]);
            }
            else {
                printf("Error: Wrong memory check.\n");
            }
        }
    }
    
    i = 0;
    while (allocated <= random_memory-minBlock) {
        random = (rand() % (maxBlock-minBlock+1)) + minBlock;
        //printf("trying to alloc %d bytes\n", random);
        if (allocated + random > random_memory)
            continue;
        
        allocated += random;
        allocated_count++;
        pointer[i] = memory_alloc(random);
        if (pointer[i]) {
            printf("allocated %d bytes\n", random);
            i++;
            mallocated_count++;
            mallocated += random;
        }
    }
    
    
    for (int j = 0; j < i; j++) {
        if (memory_check(pointer[j])) {
            memory_free(pointer[j]);
        }
        else {
            printf("Error: Wrong memory check.\n");
        }
    }
    
    memset(region + 500, 0, random_memory);
    
    for (int j = 0; j < 100000; j++) {
        if (region[j] != 0) {
            region[j] = 0;
            printf("Error: Modified memory outside the managed region. index: %d\n",j-500);
        }
    }
    
    
    float result = ((float)mallocated_count / allocated_count) * 100;
    float result_bytes = ((float)mallocated / allocated) * 100;
    printf("Memory size of %d bytes: allocated %.2f%% blocks (%.2f%% bytes).\n", random_memory, result, result_bytes);
}


int main(int argc, const char * argv[]) {
    //char memory_array[MAX_SIZE];
    
   /* memory_init(memory_array, MAX_SIZE);
    //memory_set_unallocated_bytes(21, 4);
    //memory_set_allocated_bytes(10, 4);
    void *pointer = memory_alloc(9);
    void *pointer2 = memory_alloc(100);
    void *pointer3 = memory_alloc(21);
    void *pointer4 = memory_alloc(10);
    //memory_set_unallocated_bytes(41, 51);
    //void *pointer2 = memory_alloc(30, 0);
    //void *pointer2 = memory_alloc(3, 0);
    //void *pointer3 = memory_alloc(50, 1);
    //memory_free(pointer);
    //memory_free(pointer3);
    //memory_free(pointer2);*/
    
    char region[100000];
    char* pointer[13000];
    //z1_testovac(region, pointer, 8, 24, 50, 100, 0);
    z1_testovac(region, pointer, 8, 1000, 10000, 20000, 0);
    //z1_testovac(region, pointer, 8, 35000, 50000, 99000, 0);
    return 0;

    
    return 0;
}


