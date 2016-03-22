#pragma once
#include "core/msg/cmd/IBufferCommand.h"
#include "core/m/OnBuffer.hpp"

namespace ka
{
	namespace msg
	{
		namespace cmd
		{
			class MultiTarget : public IBufferCommand
			{
			public:
				MultiTarget(IMessageReader* reader, DataStore* store, m::OnBuffer* recvbuf)
					:IBufferCommand(reader, store, recvbuf)
				{

				}
				virtual bool execute()
				{
					const QueryReader* query = reader_->query();

					data::Group* g = query->obtainGroup();
					if (g)
					{
						data::Device* d = query->obtainDevice(g);
						if (d)
						{
							procMultiNode(g, d);
							return true;
						}
						data::Iterator<data::Device> it(g->devices);
						while (it.next())
						{
							if (query->contain(it.current))
							{
								procMultiNode(g, it.current);
							}
						}
						return true;
					}

					data::Iterator<data::Group> itg(store_->getGroups());
					while (itg.next())
					{
						if (!query->contain(itg.current))
							continue;
						data::Iterator<data::Device> it(itg.current->devices);
						while (it.next())
						{
							if (query->contain(it.current))
								continue;
							procMultiNode(itg.current, it.current);
						}
					}
					return true;
				}
			private:
				Nil procMultiNode(data::Group* g, data::Device* d)
				{
					const QueryReader* query = reader_->query();
					const Header* header = reader_->header();
					

					data::Iterator<data::Node> it(d->nodes);
					Header h;
					while (it.next())
					{
						if (!query->contain(it.current))
							continue;
						data::IBuffer* buff = data::IBuffer::create(1024 + header->bodyLength);
						IMessageWriter* writer = IMessageWriter::create(buff->writer());

						utils::copy(&h, header);
						h.flags = HeaderFlags::IsSingleTarget;
						h.to = makeID(g->id, d->id, it.current->id);
						writer->writeHeader();
						writeBody(header, writer);
						post(buff, h.to);
						buff->unref();
						writer->unref();
					}
				}
				Nil writeBody(const Header* header, IMessageWriter* writer)
				{
					const Header* header = reader_->header();
					if(header->bodyLength <= 0)return;
					reader_->gotoBodyPosition();
					Byte bytes[1024];
					while (reader_->byteAvaliable() > 0)
					{
						Int num = reader_->read(bytes, 1024);
						writer->write(bytes, num);
					}
				}

			};
		}
	}
}
