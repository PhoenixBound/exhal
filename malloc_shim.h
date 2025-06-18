#include <stddef.h> // size_t

void exhal_shim_init(int atf);
void *exhal_shim_malloc(size_t size);
void *exhal_shim_calloc(size_t num, size_t size);

#undef malloc
#define malloc(n) exhal_shim_malloc(n)
#undef calloc
#define calloc(n, m) exhal_shim_calloc(n, m)
