//
// Created by os on 2/11/23.
//

#include "../lib/console.h"
// #include "../h/my_mem.h"
#include "../h/tcb.hpp"
#include "../src/my_mem.c"
#include "../test/printing.hpp"
#include "../h/riscv.hpp"


extern void userMain();

bool done = false;
void
helper(void *arg)
{
    userMain();
    done = true;
}

int
main()
{
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);

    thread_t main, user_main;

    thread_create(&main, nullptr, nullptr);
    printString("Main Created\n");
    TCB::running = main;

    thread_create(&user_main, helper, nullptr);
    printString("UserMain Created\n");

    while (!done)
        thread_dispatch();

    printString("\nZavrsio\n");
    return 0;
}






/*
int main()
{
    // TEST MEM ALLOC
    int* buffer  = (int*) __mem_alloc(1);
    buffer[0] = 0;
    buffer[1] = 1;
    buffer[2] = 2;
    buffer[3] = 3;
    buffer[4] = 4;
    buffer[5] = 5;
    buffer[6] = 6;
    buffer[7] = 7;
    buffer[8] = 8;
    buffer[9] = 9;
    buffer[15] = 15;
    // buffer[16] = 16; // scause = 0x0000000000000007, sepc = 0x00000000000015b4, stval = 0x0, panic: kerneltrap


    int* ime = (int*) __mem_alloc(5);
    ime[0] = 8;
    ime[2] = 9;

    if (__mem_free(buffer) < 0)
        __putc('G');

    if (__mem_free(ime) < 0)
        __putc('a');

    int* nesto = 0;
    if (__mem_free(nesto) < 0) // We wanted -1
        __putc('f');

    if (__mem_free(ime) < 0) // We wanted -1
        __putc('g');


    int* buffer1  = (int*) __mem_alloc(1);
    buffer1[0] = 0;
    buffer1[1] = 1;
    buffer1[2] = 2;
    buffer1[3] = 3;
    buffer1[4] = 4;
    buffer1[5] = 5;
    buffer1[6] = 6;
    buffer1[7] = 7;
    buffer1[8] = 8;
    buffer1[9] = 9;
    buffer1[15] = 15;

    int cnt = 0;
    while (buffer1 != 0)
    {
        buffer1 = (int*) __mem_alloc(1000);
        cnt++;
    }

    // Possible to alocate 1000 blocks 2096 times + 1block and then we're left with size=1584 B
    buffer1  = (int*) __mem_alloc(1000);

    if (buffer1)
        buffer1[5] = 9 + cnt;

    // __putc(cnt);
    __putc('k');


    return 0;
}*/


/*
bool done = false;

void
helper(void *arg)
{
    userMain();
    done = true;
}

// Threads CPP_API test
int
main()
{
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);

    thread_t main, user_main;

    thread_create(&main, nullptr, nullptr);
    printString("Main Created\n");
    TCB::running = main;

    thread_create(&user_main, helper, nullptr);
    printString("UserMain Created\n");


    *//* TEST SISTEMSKIH POZIVA mem_alloc i mem_free *//*

    int* buffer  = (int*) mem_alloc(20); // 20 bytes, 1 block, 64 bytes => 16 4bytes integers
    buffer[0] = 0;
    buffer[1] = 1;
    buffer[2] = 2;
    buffer[3] = 3;
    buffer[4] = 4;
    buffer[5] = 5;
    buffer[6] = 6;
    buffer[7] = 7;
    buffer[8] = 8;
    buffer[9] = 9;
    buffer[15] = 15;
    // buffer[16] = 16; // scause = 0x0000000000000007, sepc = 0x00000000000015b4, stval = 0x0, panic: kerneltrap


    int* ime = (int*) mem_alloc(300); // 300 bytes, 5 blocks, 320 bytes
    ime[0] = 8;
    ime[2] = 9;

    if (mem_free(buffer) < 0)
        printString("Greska prilikom free-ovanja buffer-a\n");

    if (mem_free(ime) < 0)
        printString("Greska prilikom free-ovanja ime-a\n");

    int* buffer1  = (int*) mem_alloc(3); // 3 bytes, 1 block, 64 bytes => 16 * 4 bytes integers
    buffer1[0] = 0;
    buffer1[1] = 1;


    // Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    while (!done)
        thread_dispatch();

    printString("Finished\n");



    return 0;
}*/


// Threads_C_API test
/*int
main()
{
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
    TCB* threads[3];

    thread_create(&threads[0], nullptr, nullptr);
    TCB::running = threads[0];

    thread_create(&threads[1], workerBodyC, nullptr);
    printString("ThreadC Created\n");

    thread_create(&threads[2], workerBodyD, nullptr);
    printString("ThreadD Created\n");


    //Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    while (!(
             threads[1]->isFinished() &&
             threads[2]->isFinished()))
    {
        thread_dispatch();
    }

    printString("Startujem D\n"); // thread_exit-om smo ugasili bas D nit unutar workspace.cpp
    thread_start(threads[2]);
    thread_dispatch();

    while (!threads[2]->isFinished())
        thread_dispatch();


    for(auto& thread : threads)
        delete thread;

    printString("Finished\n");

    return 0;
}*/