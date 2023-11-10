#include "cuda.h"

// Implement a simple cuda kernel to write a string to a buffer.
__global__ void hello(char* a, int* b)
{
	a[threadIdx.x] += b[threadIdx.x];
}