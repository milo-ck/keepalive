#pragma once
#include "xlib/types_.h"
#include "xlib/Mutex.hpp"
#include "xlib/Log.hpp"
#include "xlib/Exception.hpp"

namespace xlib
{
	class MemoryPool
	{
	private:
		class Block
		{
		private:
			Int size_;
			Byte* data_;
			Block* next_;
			UShort count_;
			UShort first_;
			UShort used_;
		public:
			Block(Byte* data, Int size, UShort count) : next_(null), used_(0)
			{
				count_ = count;
				size_ = size;
				first_ = 0;
				data_ = data;
				for (Int i = 0; i < count; i++)
				{
					UShort next = i + 1;
					data[0] = (next >> 8) & 0xff;
					data[1] = next & 0xff;
					data += size;
				}
			}
			~Block()
			{
				if(data_)free(data_);
				data_ = null;
				next_ = null;
				count_ = 0;
			}
			Byte* alloc()
			{
				used_++;
				Byte* data = &data_[first_ * size_];
				first_ = (data[0] << 8) | data[1];
				return data;

			}
			Nil dealloc(Object ptr)
			{
				Byte* data = (Byte*)ptr;
				data[0] = (first_ >> 8) & 0xff;
				data[1] = first_ & 0xff;
				first_ = static_cast<UShort>((data - data_) / size_);
				used_--;
			}
			Boolean available() { return used_ < count_; }
			Boolean contains(Object data)
			{
				return data >= data_ && data < (data_ + size_ * count_);
			}
			Block* next() { return next_; }
			Nil next(Block* b) { next_ = b; }
			Int size() { return size_; }
			UShort count() { return count_; }
			Nil dump()
			{
				xprint("	[MemPool] Block, size=%d, count=%d, used=%d, first=%d. start=%llx, to=%llx.", size_, count_, used_, first_, data_, data_ + count_ * size_);
			}
		};

		class BlockArray
		{
		private:
			Block* block_;
			Mutex mutex_;
			Block* newBlock(Int size, UShort count)
			{
				Byte* data = (Byte*)malloc(size * count);
				xthrow(data == null,	error::OutOfMemory, "Can not malloc %db memories.", size * count);
				Block* b = new Block(data, size, count);
				xthrow(b == null,		error::OutOfMemory, "Can not malloc new Block.");
				b->next(block_);
				block_ = b;
				return b;
			}
		public:
			BlockArray():block_(null) {}
			~BlockArray()
			{
				if (!block_)return;
				Block* b = block_;
				block_ = null;
				while (b != null)
				{
					Block* temp = b;
					b = b->next();
					delete temp;
				}
			}
			Nil init(Int size, UShort count, UShort initCount)
			{
				for (UShort i = 0; i < initCount; i++)
				{
					newBlock(size, count);
				}
			}
			Int size() { return block_->size(); }
			UShort count() { return block_->count(); }
			Byte* alloc()
			{
				xlock_mutex(&mutex_);
				Block* b = block_;
				while (b != null)
				{
					if (b->available())
					{
						//xprint("[MemPool] alloc from size=%d, count=%d", b->size(), b->count());
						return b->alloc();
					}	
					b = b->next();
				}
				Int size = block_->size();
				UShort count = block_->count();
				b = newBlock(size, count);
				return b->alloc();
			}
			Boolean dealloc(Object data)
			{
				xlock_mutex(&mutex_);
				Block* b = block_;
				while (b != null)
				{
					if (b->contains(data))
					{
						b->dealloc(data);
						xprint("[MemPool] dealloc from size=%d, count=%d.", b->size(), b->count());
						return true;
					}
					b = b->next();
				}
				return false;
			}
			Nil dump()
			{
				Int count = 0;
				Block* b = block_;
				while (b != null)
				{
					b->dump();
					count++;
					b = b->next();
				}
				xprint("[MemPool] BlockArray count=%d, size=%d.", count, size());

			}
		};
	private:
		Int count_;
		BlockArray* blocks_;
	public:
		MemoryPool() :blocks_(null), count_(0) {}
		static MemoryPool* pool()
		{
			static MemoryPool i;
			return &i;
		}
		Nil init(const Int* sizes, const UShort* allocNumPatch, const UShort* initCount, Int count)
		{
			if (blocks_ != null)return;
			count_ = count;
			blocks_ = new BlockArray[count];
			for (Int i = 0; i < count; i++)
			{
				blocks_[i].init(sizes[i], allocNumPatch[i], initCount[i]);
			}
		}
		Nil clear()
		{
			if (blocks_ == null)return;
			delete[] blocks_;
			blocks_ = null;
		}
		Byte* alloc(Int size)
		{
			if (blocks_)
			{
				for (Int i = 0; i < count_; i++)
				{
					if (blocks_[i].size() >= size)
						return blocks_[i].alloc();
				}
			}
			xprint("[MemPool] alloc from std-malloc, size=%d.", size);
			return (Byte*)malloc(size);
		}
		Int align(Int size)
		{
			if (blocks_)
			{
				for (Int i = 0; i < count_; i++)
				{
					if (blocks_[i].size() >= size)
						return blocks_[i].size();
				}
			}
			return size;
		}
		Nil dealloc(Object ptr)
		{
			if (blocks_)
			{
				for (Int i = 0; i < count_; i++)
				{
					if (blocks_[i].dealloc(ptr))
						return;
				}
			}
			
			xprint("[MemPool] dealloc from std-free, ptr=%x", ptr);
			free(ptr);
		}
		Nil dump()
		{
			for (Int i = 0; i < count_; i++)
			{
				blocks_[i].dump();
			}
		}
	};
}