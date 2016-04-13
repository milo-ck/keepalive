#pragma once
#pragma once
#include "core/kastd.h"
#include "core/m/Method.hpp"
namespace ka
{
	namespace m
	{
		class OnNodeClosed : public Method
		{
		public:
			enum { ID = NodeClosed, };
			OnNodeClosed(Byte fromId, Byte toId, data::Node* node) :Method(fromId, ID, toId, 0)
			{
				p_.p1 = node;
				p_.p2 = 0;
				p_.p3 = 0;
			};
			OnNodeClosed(const core::Parameter* p) : Method()
			{
				p_ = *p;
			};

			data::Node* node() { return (data::Node*)p_.p1; }
		};
	}
}
