#pragma once
#include "core/kastd.h"

#include "core/msg/IMessageReader.h"
#include "core/msg/IMessageWriter.h"

#include "core/data/IBuffer.h"
#include "core/m/OnBuffer.hpp"

#include "core/msg/cmd/Login.hpp"
#include "core/msg/cmd/MultiTarget.hpp"
#include "core/msg/cmd/SingleTarget.hpp"

#include "core/msg/cmd/QueryNodes.hpp"
#include "core/msg/cmd/QueryOwnerID.hpp"
#include "core/msg/cmd/QueryStatus.hpp"
#include "core/msg/cmd/KeepAlive.hpp"

namespace ka
{
namespace msg
{
namespace cmd
{
	class BufferParser
	{
	public:
		Nil onBuffer(m::OnBuffer& buf, DataStore* store)
		{
			data::IBuffer* buffer = buf.buffer();
			buffer->position(0);

			IMessageReader* reader = IMessageReader::create(buffer->reader(), store);

			const Header* header = reader->header();

			Login login(reader, store, &buf);
			if (!login.execute())
			{
				buffer->unref();
				reader->unref();
				return;
			}
			
			if (header->flags.has(HeaderFlags::IsMultiTarget))
			{
				MultiTarget mt(reader, store, &buf);
				mt.execute();
			}
			else if(header->flags.has(HeaderFlags::IsSystem))
			{
				switch(header->type)
				{
					case def::KeepAlive:
						{
							KeepAlive kalive(reader, store, &buf);
							kalive.execute();
							break;
						}
					case def::QueryID:
						{
							QueryOwnerID qoid(reader, store, &buf);
							qoid.execute();
							break;
						}
					case def::QueryNodes:
						{
							QueryNodes qnodes(reader, store, &buf);
							qnodes.execute();
							break;
						}
					case def::QueryStatus:
						{
							QueryStatus qstatus(reader, store, &buf);
							qstatus.execute();
							break;
						}
				}
			}
			else
			{
				SingleTarget st(reader, store, &buf);
				st.execute();
			}
			buffer->unref();
			reader->unref();
		}
	};
}
}
}