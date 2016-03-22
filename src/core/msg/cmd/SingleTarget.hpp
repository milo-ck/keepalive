#pragma once
#include "core/msg/cmd/ICommand.h"
#include "core/m/OnBuffer.hpp"
#include "core/msg/data/RequestPool.hpp"

namespace ka
{
	namespace msg
	{
		namespace cmd
		{
			class SingleTarget : public IBufferCommand
			{
			public:
				SingleTarget(IMessageReader* reader, DataStore* store, m::OnBuffer* recvbuf)
					:IBufferCommand(reader, store, recvbuf)
				{

				}
				virtual bool execute()
				{
					const Header* header = reader_->header();
					
					if (header->flags.has(HeaderFlags::IsRequest))
					{
						data::Node* node = store_->getNode(header->from);
						data::RequestPool* pool = data::RequestPool::getOrNew(node);
						pool->push(header->id, header->from);
					}
					else if (header->flags.has(HeaderFlags::IsResponse))
					{
						data::Node* node = store_->getNode(header->from);
						data::RequestPool* pool = data::RequestPool::get(node);
						if(pool)
							pool->remove(header->id);
					}
					data::IBuffer* buff = data::IBuffer::create(recvbuf_->buffer()->length());
					bool bok = post(buff, header->to);
					buff->unref();
					return bok;
				}
			};
		}
	}
}
