#pragma once
#include "xlib/pthread_.h"
namespace xlib
{
    class Mutex
    {
    private:
        pthread_mutex_t mutex;
        Nil init()
        {
            pthread_mutex_init(&mutex, 0);
        };
        Nil release()
        {
            pthread_mutex_destroy(&mutex);
        };
    public:
        Mutex()
        {
            init();
        };
        ~Mutex()
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
    };
	class MutexLocker
	{
	private:
		Mutex* m_;
	public:
		MutexLocker(Mutex* m):m_(m)
		{
			if(m_)m_->lock();
		}
		~MutexLocker()
		{
			if(m_)m_->unlock();
		}
	}; 
#define __lock_mutex(m, n) xlib::MutexLocker __mutex__##n (m) 
#define xlock_mutex(m) __lock_mutex(m, __LINE__)
};

