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

int urlEncode(char* outBuf, const char* str)
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

int urlDecode(char* outBuf, const char* str)
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
