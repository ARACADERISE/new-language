#include "mem_management.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

memory_struct* setup_memory() {
    memory_struct* mem = malloc(sizeof(struct MEM_STRUCT));

    mem->memory_left_over = 0;
    mem->index = 0;
    /**/
    mem->total_allocated_memory = calloc(mem->index+1,sizeof(size_t));
    mem->next = calloc(2,sizeof(struct MEM_STRUCT*));
    mem->next[0] = mem;
    mem->next[1] = mem->next[0];

    return mem;
}

memory_struct* MIS_Match_Memory_Allocate(memory_struct* mem) {

    if(
        (mem->total_allocated_memory[0]^mem->total_allocated_memory[mem->index])>mem->total_allocated_memory[0]||
        (mem->total_allocated_memory[0]|mem->total_allocated_memory[mem->index])>=mem->total_allocated_memory[0]) 
    {
        // This should be the last size that was allocated
        mem->memory_current_ability = ((mem->total_allocated_memory[0]^mem->total_allocated_memory[mem->index])>mem->total_allocated_memory[0]) ? (mem->total_allocated_memory[mem->index]|mem->total_allocated_memory[0])^1 : mem->total_allocated_memory[mem->index]|mem->total_allocated_memory[0];
    } else {
        // OR bitwiser operation will take place since it is not larger in either cases
        mem->memory_current_ability = mem->total_allocated_memory[mem->index]|mem->total_allocated_memory[0];
    }

    return mem;

}

memory_struct* Brew_Update_Memory(memory_struct* mem) {

    MIS_Match_Memory_Allocate(mem);

    // memory_current_ability should be the same as the current allocation that was recently stored in total_allocated_memory. If not, throw an error
    if(!(mem->memory_current_ability==mem->total_allocated_memory[mem->index])) {
        sprintf(stderr,"Error:");
    }

    /* Setting up next[1] */
    mem->next[1] = mem->next[0];

    /*
        Setting to void pointer then reallocating memory for mem->next[0].
        There are other allocations going on with it while it's in use..we don't want that being carried throughout the program
    */
    mem->next[0] = (void*)0;
    mem->next[0] = calloc(1,sizeof(struct MEM_STRUCT*));

    mem->index++;
    mem->total_allocated_memory = realloc(
        mem->total_allocated_memory,
        mem->index+1
    );
    /* Reseting memory struct so we don't run into memory issues ourselves */
    mem->allocated.allocated_size = 0;
    mem->reallocated.re_allocated_size = 0;
    mem->calloc_.calloc_index_size = 0;
    mem->calloc_.allocated_index_size = 0;
    mem->DeAllocate.DeAllocatedSize = 0;
    mem->next[0] = mem;

    return mem;
}
char* Brew_Strict_DeAllocate(void* ptr, size_t strict_size, size_t size_of_allocation, size_t auto_allocate,memory_struct* mem) {
    if(!(ptr==NULL)){
        if(!(auto_allocate==0)) {
            ptr = realloc(
                ptr,
                (strict_size-auto_allocate/8)*size_of_allocation
            );
            mem->DeAllocate.DeAllocatedSize = (strict_size-auto_allocate/8)*size_of_allocation;
            mem->total_allocated_memory[mem->index] = mem->DeAllocate.DeAllocatedSize;
            Brew_Update_Memory(mem);
        }
        else ptr = realloc(ptr,strict_size*size_of_allocation);
    }
    else ptr = (void*)0; // nothing to assign..will be a void poitner

    return ptr;
}
char* Brew_Realloc_Memory_Strict(void* ptr, size_t mem_to_assign, size_t sizeof_,memory_struct* mem) {
    size_t total_size = 0;

    ptr = realloc(
        ptr,
        mem_to_assign*sizeof_
    );
    mem->reallocated.re_allocated_size = mem_to_assign*sizeof_;
    mem->total_allocated_memory[mem->index] = mem->reallocated.re_allocated_size;
    Brew_Update_Memory(mem);

    /* Checking memory allocation */
    for(int i = 0; i < strlen(ptr); i++) {
        total_size+=8;
    }
    if(mem_to_assign*sizeof_>total_size/8) {
        printf("\n\nErr: Memory reallocation failed.\nAllocated %ld bits less than what was aquired.\n\n",mem_to_assign*sizeof_-total_size/8);
        exit(1);
    }
    if(mem_to_assign*sizeof_<total_size/8) {
        printf("\n\nErr: Memory reallocation failed.\nAllocated %ld bits above what was aquired.\n\n",total_size/8-mem_to_assign*sizeof_);
        exit(1);
    }

    return ptr;
}
void* Brew_Allocate_Memory(void *ptr, size_t mem_to_assign, size_t sizeof_,memory_struct* mem) {
    ptr = malloc(
        mem_to_assign*sizeof_
    );
    mem->allocated.allocated_size = mem_to_assign*sizeof_;
    mem->total_allocated_memory[mem->index] = mem->allocated.allocated_size;

    /* Updating memory block */
    Brew_Update_Memory(mem);

    return ptr;
}