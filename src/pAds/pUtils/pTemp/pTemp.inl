////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2011 Plustar Corporation. All rights reserved. プラスター
//
// ┌──┐┌┐    ┌┐┌┐┌──┐┌──┐┌──┐┌──┐
// └─┐│││    │││││┌─┘└┐┌┘│┌┐││┌┐│
// ┌─┘│││    │││││└─┐  ││  │└┘││└┴┘
// │┌─┘││    ││││└─┐│  ││  │┌┐││┌┬┐
// ││    │└─┐│└┘│┌─┘│  ││  ││││││││
// └┘    └──┘└──┘└──┘  └┘  └┘└┘└┘└┘
//
// Powered by jobscale
// Since 22 July 1973
//

CRITICAL_SECTION _cr;

int MemInitialize()
{
	InitializeCriticalSection(&_cr);

	for (int i = 0; _memData[i].name; i++)
	{
		char* p = (char*)_memData[i].org;
		int j = 0;
		for (; memcmp(&p[j], P("a-IR.o_F#ey,OU"), 14) != 0; j++)
		{
			p[j]^=87;
		}
		p[j] = 0;
		_memData[i].length = j;
	}

	return 0;
}

static int _memInitialize = MemInitialize();

struct TH_DATA
{
	DWORD th;
	const char* fname;

	const char* pos;
	int length;

	TH_DATA* next;
};

TH_DATA* _th_first = NULL;

MEM_DATA* GetMemData(const char* fname)
{
	for (int i = 0; _memData[i].name; i++)
	{
		if (memcmp(_memData[i].name, fname, strlen(fname) + 1) == 0)
		{
			return &_memData[i];
		}
	}

	return NULL;
}

TH_DATA* GetThData(const char* fname)
{
	DWORD th = GetCurrentThreadId();

	TH_DATA* pTarget = NULL;
	TH_DATA* pPrev = NULL;
	for (pTarget = _th_first; pTarget; pTarget = pTarget->next)
	{
		if (pTarget->th == th && strcmp(pTarget->fname, fname) == 0)
		{
			break;
		}
		pPrev = pTarget;
	}
	if (!pTarget)
	{
		const char* fname_ = 0;
		const char* pos_ = 0;
		int length_ = 0;
		for (int i = 0; _memData[i].name; i++)
		{
			if (memcmp(fname, _memData[i].name, strlen(_memData[i].name) + 1) == 0)
			{
				fname_ = _memData[i].name;
				pos_ = _memData[i].org;
				length_ = _memData[i].length;
				break;
			}
		}

		if (!length_)
		{
			return (TH_DATA*)-1; // 存在しない
		}

		pTarget = new TH_DATA;
		pTarget->th = th;
		pTarget->fname = fname_;
		pTarget->pos = pos_;
		pTarget->length = length_;
		pTarget->next = NULL;

		if (_th_first)
		{
			pTarget->next = _th_first;
		}
		_th_first = pTarget;
	}

	if (pTarget->length == 0)
	{
		if (pTarget == _th_first)
		{
			_th_first = pTarget->next;
		}
		else if (pPrev)
		{
			pPrev->next = pTarget->next;
		}
		delete pTarget;

		return NULL; // 終了
	}

	return pTarget;
}

char* template_fgets(char* _Buf, const int _MaxCount, FILE* _File, const char* fname)
{
	EnterCriticalSection(&_cr);

	TH_DATA* pTarget = GetThData(fname);

	LeaveCriticalSection(&_cr);

	if (pTarget == (TH_DATA*)-1)
	{
		return (char*)-1;
	}

	if (!pTarget)
	{
		return NULL;
	}

	const char* eol = strstr(pTarget->pos, "\n");
	int line_length = eol ? (eol - pTarget->pos + 1) : pTarget->length;

	if (line_length >= _MaxCount)
	{
		line_length = _MaxCount - 1;
	}

	memcpy(_Buf, pTarget->pos, line_length);
	_Buf[line_length] = NULL;

	if (pTarget->length > line_length)
	{
		pTarget->pos += line_length;
		pTarget->length -= line_length;
	}
	else
	{
		pTarget->pos = 0;
		pTarget->length = 0;
	}

	return _Buf;
}

int template_load_length(const char* fname, int* plength)
{
	EnterCriticalSection(&_cr);

	MEM_DATA* pTarget = GetMemData(fname);

	LeaveCriticalSection(&_cr);

	if (!pTarget)
	{
		return 3;
	}

	*plength = pTarget->length;

	return 0;
}

int template_load_content(const char* fname, char* content)
{
	EnterCriticalSection(&_cr);

	MEM_DATA* pTarget = GetMemData(fname);

	LeaveCriticalSection(&_cr);

	if (!pTarget)
	{
		return 3;
	}

	memcpy(content, pTarget->org, pTarget->length + 1);

	return 0;
}
