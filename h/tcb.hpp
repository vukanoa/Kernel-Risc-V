//
// Created by os on 2/11/23.
//

#ifndef PROJECT_BASE_TCB_HPP
#define PROJECT_BASE_TCB_HPP

#include "../lib/hw.h"
#include "scheduler.hpp"

typedef enum State{ NEW, ACTIVE, READY, BLOCKED, FINISHED } State;

// Thread Control Block
class TCB {
public:
    ~TCB() { delete[] stack; }

    bool isFinished() const      { return finished;  }
    void setFinished(bool value) { finished = value; }
    uint64 getTimeSlice() const  { return timeSlice; }

    static void dispatch();


    State getState()            { return state;     }
    void  setState(State state) {TCB::state = state;}

    using Body = void (*)(void*);

    // static TCB *createThread(Body body);
    static TCB *createThread(Body body, void* arg, void* stack_space, bool start_immediately);
//  static void yield();

    static TCB *running;

private:
//    explicit TCB(Body body, uint64 timeSlice) :
//            body(body),
//            stack(body != nullptr ? new uint64[STACK_SIZE] : nullptr),
//            context({(uint64)&threadWrapper,
//                     stack != nullptr ? (uint64) &stack[STACK_SIZE] : 0
//                    }),
//            timeSlice(timeSlice),
//            finished(false) {
//        if (body != nullptr)
//            Scheduler::put(this);
//    }
    explicit TCB(Body body, void* arg, void* stack_space, bool start_immediately) :
            body(body),
            arg(arg),
            stack(body != nullptr ? (uint64*)stack_space : nullptr),
            context({(uint64)&threadWrapper,
                     stack != nullptr ? (uint64) &stack[DEFAULT_STACK_SIZE] : 0
                    }),
            timeSlice(DEFAULT_TIME_SLICE),
            finished(false),
            start_immediately(start_immediately)
    {
        if (body != nullptr)
        {
            if (start_immediately == true)
            {
                this->state = READY;
                Scheduler::put(this);
            }
            else
                this->state = NEW;
        }
        else
            this->state = ACTIVE;
    }

    struct Context {
        uint64 ra;
        uint64 sp;
    };

    Body body;
    void* arg = nullptr;
    uint64 *stack;
    Context context;
    uint64 timeSlice;
    State state;
    bool finished;
    bool start_immediately;

    friend class Riscv;

    static void threadWrapper();
    static void contextSwitch(Context *oldContext, Context *runningContext);
    static int threadExit();
    static int threadStart(TCB* handle);

    static uint64 timeSliceCounter;

    static uint64 constexpr STACK_SIZE = 1024;
    static uint64 constexpr TIME_SLICE = 2;
};


#endif //PROJECT_BASE_TCB_HPP
