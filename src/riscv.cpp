//
// Created by marko on 20.4.22..
//

#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../lib/console.h"
#include "../test/printing.hpp"
#include "../h/syscall_c.h"
#include "../h/my_mem.h"
#include "../h/sem.hpp"

using Body = void (*)(void*);

void Riscv::popSppSpie()
{
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

void Riscv::handleSupervisorTrap()
{
    uint64 volatile scause = r_scause();

    if (scause == 0x0000000000000008UL || scause == 0x0000000000000009UL)
    {
        // Interrupt: no, cause code: environment code for S-mode(9)
        uint64 sepc = r_sepc() + 4;
        uint64 sstatus = r_sstatus();

        uint64 number;

        __asm__ volatile ("mv %0, a0" : "=r" (number));

        switch (number)
        {
            case 0x01:
            {
                size_t allocate;
                __asm__ volatile ("mv %0, a1" : "=r" (allocate));

                uint64 *allocated_address = (uint64*) __mem_alloc(allocate);
                __asm__ volatile ("mv a0, %0" : : "r" (allocated_address));

                break;
            }
            case 0x02:
            {
                void *allocated_address;
                __asm__ volatile ("mv %0, a1" : "=r" (allocated_address));

                int ret = __mem_free(allocated_address);
                __asm__ volatile ("mv a0, %0" : : "r" (ret));
                break;
            }
            case 0x11:
            {
                thread_t *handle;
                Body body;
                void *arg;
                uint64  *stack;

                __asm__ volatile ("mv %0, a1" : "=r" (handle));
                __asm__ volatile ("mv %0, a2" : "=r" (body));
                __asm__ volatile ("mv %0, a6" : "=r" (arg));

                if (body != 0)
                    stack = (uint64 *)new uint64[DEFAULT_STACK_SIZE];
                else
                    stack = 0;

                *handle = TCB::createThread(body, arg, stack, true);

                int ret;
                ret = (*handle) != nullptr ? 0 : -1;
                __asm__ volatile ("mv a0, %0" : : "r" (ret));

                break;
            }
            case 0x12:
            {
//                printString("Exiting...\n");
                int ret;
                ret = TCB::threadExit();
                __asm__ volatile ("mv a0, %0" : : "r" (ret));

                break;
            }
            case 0x13:
            {
                TCB::dispatch();

                break;
            }
            case 0x14:
            {
                thread_t *handle;
                Body body;
                void *arg;
                uint64  *stack;

                __asm__ volatile ("mv %0, a1" : "=r" (handle));
                __asm__ volatile ("mv %0, a2" : "=r" (body));
                __asm__ volatile ("mv %0, a5" : "=r" (arg));
                __asm__ volatile ("mv %0, a6" : "=r" (arg));

                if (body != 0)
                    stack = (uint64 *)new uint64[DEFAULT_STACK_SIZE];
                else
                    stack = 0;

                *handle = TCB::createThread(body, arg, stack, false);

                int ret;
                ret = (*handle) != nullptr ? 0 : -1;
                __asm__ volatile ("mv a0, %0" : : "r" (ret));

                break;
            }
            case 0x15:
            {
                thread_t *handle;
                __asm__ volatile ("mv %0, a1" : "=r" (handle));

                TCB::threadStart((TCB*)handle);
                break;
            }
            case 0x21:
            {
                unsigned init;
                sem_t* handle;

                __asm__ volatile ("mv %0, a2" : "=r" (init));
                __asm__ volatile ("mv %0, a6" : "=r" (handle));

                int retu;
                retu = Sem::__open(handle, init);

                __asm__ volatile ("mv a0, %0" : : "r" (retu));
                break;
            }
            case 0x22:
            {
                sem_t handle;
                __asm__ volatile ("mv %0, a1" : "=r" (handle));

                int retu;
                retu = handle->__close();

                __asm__ volatile ("mv a0, %0" : : "r" (retu));

                break;
            }
            case 0x23:
            {
                sem_t handle;
                __asm__ volatile ("mv %0, a1" : "=r" (handle));

                int retu;
                retu = handle->__wait();

                __asm__ volatile ("mv a0, %0" : : "r" (retu));

                break;
            }
            case 0x24:
            {
                sem_t handle;
                __asm__ volatile ("mv %0, a1" : "=r" (handle));

                int retu;
                retu = handle->__signal();

                __asm__ volatile ("mv a0, %0" : : "r" (retu));
                break;
            }
        }

//        TCB::timeSliceCounter = 0;
//        TCB::dispatch();
        w_sstatus(sstatus);
        w_sepc(sepc);
    }
    else if (scause == 0x8000000000000001UL)
    {
        // Interrupt: yes, cause code: Software interrupt (timer)
        TCB::timeSliceCounter++;

        if (TCB::timeSliceCounter >= TCB::running->getTimeSlice())
        {
            uint64 sepc = r_sepc();
            uint64 sstatus = r_sstatus();
            TCB::timeSliceCounter = 0;
            TCB::dispatch();
            w_sstatus(sstatus);
            w_sepc(sepc);
        }

        mc_sip(SIP_SSIP);
    }
    else if (scause == 0x8000000000000009UL)
    {
        // Interrupt: yes, cause code: Software external interrupt (console)
        console_handler();
    }
    else
    {
        // Unexpected trap cause
        //printInt(scause);
//        printInt(sepc);
//        printInt(stvalue);
//        printString("Unexpected Trap Cause...\n");
    }
}