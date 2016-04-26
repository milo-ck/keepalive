#pragma once
#include "xlib/types_.h"
#include "xlib/xlib.u_.h"


#include "xlib/Exception.hpp"
#include "xlib/URI.hpp"
#include "xlib/Mutex.hpp"
#include "xlib/MutexSignal.hpp"
#include "xlib/Log.hpp"
#include "xlib/Counter.hpp"
#include "xlib/allocator.hpp"
#include "xlib/MemoryStream.hpp"
#include "xlib/FileStream.hpp"
#include "xlib/StreamReader.hpp"
#include "xlib/StreamWriter.hpp"
#include "xlib/Thread.hpp"

#ifdef XPLATFORM_WIN
	#include "xlib/DataEntryWin.hpp"
#else
	#include "xlib/DataEntryLinux.hpp"
#endif
