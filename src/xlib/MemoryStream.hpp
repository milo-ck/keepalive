#pragma once
#include "xlib/types_.h"
#include "xlib/allocator.hpp"
#include "xlib/stream_.h"
#include "xlib/Exception.hpp"
namespace xlib
{
	class MemoryStream : public IStream
	{
	private:
		Int position_;
		Int length_;
		Byte* buffer_;
		Int capacity_;

		Boolean setCapacity(Int len)
		{
			xthrow(len < 0, error::OutOfRange, "Failed to set capacity to %d", len);
			if(capacity_ < len)
			{
				Byte* temp = alloc<Byte>(len);
				if(buffer_)
				{
					memcpy(temp, buffer_, length_);
					dealloc(buffer_);
				}
				buffer_ = temp;
				capacity_ = MemoryPool::pool()->align(len);
				return true;
			}
			return true;
		}
	public:
		MemoryStream(Int capacity = 0) : capacity_(0), length_(0),position_(0),buffer_(null)
		{
			capacity = capacity <= 0 ? 1024 : capacity;
			setCapacity(capacity);
		}
		virtual ~MemoryStream()
		{
			dealloc(buffer_);
			position_ = 0;
			length_ = 0;
			capacity_ = 0;
		}

		virtual Int		position()			{ return position_;				}
		virtual Boolean position(Int pos)	{ position_ = pos;	return true;}
		virtual Int		length()			{ return length_;				}
		virtual Boolean eos()				{ return position_ >= length_;	}

		virtual Int		read(Byte* data, Int len)
		{
			Int count = length_ - position_;
			count = count > len ? len : count;
			if(count <= 0)return 0;
			memcpy(data, &buffer_[position_], count);
			position_ += count;
			return count;
		}
		virtual Int		write(const Byte* data, Int len)
		{
			Int count = position_ + len;
			setCapacity(count);
			memcpy(&buffer_[position_], data, len);
			length_ = count > length_ ? count : length_;
			position_ += len;
			return len;
		}
	};
}