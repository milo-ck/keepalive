#pragma once
#include "xlib/xlib.h"
#include "core/kastd.h"
#include "core/msg/Header.h"
#include "core/msg/QueryWriter.hpp"

namespace ka
{
	namespace msg
	{
		class IMessageWriter : public core::AutoRef
		{
		public:
			static IMessageWriter* create(xlib::StreamWriter* writer);
			virtual Header* header(const Header* from = 0) = 0;
			virtual Authentication* authentication() = 0;
			virtual Int writeHeader() = 0;
			

			virtual Int writeAuthentication() = 0;
			virtual const QueryWriter* query() = 0;
			virtual Int write(Byte buff[], Int len) = 0;

			virtual Int setBodyLength(UInt len) = 0;

			virtual xlib::StreamWriter* stream() = 0;
		};
	};
};

