#include "../h/syscall_cpp.hpp"

void
Thread::wrapper(void *handle)
{
    if(handle)
        ((Thread*)handle)->run();
}

Thread::Thread(void (*body)(void *), void *arg)
{
    myHandle = nullptr;
    thread_create_wo_start(&myHandle, body, arg);
}

Thread::Thread()
{
    myHandle = nullptr;
    thread_create_wo_start(&myHandle, Thread::wrapper, this);
}

Thread::~Thread()
{
    myHandle = nullptr;
    delete myHandle;
}

int
Thread::start()
{
    int ret = -3;

    if(myHandle)
        ret = thread_start(myHandle);

    return ret;
}

void
Thread::dispatch()
{
    thread_dispatch();
}