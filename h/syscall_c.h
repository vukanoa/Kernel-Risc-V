#ifndef _syscall_c
#define _syscall_c
#include "../lib/hw.h"
#include "../h/tcb.hpp"

#define NULL 0


void* mem_alloc (size_t size);
int mem_free (void*);


class TCB;
typedef TCB* thread_t;

int  thread_create(thread_t *handle, void(*start_routine)(void*), void *arg);
int  thread_exit();
void thread_dispatch();
int  thread_create_wo_start(thread_t *handle, void(*start_routine)(void*), void *arg);
int  thread_start(thread_t handle);



class Sem;
typedef Sem* sem_t;

int sem_open (sem_t* handle, unsigned init);
int sem_wait (sem_t id);
int sem_signal (sem_t id);
int sem_close (sem_t handle);



typedef unsigned long time_t;
int time_sleep (time_t);

const int EOF = -1;
char getc ();
void putc (char);


#endif
