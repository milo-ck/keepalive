#pragma once
#include "xlib/xlib.h"
#include <vector>
namespace ka
{
	namespace core
	{
		enum IDs
		{
			IDMessage,
			IDTCPEntry,
			IDHTTPEntry,
		};

		struct Parameter
		{
			Byte fromId;
			Byte toId;
			Byte method;
			Byte reserve;
			Nil* p1;
			Nil* p2;
			Nil* p3;
		};

		class IModule : public core::AutoRef
		{
		private:
			friend class Controller;
			friend class Thread;

			xlib::MutexSignal signal_;
			std::vector<Parameter> parameters_;
			Nil loop(Object args)
			{
				running = true;
				Parameter p;
				while (running)
				{
					bool isNew = false;
					signal_.lock();
					Int len = parameters_.size();
					if (len == 0)
					{
						signal_.wait();
						len = parameters_.size();
					}
					if (len > 0)
					{
						p = parameters_[len - 1];
						isNew = true;
						parameters_.pop_back();
					}
					signal_.unlock();
					if(isNew)execute(&p);
				}
			}
			Nil put(const Parameter* p)
			{
				
				signal_.lock();
				parameters_.push_back(*p);
				signal_.signal();
				signal_.unlock();
			};
		protected:
			virtual Nil disposing()
			{
				running = false;
				signal_.broadcast();
			}
			virtual Nil execute(const Parameter* p) = 0;
			bool running;

		public:
			virtual Byte id() = 0;
			void run(Int numThreads)
			{
				for (Int i = 0; i < numThreads; i ++)
				{
					xlib::Thread<IModule>::start(this, loop, 0);
				}
			}
		};
		class Controller
		{
		private:
			std::vector<IModule*> components_;
			static Controller* controller()
			{
				static Controller ctrol;
				return &ctrol;
			}
			Nil pushModule(IModule* component)
			{
				components_.push_back(component);
			};
			bool pushParam(const Parameter* p)
			{
				for (std::vector<IModule*>::iterator it = components_.begin(); it != components_.end(); ++it)
				{
					if ((*it)->id() == p->toId)
					{
						(*it)->put(p);
						return true;
					}
						
				}
				return false;
			};
		public:
			
			Nil static add(IModule* component)
			{
				controller()->pushModule(component);
			};
			bool static put(const Parameter* p)
			{
				return controller()->pushParam(p);
			};
		};
	};
};

