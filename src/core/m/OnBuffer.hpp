#pragma once
#include "core/kastd.h"
#include "core/m/Method.hpp"
#include "core/data/IBuffer.h"
namespace ka
{
	namespace m
	{
		class OnBuffer : public Method
		{
		public:
			enum { ID = Buffer, };
			OnBuffer(Byte fromId, Byte toId, data::IBuffer* buffer, data::Node* node):Method(fromId, ID, toId, 0)
			{
				p_.p1 = buffer;
				p_.p2 = node;
				p_.p3 = 0;
			};
			OnBuffer(const core::Parameter* p) : Method()
			{
				p_ = *p;
			};

			data::IBuffer* buffer() { return (data::IBuffer*)p_.p1;}
			data::Node* node() { return (data::Node*)p_.p2; }

			virtual bool post()
			{
				if (Method::post())
				{
					data::IBuffer* buff = buffer();
					if (buff)buff->ref();
					return true;
				}
				return false;
			}
		};
	}
}
