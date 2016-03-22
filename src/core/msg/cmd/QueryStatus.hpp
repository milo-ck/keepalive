#pragma once
#include "core/msg/cmd/IBufferCommand.h"
#include "core/m/OnBuffer.hpp"

namespace ka
{
	namespace msg
	{
		namespace cmd
		{
			class QueryStatus : public IBufferCommand
			{
			public:
				QueryStatus(IMessageReader* reader, DataStore* store, m::OnBuffer* recvbuf)
					:IBufferCommand(reader, store, recvbuf)
				{

				}
				virtual bool execute()
				{
					Header h;
					const Header* header = reader_->header();
					utils::copy(&h, header);
					h.flags = HeaderFlags::IsSingleTarget | HeaderFlags::IsSystem;
					h.to = header->from;
					h.bodyType = def::Bin;
					h.from = 0;

					data::IBuffer* buff = data::IBuffer::create(1024);
					IMessageWriter* writer = IMessageWriter::create(buff->writer());
					writer->header(&h);
					writer->writeHeader();
					writer->setBodyLength(writeBody(buff->writer()));
					
					post(buff, h.to);

					buff->unref();
					writer->unref();
					return true;
				}
			private:
				UInt writeBody(xlib::StreamWriter* writer)
				{
					Int pos = writer->stream()->position();
					writer->writeUInt(store_->getTotalNodes());
					writer->writeUInt(store_->getAliveNodes());
					return writer->stream()->position() - pos;
				}
			};
		}
	}
}
