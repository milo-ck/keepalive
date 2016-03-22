#pragma once
#include "core/kastd.h"
namespace ka
{
	namespace data
	{
		class IBuffer : public core::AutoRef
		{
		public:
			static IBuffer* create(Int len);

			virtual xlib::StreamReader* reader() = 0;
			virtual xlib::StreamWriter* writer() = 0;

			
			virtual Int length() = 0;
			virtual Int position() = 0;
			virtual Boolean position(Int pos) = 0;
		};
	};
};
