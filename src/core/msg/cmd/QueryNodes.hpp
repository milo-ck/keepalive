#pragma once
#include "core/msg/cmd/IBufferCommand.h"
#include "core/m/OnBuffer.hpp"

namespace ka
{
	namespace msg
	{
		namespace cmd
		{
			class QueryNodes : public IBufferCommand
			{
			public:
				QueryNodes(IMessageReader* reader, DataStore* store, m::OnBuffer* recvbuf)
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
				UInt  writeBody(xlib::StreamWriter* writer)
				{
					Int pos = writer->stream()->position();

					QueryReader query(reader_);
					query.read();
					data::Group* g = query.obtainGroup();
					if (g)
					{
						data::Device* d = query.obtainDevice(g);
						if (d)
						{
							writeNodes(g, d, writer, &query);
							return writer->stream()->position() - pos;
						}
						data::Iterator<data::Device> it(g->devices);
						while (it.next())
						{
							if (query.contain(it.current))
							{
								writeNodes(g, it.current, writer, &query);
							}
						}
						return writer->stream()->position() - pos;
					}
					return writer->stream()->position() - pos;
				}
				Nil writeNodes(data::Group* g, data::Device* d, xlib::StreamWriter* writer, QueryReader* query)
				{
					data::Iterator<data::Node> it(d->nodes);
					while (it.next())
					{
						if (!query->contain(it.current))
							continue;
						data::Node* n = it.current;
						UShort len = strlen(g->name) + 1;
						len += strlen(d->name) + 1;
						len += (n->name ? strlen(n->name) : 0) + 1;
						len += sizeof(ka::id);
						len += sizeof(UInt);
						len += sizeof(UInt);
						len += sizeof(UInt);
						writer->writeUShort(len);
						//len += 

						writer->writeString(g->name);
						writer->writeString(d->name);
						writer->writeString(n->name);

						ka::id id = data::u::makeID(g->id, d->id, n->id);
						writer->writeUInt64(id);
						writer->writeUInt(n->flags);
						writer->writeUInt(n->updateTime);
						writer->writeUInt(n->createTime);
					}
				}
			};
		}
	}
}
