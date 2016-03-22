#pragma once
#include "core/kastd.h"
#include "core/msg/Header.h"
#include "core/msg/QueryReader.hpp"
#include "core/msg/DataStore.hpp"

namespace ka
{
	namespace msg
	{
		class IMessageReader : public core::AutoRef
		{
		public:
			static IMessageReader* create(xlib::StreamReader* reader, DataStore* store);

			virtual const Header*			header() = 0;
			virtual const QueryReader*		query() = 0;
			virtual const Authentication*	authentication() = 0;
			virtual Int read(Byte buff[], Int len) = 0;
			virtual Int byteAvaliable() = 0;


			virtual xlib::StreamReader* stream() = 0;
			virtual DataStore* store() = 0;
			virtual void gotoBodyPosition() = 0;
		};
	};
};


