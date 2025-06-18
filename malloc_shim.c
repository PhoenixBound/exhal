#include <stddef.h>
#include <stdlib.h>

int allocations_so_far;
int allocation_to_fail;

void exhal_shim_init(int atf) {
	allocations_so_far = 0;
	allocation_to_fail = atf;
}

void *exhal_shim_malloc(size_t size) {
	if (allocations_so_far++ == allocation_to_fail) return NULL;
	return malloc(size);
}

void *exhal_shim_calloc(size_t num, size_t size) {
	if (allocations_so_far++ == allocation_to_fail) return NULL;
	return calloc(num, size);
}
