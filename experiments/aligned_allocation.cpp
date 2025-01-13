#include <iostream>
#include <cstdlib>
#include <cassert>
#include <stdint.h>

int main() {

    uint32_t alignment = 64;
    uint32_t fftSize = 256;
    uint32_t fftAllocSize = fftSize * sizeof(float);

    uint32_t dftSize = fftSize +2;
    uint32_t padding = dftSize*sizeof(float) & alignment;
    uint32_t dftAllocSize = dftSize*sizeof(float) + padding;

    uint32_t  totalAllocationSize = fftSize*2 + (dftAllocSize)*2;

    uintptr_t *memory = (uintptr_t*)aligned_alloc(alignment, totalAllocationSize);
    uint64_t index = 0;

    float* ptr1 = (float*)(memory+index);
    index += fftSize;
    float* ptr2 = (float*)(memory+index);
    index += fftSize;
    float* ptr3 = (float*)(memory+index);
    index += dftAllocSize;
    float* ptr4 = (float*)(memory+index);
    index += dftAllocSize;

    assert(((uintptr_t)ptr1 & 0xF) == 0);
    assert(((uintptr_t)ptr2 & 0xF) == 0);
    assert(((uintptr_t)ptr3 & 0xF) == 0);
    assert(((uintptr_t)ptr4 & 0xF) == 0);
    assert(index == totalAllocationSize);

    printf("fin du programme\n");


    free(memory);
    return 0;
}
