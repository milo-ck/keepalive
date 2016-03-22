#pragma once
#include "core/data/IBuffer.h"
#include "core/msg/cmd/ICommand.h"
#include "core/msg/IMessageWriter.h"

#include "core/m/OnNodeClosed.hpp"
#include "core/m/OnBuffer.hpp"

#include "core/msg/DataStore.hpp"
#include "core/msg/data/RequestPool.hpp"

namespace ka
{
	namespace msg
	{
		namespace cmd
		{
			class NodeClosed : public ICommand
			{
			private:
				m::OnNodeClosed* onNodeClosed_;
				DataStore* store_;
			public:
				NodeClosed(m::OnNodeClosed* onNodeClosed, DataStore* store):onNodeClosed_(onNodeClosed), store_(store)
				{

				}
				virtual bool execute()
				{
					data::Node* node = onNodeClosed_->node();
					if (node)
					{
						data::RequestPool* pool = data::RequestPool::get(node);
						if (pool)
						{
							Int len = pool->length();
							for (Int  i = len - 1; i >= 0; i--)
							{
								data::Node* from = store_->getNode(pool->fromAt(i));
								if (!from)continue;
							
								ka::id id = pool->idAt(i);
								data::IBuffer* buff = data::IBuffer::create(1024);
								Header h;
								utils::zore(&h);
								h.id = id;
								h.from = node->id;
								h.to = from->id;
								h.flags = HeaderFlags::IsSingleTarget | HeaderFlags::IsResponse;
								h.type = def::TargetOffline;

								IMessageWriter* writer = IMessageWriter::create(buff->writer());
								writer->header(&h);
								writer->writeHeader();

								buff->ref();
								const core::Parameter* param = onNodeClosed_->p();
								m::OnBuffer sndbuf(param->toId, param->fromId, m::OnBuffer::ID, buff, from);
								sndbuf.post();

								buff->unref();
								writer->unref();
							}
						}
						core::u::unref(&node->sender);
						core::u::unref(&node->recver);
						core::u::unref(&node->requestor);
						store_->removeNode(node);
					}
					return true;
				}

			};
		}
	}
}
