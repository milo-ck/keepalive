#pragma once
#include "xlib/types_.h"
namespace xlib
{
	namespace u
	{
		inline Boolean isLittleEndian()
		{
			union
			{
				Short s;
				Byte b[2];
			}a;
			a.s = 0x0102;
			return (a.b[0] == 2 && a.b[1] == 1);
		}
		inline Nil invertBytes(Byte* src, Int num)
		{
			Int len = num / 2;
			for (Int l = 0, r = num - 1; l < len; l++, r--)
			{
				Byte temp = src[r];
				src[r] = src[l];
				src[l] = temp;
			}
		}
		inline Boolean isWhiteSpace(Char c)
		{
			switch (c)
			{
			case ' ':
			case '\t':
			case '\f':
			case '\r':
			case '\n':
				return true;
			}
			return false;
		}
		enum TrimT
		{
			TrimLeft = 1,
			TrimRight = 2,
			TrimBoth = 3,
		};
		inline Char* trim(Char* s, TrimT t)
		{
			if (!s)return null;
			Int len = strlen(s) - 1;
			if ((t & 2))
			{
				while (len > 0)
				{
					if (!isWhiteSpace(s[len]))
						break;
					s[len] = '\0';
					len--;
				}
			}
			Char* ret = s;
			if ((t & 1))
			{
				for (Int i = 0; i < len; i++)
				{
					if (!isWhiteSpace(s[i]))
						break;
					ret++;
				}
			}
			return ret;
		}
		inline Boolean startWith(const Char* src, const Char* to)
		{
			if (!to || !src)return src == to;
			Int len = strlen(to);
			return strncmp(src, to, len) == 0;
		}
		inline Int toInt(const Char* s, Int def = 0)
		{
			Int ret = def;
			sscanf(s, "%d", ret);
			return ret;
		}
	}

}
