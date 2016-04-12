#pragma once
#include "core/kastd.h"
#include <vector>
namespace ka
{
	namespace data
	{
		class RequestPool : public core::AutoRef
		{
		private:
			xlib::Mutex mutex_;
			struct I
			{
				ka::id id;
				ka::id from;
			};
			std::vector<I> items_;
			Int len_;
		public:
			static RequestPool* get(data::Node* node)
			{
				if (node->requestPool)
					return (RequestPool*)node->requestPool;
				return 0;
			}
			static RequestPool* getOrNew(data::Node* node)
			{
				if (node->requestPool == 0)
				{
					node->requestPool = new RequestPool();
				}
				return (RequestPool*)node->requestPool;
			}
			RequestPool() :len_(0)
			{

			}
			virtual Nil push(ka::id id, ka::id from)
			{
				xlock_mutex(&mutex_);
				I i = { id, from };
				items_.push_back(i);
				len_++;
			}

			virtual Int length() { return len_; };
			virtual ka::id idAt(Int i) { return i < len_ && i >= 0 ? items_[i].id : 0; };
			virtual ka::id fromAt(Int i) { return i < len_ && i >= 0 ? items_[i].from : 0; };
			virtual Nil remove(Int i)
			{
				xlock_mutex(&mutex_);
				if (i >= 0 && i < len_)
				{
					items_.erase(items_.begin() + i);
					len_--;
				}
			}
			virtual Nil remove(ka::id id)
			{
				Int len = length();
				for (Int i = len - 1; i >= 0; i--)
				{
					if (idAt(i) == id)
						remove(i);
				}
			}
		};

	}
}
