#pragma once
#include "xlib/pthread_.h"
namespace xlib
{
	template<class T>
	class Thread
	{
	private:
		Int id_;
		typedef Nil (T::*Handle)(Nil* args);
		const Handle handler_;
		T* owner_;
		Nil* args_;
		bool autoDispose;
	public:
		Thread() :id_(0), handler_(0), owner_(0), args_(0), autoDispose(false)
		{

		}
		~Thread()
		{
			id_ = 0;
			handler_ = 0;
			owner_ = 0;
			args_ = 0;
			autoDispose = false;
		}
		static Thread* start(T* owner, const Handle handler, Nil* args)
		{
			Thread* thread = new Thread();
			thread->autoDispose = true;
			thread->start(owner, handler, args);
			return thread;
		};
	private:
		static Nil* running(Nil* args)
		{
			Thread* t = (Thread*)args;
			if(t->owner_)
				t->owner_->*(t->handler_)(t->args_);
			if (t->autoDispose)
				delete t;
			return 0;
		}
		
	public:
		Int id() { return id_; };
		bool start(T* owner, const Handle handler, Nil* args)
		{
			owner_ = owner;
			handler_ = handler;
			args_ = args;
			pthread_t id;
			if (pthread_create(&id, 0, running, (Nil*)this) == 0)
			{
				id_ = (Int)id.x;
				return true;
			}
			id_ = 0;
			return false;
		}
	};
};
