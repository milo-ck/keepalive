#pragma once
#include <vector>
#include <string>
#include "xlib/xlib.h"

namespace ka
{
	typedef unsigned long long id;

	namespace data
	{
		struct Array
		{
			Object content;
			xlib::Mutex* mutex;
		};
		struct Node //the node release by NodeClose at core/msg/cmd/
		{
			/*Object recvRef; //@see IBuffer
			Object sendRef; //@see IBufferChain
			Object requestRef;//@see IReqeustPool*/
			Object tag;
			Object sender;
			Object recver;
			Object requestor;

			
			UInt updateTime;
			UInt createTime;
			UInt flags;
			Int socket;
			Byte ip[4];
			UShort port;
			
			// The node type defined in low 4 bits. 
			//If the value is 1 then it indicate at from device, 
			//If the value is 2 then it indicate at from editor.
			//If the value is 3 then it indicate at from web.
			const Char* name;
			ka::id id;

		};


		struct Group
		{
			Array* devices;
			const Char* name;
			UShort id;
		};
		struct Device
		{
			Array* nodes;
			const Char* name;
			UShort id;
		};
		template <class T>
		class Iterator
		{
		private:
			std::vector<T*>* arr;
			xlib::Mutex* mutex;
			Int i;
		public:
			T* current;
			Iterator(Array* data)
			{
				this->arr = (std::vector<T*>*)data->content;
				this->mutex = data->mutex;
				this->i = length();
			};
			~Iterator()
			{
				arr = 0;
				i = 0;
				mutex = 0;
				current = 0;
				len = 0;
			}
			T* next(Int limit = 0)
			{
				i--;
				if (i >= limit)
				{
					current = arr[i];
					return current;
				}
				current = 0;
				return 0;
			};
			T* at(Int i)
			{
				return arr[i];
			}
			Iterator<T>* reset()
			{
				i = length();
				return this;
			}

			Int length() { return arr ? this->arr->size() : 0; }

			Iterator<T>* lock()
			{
				if (mutex)mutex->lock();
				return this;
			}
			Nil unlock()
			{
				if (mutex)mutex->unlock();
			}
			T* push(T* item)
			{
				if (item)arr->push_back(item);
				return item;
			}
			T* remove(T* item)
			{
				for (std::vector<T*>::iterator it = arr->begin(); it != arr->end(); ++it)
				{
					if (*it == item)
					{
						arr->erase(it);
						return item;
					}
				}
				return 0;
			}

		};
	};
};

#include "core/data.u.h"

