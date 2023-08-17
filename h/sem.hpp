#ifndef MY_SEM
#define MY_SEM


#include "../h/list.hpp"
#include "../h/tcb.hpp"

class Sem {
public:
    Sem(unsigned int capacity);
    ~Sem();
    static int __open(Sem **handle, unsigned init);
    int __close ();
    int __wait  ();
    int __signal();

    int __getValue();
private:
    int value;
    bool open;
    List<TCB> blockedCoroutineQueue;
};

#endif // MY_SEM