#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

// LD_PRELOAD，是个环境变量，用于动态库的加载，动态库加载的优先级最高，一般情况下，其加载顺序为
// LD_PRELOAD>LD_LIBRARY_PATH>/etc/ld.so.cache>/lib>/usr/lib。
// 自己编译的库的加载顺序优先于系统库
int main()
{
    // dlopen的测试代码, dlsym
    // typedef int(* FUNC_ADD)(int, int);
    // const char* dllpath = "./libhook.so";

    // void* handle = dlopen(dllpath, RTLD_LAZY);

    // if (!handle)
    // {
    //     fprintf(stderr, "[%s][%d] dlopen get error: %s\n", __FILE__, __LINE__, dlerror());
    //     exit(1);
    // }

    // do
    // {
    //     FUNC_ADD add_func = (FUNC_ADD)dlsym(handle, "add");
    //     printf("1 add 2 is :%d\n", add_func(1,2));
    // }while(0);

    // dlclose(handle);

    int* p = (int*)malloc(sizeof(int));
    free(p);

    return 0;
}

// gcc -o dltest dltest.cc -ldl2


// gcc hook.cc -shared -fPIC -o libhook.so -ldl
// gcc -o dltest dltest.cc
// LD_PRELOAD=./libhook.so ./dltest
