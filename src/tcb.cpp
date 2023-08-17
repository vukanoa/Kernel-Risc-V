//
// Created by os on 2/11/23.
//

#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../test/printing.hpp"

TCB *TCB::running = nullptr;
uint64 TCB::timeSliceCounter = 0;

//TCB* TCB::createThread(Body body)
//{
//    return new TCB(body, TIME_SLICE);
//}

TCB*
TCB::createThread(Body body, void* arg, void* stack_space, bool start_immediately)
{
    return new TCB(body, arg, stack_space, start_immediately);
}

//void TCB::yield()
//{
//    __asm__ volatile("ecall");
//}


int
TCB::threadExit()
{
    if (running->isFinished() || running->getState() != State::ACTIVE)
        return -1;

    running->setFinished(true);
    running->state = State::FINISHED;
    TCB::dispatch();

    return 0;
}


void
TCB::dispatch()
{
    TCB *old = running;

    if (old->state == State::ACTIVE)
    {
        old->state = State::READY;
        Scheduler::put(old);
    }

    running = Scheduler::get();

    if (running)
    {
        running->setState(State::ACTIVE);
        TCB::contextSwitch(&old->context, &running->context);
    }
    else
        printString("\n------- NESTO ---\n");

}


void
TCB::threadWrapper()
{
    Riscv::popSppSpie();
    running->body(running->arg);
    running->setFinished(true);
    running->setState(State::FINISHED);

    TCB::dispatch();
}


int
TCB::threadStart(TCB *handle)
{
    if (handle)
    {
        if (handle->getState() != State::NEW)
            return -1;

        handle->setFinished(false);
        handle->state = State::READY;
        Scheduler::put(handle);

        return 0;
    }

    return -1;
}
