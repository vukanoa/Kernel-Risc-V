#include "../h/sem.hpp"

class TCB;
typedef TCB* thread_t;


Sem::Sem(unsigned init)
{
    this->value = (int)init;
    this->open = true;
}


Sem::~Sem()
{
    __close();
}


int
Sem::__open(Sem **handle, unsigned init)
{
    *handle = new Sem(init);

    if (*handle)
        return 0;

    return -1;
}


int
Sem::__close()
{
    if(!open)
        return -1;

    open = false;

    while(blockedCoroutineQueue.peekLast())
    {
        thread_t curr = blockedCoroutineQueue.removeFirst();
        curr->setState(State::READY);
        Scheduler::put(curr);
    }

    return 0;
}


int
Sem::__wait()
{
    if(!open)
        return -1;

    if(--(this->value) < 0)
    {
        TCB::running->setState(State::BLOCKED);
        blockedCoroutineQueue.addLast(TCB::running);
        TCB::dispatch();
    }

    return 0;
}


int
Sem::__signal()
{
    if(!open)
        return -1;

    if(++(this->value) <= 0)
    {
        thread_t curr = blockedCoroutineQueue.removeFirst();
        curr->setState(State::READY);
        Scheduler::put(curr);
    }


    return 0;
}


int
Sem::__getValue()
{
    return this->value;
}