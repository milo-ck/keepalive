#pragma once
#include "xlib/pthread_.h"
namespace xlib
{
    class MutexSignal
    {
    private:
        pthread_mutex_t mutex;
        pthread_cond_t cond;
        Nil init()
        {
            pthread_cond_init(&cond, 0);
            pthread_mutex_init(&mutex, 0);
        };
        Nil release()
        {
            pthread_mutex_destroy(&mutex);
            pthread_cond_destroy(&cond);
        };
    public:
        MutexSignal()
        {
            init();
        };
        ~MutexSignal()
        {
            release();
        };
        Int lock()
        {
            return pthread_mutex_lock(&mutex);
        };
        Int unlock()
        {
            return pthread_mutex_unlock(&mutex);
        };
        Int wait()
        {
            return pthread_cond_wait(&cond, &mutex);
            //this function atomically release mutex and cause the calling thread to block on the condition variable cond;
            //respect to access by another thread to the mutex and then condition variable.
            //Upon successful return, the mutex shall have been locked and shall be owned by the calling thread.
        };
        Int broadcast()
        {
            return pthread_cond_broadcast(&cond);
            //this function shall unblock all threads currently blocked on the specified condition variable cond.
        };
        Int signal()
        {
            return pthread_cond_signal(&cond);
        };
    };
};

