#pragma once
#include "core/kastd.h"
#include "core/Controller.hpp"

namespace ka
{
	namespace entry
	{
		class TCPEntry : public core::IModule
		{
		public:
			virtual Byte id() { return core::IDTCPEntry; };

		protected:
			virtual Nil execute(const core::Parameter* p)
			{
				/*switch (p->method)
				{
				case m::OnBuffer::ID:
				{
					m::OnBuffer buf(p);
					cmd::BufferParser parser;
					parser.onBuffer(buf, store_);
					break;
				}
				case m::OnNodeClosed::ID:
				{
					m::OnNodeClosed node(p);
					cmd::NodeClosed nodeClosed(&node, store_);
					nodeClosed.execute();
					break;
				}
				}*/
			};
		public:
			virtual Nil run(Int numThreads)
			{
				core::IModule::run(numThreads);
			}
		};
	};
};