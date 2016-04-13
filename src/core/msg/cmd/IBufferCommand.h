#pragma once
#include "core/kastd.h"
#include "core/msg/IMessageReader.h"
#include "core/msg/IMessageWriter.h"
#include "core/m/OnBuffer.hpp"

#include "core/msg/cmd/ICommand.h"

namespace ka
{
	namespace msg
	{
		namespace cmd
		{
			class IBufferCommand : ICommand
			{
			protected:
				IMessageReader* reader_;
				DataStore* store_;
				m::OnBuffer* recvbuf_;

				bool post(data::IBuffer* buff, ka::id to)
				{
					const core::Parameter* param = recvbuf_->p();
					m::OnBuffer sndbuf(param->toId, param->fromId, buff, recvbuf_->node());
					sndbuf.post();
				};

			public:
				IBufferCommand(IMessageReader* reader, DataStore* store, m::OnBuffer* recvbuf)
					: reader_(reader), store_(store), recvbuf_(recvbuf)
				{
				}
				virtual bool execute() = 0;

			};
		}
	}
}

