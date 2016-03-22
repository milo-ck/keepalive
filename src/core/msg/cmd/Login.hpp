#pragma once
#include "core/msg/cmd/IBufferCommand.h"
#include "core/m/OnBuffer.hpp"

namespace ka
{
	namespace msg
	{
		namespace cmd
		{
			class Login : public IBufferCommand
			{
			public:
				Login(IMessageReader* reader, DataStore* store, m::OnBuffer* recvbuf)
					:IBufferCommand(reader, store, recvbuf)
				{

				}
				virtual bool execute()
				{
					const Header* header = reader_->header();
					if (!header->flags.has(HeaderFlags::IsUnlogin))
					{
						return true;
					}
					const Authentication* auth = reader_->authentication();
					if (!login(auth))
						return false;
					data::Group* g = store_->addGroup(auth->group.c_str());
					data::Device* d = store_->addDevice(g, auth->device.c_str());
					data::Node* n = store_->addNode(d, recvbuf_->node(), auth->node.c_str(), auth->nodeFlags);
					if (!g || !d || !n)
					{
						return false;
					}
					reader_->header()->setFrom(makeID(g->id, d->id, n->id));
					return true;
				}
			private:
				bool login(const Authentication* auth)
				{
					return true;
				}

			};
		}
	}
}
