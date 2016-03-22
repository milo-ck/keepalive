#include "xlib/xlib.h"
namespace ka
{
	namespace core
	{
		namespace u
		{
			template<typename T>
			inline Nil unref(T** pautoref)
			{
				Nil* autoref = *pautoref;
				*pautoref = 0;
				if (autoref)
					((core::AutoRef*)autoref)->unref();
			};
		}
	}
}