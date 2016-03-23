#pragma once
#include "xlib/xlib.h"
#include <stdarg.h>
namespace ka
{
	class Config
	{
	public:
		struct Threads
		{
			Int message;
			Int tcp;
			Int http;
		};
		struct  Alloc
		{
			Int* size;
			UShort* num;
			UShort* initCount;
			UShort length;
		};
		struct Path
		{
			String temp;
		};
		struct Server
		{
			UShort port;
			UShort httpPort;
		};
	private:
		Threads threads_;
		Alloc alloc_;
		Path path_;
		Server server_;
		static Config* config()
		{
			static Config cnf;
			return &cnf;
		}
		Config()
		{
			memset(&alloc_, 0, sizeof(alloc_));
			threads_.http = threads_.message = threads_.tcp = 2;
			server_.port = server_.httpPort = 0;
			FILE* file = fopen("ka.cnf", "r");
			if (file != null)
			{
				load(file);
				fclose(file);
				if (!alloc_.initCount || !alloc_.num || !alloc_.size)
					alloc_.length = 0;
			}

		}
		template<typename T> 
		T* split(Char* s, T* def)
		{
			s = strstr(s, "=");
			Int len;
			if (s && (s = xlib::u::trim(s + 1, xlib::u::TrimBoth)))
			{
				len = 1;
				Char* temp = s;
				while (temp = strstr(temp, ","))
				{
					temp++;
					len++;
				}
				def = new T[len];
				len = strlen(s);
				temp = s;
				Int index = 0;
				for (Int i = 0; i < len; i++)
				{
					if (s[i] != ',')
						continue;
					s[i] = '\0';
					temp = xlib::u::trim(temp, xlib::u::TrimBoth);
					if (temp && strlen(temp) > 0)
					{
						def[index] = (T)(xlib::u::toInt(temp));
						index++;
					}
					temp = i != slen - 1 ? s[i + 1] : null;
				}
				if (temp && strlen(temp = xlib::u::trim(temp, xlib::u::TrimBoth)) > 0)
				{
					def[index] = (T)(xlib::u::toInt(temp));
					index++;
				}
				len = index;
			}
			if (len <= 0)
			{
				alloc_.length = 0;
				def = null;
			}
			else
			{
				alloc_.length = alloc_.length == 0 ? len : (alloc_.length > len ? alloc_.length : len);
			}
			return def;
			
		}
		Nil load(FILE* file)
		{
			Char line[4096];
			while (fgets(line, 4096, file))
			{
				Char* s = xlib::u::trim(line, xlib::u::TrimBoth);
				if (strlen(s) <= 0 || s[0] == '#')
					continue;
				if (xlib::u::startWith(s, "threads.tcp"))
				{
					s = strstr(s, "=");
					if (s)threads_.tcp = xlib::u::toInt(xlib::u::trim(s + 1, xlib::u::TrimBoth), threads_.tcp);
				}
				else if (xlib::u::startWith(s, "threads.http"))
				{
					s = strstr(s, "=");
					if (s)threads_.http = xlib::u::toInt(xlib::u::trim(s + 1, xlib::u::TrimBoth), threads_.http);
				}
				else if (xlib::u::startWith(s, "threads.message"))
				{
					s = strstr(s, "=");
					if (s)threads_.message = xlib::u::toInt(xlib::u::trim(s + 1, xlib::u::TrimBoth), threads_.message);
				}
				else if (xlib::u::startWith(s, "path.temp"))
				{
					s = strstr(s, "=");
					if (s)path_.temp = s + 1;
				}
				else if (xlib::u::startWith(s, "alloc.size"))
				{
					alloc_.size = split<Int>(s, alloc_.size);
				}
				else if (xlib::u::startWith(s, "alloc.num"))
				{
					alloc_.num = split<UShort>(s, alloc_.num);
				}
				else if (xlib::u::startWith(s, "alloc.initCount"))
				{
					alloc_.initCount = split<UShort>(s, alloc_.initCount);
				}
				else if(xlib::u::startWith(s, "server.port"))
				{
					s = strstr(s, "=");
					if (s)server_.port = (UShort)xlib::u::toInt(xlib::u::trim(s + 1, xlib::u::TrimBoth), server_.port);
				}
				else if (xlib::u::startWith(s, "server.httpPort"))
				{
					s = strstr(s, "=");
					if (s)server_.httpPort = xlib::u::toInt(xlib::u::trim(s + 1, xlib::u::TrimBoth), server_.httpPort);
				}
			}
		}
	public:
		
		static const Threads* threads()
		{
			return &config()->threads_;
		}
		static const Alloc* alloc()
		{
			return config()->alloc_.length > 0 ? &config()->alloc_ : null;
		}
		static const Path* path()
		{
			return &config()->path_;
		}
		static const Server* server()
		{
			return &config()->server_;
		}
		static String getTempPath(const Char* format, ...)
		{
			Char temp[1024];
			Char* allocated = null;
			va_list va;
			va_start(va, format);
			Int count = vprintf(format, va);
			if (count < 1020)
			{
				vsprintf(temp, format, va);
			}
			else
			{
				allocated = xlib::create<Char>(count + 1);
				if (allocated == 0)
				{
					va_end(va);
					xthrow(true, error::OutOfMemory, "Failed to get temp path.");
				}
				allocated[count] = '\0';
				vsprintf(allocated, format, va);
			}
			va_end(va);
			String ret = config()->path_.temp;
			if (allocated)
			{
				ret += allocated;
				xlib::dealloc(allocated);
			}
			else
			{
				ret += temp;
			}
			return ret;
		}
	};
}