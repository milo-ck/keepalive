#pragma once
#include "xlib/types_.h"
namespace xlib
{
	class URI
	{
	private:
		static unsigned char toHex(unsigned char x)
		{
			return (unsigned char)(x > 9 ? x + 55 : x + 48);
		}
		static int isANC(unsigned char c)
		{
			if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
				return 1;
			return 0;
		}
	public:
		void encode(const Char* src,String& result)
		{
			int  len = strlen(src);
			for (Int i = 0; i < len; i++)
			{
				Char ch = src[i];
				if (ch == ' ')
				{
					result  += '+';
				}
				else if (isANC(ch) || strchr("=!~*'()", ch))
				{
					result += ch;
				}
				else
				{
					result += '%';
					result += toHex((unsigned char)(ch >> 4));
					result += toHex((unsigned char)(ch % 16));
				}
			}
		}
		void decode(const Char* src, String& result)
		{
			char p[2];
			unsigned int num;

			Int len = strlen(src);
			for (Int i = 0; i < len; i++)
			{
				memset(p, '/0', 2);
				if (src[i] != '%')
				{
					result = src[i];
					continue;
				}

				p[0] = src[++i];
				p[1] = src[++i];

				p[0] = p[0] - 48 - ((p[0] >= 'A') ? 7 : 0) - ((p[0] >= 'a') ? 32 : 0);
				p[1] = p[1] - 48 - ((p[1] >= 'A') ? 7 : 0) - ((p[1] >= 'a') ? 32 : 0);
				result += (p[0] * 16 + p[1]);

			}
		}
	};
}
