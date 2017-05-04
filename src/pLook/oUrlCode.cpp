///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2010 Plustar Corporation. All rights reserved. プラスター
// Powered by jobscale
// Since 22 July 1973
// Area in osaka, Japan
//

#define _WIN32_WINNT 0x0502
#define WINVER 0x0502

#ifdef _AFXDLL
#include <afxwin.h>
#else
#include <atlcomtime.h>
#endif

#include <string>

int hexToInt(char c)
{
    if ('0' <= c && c <= '9') { return c - '0'; }
	else if ('a' <= c && c <= 'f') { return c - 'a' + 0x0a; }
	else if ('A' <= c && c <= 'F') { return c - 'A' + 0x0a; }
	else { return -1; }
}

char intToHex(int x)
{
    if (0 <= x && x <= 9) { return x + '0'; }
	else if (10 <= x && x <= 15) { return x - 10 + 'A'; }
	else { return '\0'; }
}

int encodeUrl(char* outBuf, const char* str)
{
	char* out = outBuf;
    for (const char* it = str; *it; it++)
	{
        char c = *it;
        if (c == ' ')
		{
            *out = '+';
			out++;
        }
		else if (('A' <= c && c <= 'Z') ||  ('a' <= c && c <= 'z') || ('0' <= c && c <= '9') ||
				(c == '@') || (c == '*') || (c == '-') || (c == '.') || (c == '_'))
		{
            *out = c;
			out++;
        }
		else
		{
            *out = '%';
			out++;
            *out = intToHex((c >> 4) & 0x0f);
			out++;
            *out = intToHex(c & 0x0f);
			out++;
        }
    }
	*out = 0;
	return 0;
}

int decodeUrl(char* outBuf, const char* str)
{
	char* out = outBuf;
    for (const char* it = str; *it; it++)
	{
        char c = *it;
        if (*it == '+')
		{
            *out = ' ';
            out++;
        }
		else if (('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z') || ('0' <= c && c <= '9') ||
				(c == '@') || (c == '*') || (c == '-') || (c == '.') || (c == '_'))
		{
            *out = c;
            out++;
        }
		else if (c == '%')
		{
            int a = 0;
            for (int i = 0; i < 2; i++)
			{
                it++;
                if (*it == 0)
				{
					*out = 0;
                    return -1;
                }
                int b = hexToInt(*it);
                if (b == -1)
				{
					*out = 0;
                    return -1;
                }
                a = (a << 4) + b;
            }
            *out = static_cast<char>(a);
            out++;
        }
		else
		{
            *out = c;
            out++;
        }
    }
	*out = 0;
	return 0;
}

int hex2int(char *pChars)
{
	int Hi;
	int Lo;
	int Result;

	Hi = pChars[0];
	if ('0' <= Hi && Hi <= '9') {
		Hi-='0';
	} else if ('a' <= Hi && Hi <= 'f') {
		Hi-=('a'-10);
	} else if ('A' <= Hi && Hi <= 'F') {
		Hi-=('A' - 10);
	}
	Lo = pChars[1];
	if ('0' <= Lo && Lo <= '9') {
		Lo -= '0';
	} else if ('a' <= Lo && Lo <= 'f') {
		Lo -= ('a'-10);
	} else if ('A' <= Lo && Lo <= 'F') {
		Lo -= ('A'-10);
	}
	Result = Lo + (16 * Hi);
	return (Result);
}

void decodeurl(char *pEncoded)
{
	char* pDecoded = pEncoded;
	while (*pDecoded) {
		if (*pDecoded == '+') *pDecoded=' ';
		pDecoded++;
	};
	pDecoded = pEncoded;
	while (*pEncoded) {
		if (*pEncoded == '%')
		{
			pEncoded++;
			if (pEncoded[0])
			{
				if (pEncoded[1])
				{
					if (isxdigit(pEncoded[0])&&isxdigit(pEncoded[1]))
					{
						// *pDecoded++=(char)hex2int((char *)pEncoded);
						// Special Plustar - escape ampersands and slashes.
						// Note: hex characters are 3 bytes, we are only adding 2, so no
						// problems with buffer overflows.
						*pDecoded = (char)hex2int(pEncoded);
						if (*pDecoded == '&')
						{
							*pDecoded++='\\';
							*pDecoded='&';
						}
						else if (*pDecoded == '\\')
						{
							*pDecoded++ = '\\';
							*pDecoded = '\\';
						}
						else if (*pDecoded == '=')
						{
							*pDecoded++ = '\\';
							*pDecoded = '=';
						}
						pDecoded++;
						// End changes

						pEncoded += 2;
					}
				} else {
					break;
				}
			} else {
				break;
			}
		} else {
			*pDecoded++ = *pEncoded++;
		}
	}
	*pDecoded = '\0';
}
