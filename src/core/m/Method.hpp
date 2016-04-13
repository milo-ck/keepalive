#pragma once
#include "core/Controller.hpp"
namespace ka
{
	namespace m
	{
		class Method
		{
		protected:
			core::Parameter p_;
			enum IDS
			{
				Buffer,
				NodeClosed,
			};
		public:
			Method() {}
			Method(Byte fromId, Byte toId, Byte method, Byte reserve)
			{
				p_.fromId = fromId;
				p_.toId = toId;
				p_.method = method;
				p_.reserve = reserve;
			}
			const core::Parameter* p() { return &p_; }
			virtual bool post() 
			{
				return core::Controller::put(p());
			}
		};
	}
}

