#pragma once
#include "core/kastd.h"

#include "core/msg/IMessageReader.h"
#include "core/msg/MessageReader.hpp"
#include "core/msg/IMessageWriter.h"
#include "core/msg/MessageWriter.hpp"

#include "core/data/IBuffer.h"
#include "core/Controller.hpp"
#include "core/m/OnBuffer.hpp"
#include "core/m/OnNodeClosed.hpp"

#include "core/msg/cmd/BufferParser.hpp"
#include "core/msg/cmd/NodeClosed.hpp"

namespace ka
{
	namespace msg
	{
		class Message : public core::IModule
		{
		private:
			DataStore* store_;
		public:
			virtual Byte id() { return core::IDMessage; };

		protected:
			virtual Nil execute(const core::Parameter* p)
			{
				switch (p->method)
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
				}
			};
		private:
		};
	};
};


