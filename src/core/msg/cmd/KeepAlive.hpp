#pragma once
#include "core/msg/cmd/IBufferCommand.h"
#include "core/m/OnBuffer.hpp"

namespace ka
{
	namespace msg
	{
		namespace cmd
		{
			class KeepAlive : public IBufferCommand
			{
			public:
				KeepAlive(IMessageReader* reader, DataStore* store, m::OnBuffer* recvbuf)
					:IBufferCommand(reader, store, recvbuf)
				{

				}
				virtual bool execute()
				{
					return true;
				}
			};
		}
	}
}
