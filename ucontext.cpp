#include <iostream>
#include <ucontext.h>

using namespace std;

static ucontext_t ctx[3];

void func1(void)
{
    cout << "func1 on doing" << endl;
    swapcontext(&ctx[1], &ctx[2]);
    cout << "func1 finished" << endl;
}

void func2(void)
{
    cout << "func2 on doing" << endl;
    swapcontext(&ctx[2], &ctx[1]);
    cout << "func2 finished" << endl;
}

int main(int argc, char** argv)
{
    char stack1[8*1024];
    // getcontext(&ctx[1]);
    ctx[1].uc_stack.ss_sp = stack1;
    ctx[1].uc_stack.ss_size = sizeof(stack1);
    ctx[1].uc_link = &ctx[0];
    makecontext(&ctx[1], func1, 0);

    char stack2[8*1024];
    getcontext(&ctx[2]);
    ctx[2].uc_stack.ss_sp = stack2;
    ctx[2].uc_stack.ss_size = sizeof(stack2);
    ctx[2].uc_link = &ctx[1];
    makecontext(&ctx[2], func2, 0);

    cout << "main on doing" << endl;
    swapcontext(&ctx[0], &ctx[2]);
    cout << "main finished" << endl;

    return 0;
    /*
     * getcontext：获取当前context
     * setcontext：切换到指定context
     * makecontext: 用于将一个新函数和堆栈，绑定到指定context中
     * swapcontext：保存当前context，并且切换到指定context
     */
}