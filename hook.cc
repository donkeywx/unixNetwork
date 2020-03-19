#include <stdio.h>
#include <dlfcn.h>
extern "C"
{
// int add(int a, int b)
// {
//     return a + b;
// }

static unsigned int invokeTimes = 0;
typedef void* (*malloc_fun)(size_t);
void* malloc(size_t sz)
{
    malloc_fun my_malloc = (malloc_fun)dlsym(RTLD_NEXT, "malloc");
    invokeTimes ++;
    fprintf(stderr, "my malloc invoked: %d\n", sz);

    return my_malloc(sz);
}
}
// gcc hook.cc -shared -fPIC -o libhook.so