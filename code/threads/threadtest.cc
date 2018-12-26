#include "kernel.h"
#include "main.h"
#include "thread.h"
using namespace std;

void
SimpleThread(int which)
{
    int num;
    
    for (num = 0; num < 5; num++) {
        printf("*** thread %d looped %d times\n", which, num);
        kernel->currentThread->Yield();
    }
}

void
ThreadTest()
{
    Thread *t = new Thread("forked thread");
    t->Fork((VoidFunctionPtr) SimpleThread, (void *) 1);
    cout<<"The id is"<<t->getid();
    
    //SimpleThread(0);
}
