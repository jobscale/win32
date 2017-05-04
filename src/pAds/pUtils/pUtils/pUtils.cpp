////////////////////////////////////////////////////////////////////////////
// Copyright 2005-2011 Plustar Corporation. All rights reserved. ƒvƒ‰ƒXƒ^[
//
// „¡„Ÿ„Ÿ„¢„¡„¢    „¡„¢„¡„¢„¡„Ÿ„Ÿ„¢„¡„Ÿ„Ÿ„¢„¡„Ÿ„Ÿ„¢„¡„Ÿ„Ÿ„¢
// „¤„Ÿ„¢„ „ „     „ „ „ „ „ „¡„Ÿ„£„¤„¢„¡„£„ „¡„¢„ „ „¡„¢„ 
// „¡„Ÿ„£„ „ „     „ „ „ „ „ „¤„Ÿ„¢  „ „   „ „¤„£„ „ „¤„¨„£
// „ „¡„Ÿ„£„ „     „ „ „ „ „¤„Ÿ„¢„   „ „   „ „¡„¢„ „ „¡„¦„¢
// „ „     „ „¤„Ÿ„¢„ „¤„£„ „¡„Ÿ„£„   „ „   „ „ „ „ „ „ „ „ 
// „¤„£    „¤„Ÿ„Ÿ„£„¤„Ÿ„Ÿ„£„¤„Ÿ„Ÿ„£  „¤„£  „¤„£„¤„£„¤„£„¤„£
//
// Powered by jobscale
// Since 22 July 1973
//

#define _WIN32_WINNT 0x0502
#define WINVER 0x0502

#ifdef _AFXDLL
#include <afxwin.h>
#else
#include <atlcomtime.h>
#endif

#include <wininet.h>
#pragma comment(lib, "wininet")

#define P(x) "\0 " x + 2

#include "../conf/adlWord.inl"
#include "../conf/blkWord.inl"

#include "../conf/config.inl"
#include "../conf/match-all.action.inl"
#include "../conf/default.action.inl"
#include "../conf/default.filter.inl"

#include "../../../oCommon/oTools.h"
#include "../../../oCommon/oSocketTools.h"
#include "../../../oCommon/oAtlHttpSocket.h"
#include "../../../oCommon/oCodeConv.h"

#if 0
#endif

/*
 * single byte character set
 *
 * P("sex ")
 * “Á‚É‰e‹¿‚È‚³‚»‚¤‚È‚Ì‚ÅAŠCŠO‚ÌƒAƒ_ƒ‹ƒgƒTƒCƒg‚Ìƒqƒbƒg‚ğ—Dæ‚·‚é
 *
 * ‰e‹¿‚·‚éƒƒWƒƒ[‚Èƒ[ƒh
 * —mŠy sex machine, ƒƒbƒNƒoƒ“ƒh sex pistols, ˆãŠw HIV (no sex ...), HIV (safer sex ...) etc...
 *
 *   |sex‚ ‚è|sex‚È‚µ|
 * --------------------
 *  0|  15294|  15303|
 *  5|      1|      1|
 *  8|   1497|   1491|
 *  9|     53|     53|
 * 10|    609|    609|
 * 11|    310|    310|
 * 13|    626|    712|
 * 15|   4666|   4671|
 * 19|   3998|   4095|
 * 21|    190|       |
 */

struct word_data
{
	int gravity;
	CComBSTR str;
};

word_data* MakeTrg(const char* pSrc, const char** pSrcAlfa)
{
	int targ_len = 1;
	for (const char* p = pSrc; p = strstr(p, "\n"); p++)
	{
		targ_len += 3;
	}

	for (int adl_i = 0; pSrcAlfa[adl_i]; adl_i++)
	{
		targ_len++;
	}

	word_data* targ = new word_data[targ_len + 1];

	int tarpos = 0;
	for (const char *p = pSrc, *s = pSrc; p = strstr(p, "\n"); s = ++p)
	{
		targ[tarpos].gravity = strtol(s, NULL, 10);
		targ[tarpos + 1].gravity = targ[tarpos].gravity;
		targ[tarpos + 2].gravity = targ[tarpos].gravity;

		s += 4;

		CString ss(s, p - s);
		if (ss.IsEmpty())
		{
			continue;
		}
		SJIStoUTF8(ss, targ[tarpos].str);
		SJIStoEUC(ss, targ[tarpos + 1].str);
		UTF8toSJIS((LPCSTR)targ[tarpos].str.m_str, targ[tarpos + 2].str);

		tarpos += 3;
	}

	for (int adl_i = 0; pSrcAlfa[adl_i]; adl_i++)
	{
		targ[tarpos].gravity = strtol(pSrcAlfa[adl_i], NULL, 10);

		int len = strlen(pSrcAlfa[adl_i] + 4);
		targ[tarpos].str.AppendBytes("", len + 1);
		memcpy((char*)targ[tarpos].str.m_str, pSrcAlfa[adl_i] + 4, len + 1);

		tarpos++;
	}

	return targ;
}

struct iob
{
   char *buf;    /**< Start of buffer        */
   char *cur;    /**< Start of relevant data */
   char *eod;    /**< End of relevant data   */
   size_t size;  /**< Size as malloc()ed     */
};

struct utl_trance
{
	char* dat;
	int len;
	void* reserve;
};

typedef void* (*pads_malloc_proc)(size_t size);
typedef void (*pads_free_proc)(void* _memory);
typedef char* (*pads_html_encode_proc)(const char *s);

struct utl_receive
{
	char** body;
	int* content_length;
	void* reserve;

	pads_malloc_proc pads_malloc;
	pads_free_proc pads_free;
	pads_html_encode_proc pads_html_encode;
};

struct http_request
{
   char *cmd;      /**< Whole command line: method, URL, Version */
   char *ocmd;     /**< Backup of original cmd for CLF logging */
   char *gpc;      /**< HTTP method: GET, POST, ... */
   char *url;      /**< The URL */
   char *ver;      /**< Protocol version */
   int status;     /**< HTTP Status */

   char *host;     /**< Host part of URL */
   int   port;     /**< Port of URL or 80 (default) */
   char *path;     /**< Path of URL */
   char *hostport; /**< host[:port] */
   int   ssl;      /**< Flag if protocol is https */

   char *host_ip_addr_str; /**< String with dotted decimal representation
                                of host's IP. NULL before connect_to() */

#ifndef FEATURE_EXTENDED_HOST_PATTERNS
   char  *dbuffer; /**< Buffer with '\0'-delimited domain name.           */
   char **dvec;    /**< List of pointers to the strings in dbuffer.       */
   int    dcount;  /**< How many parts to this domain? (length of dvec)   */
#endif /* ndef FEATURE_EXTENDED_HOST_PATTERNS */
};

typedef SOCKET jb_socket;

struct reusable_connection
{
   jb_socket sfd;
   int       in_use;
   time_t    timestamp; /* XXX: rename? */

   time_t    request_sent;
   time_t    response_received;

   /*
    * Number of seconds after which this
    * connection will no longer be reused.
    */
   unsigned int keep_alive_timeout;

   char *host;
   int  port;
   int  forwarder_type;
   char *gateway_host;
   int  gateway_port;
   char *forward_host;
   int  forward_port;
};

struct list_entry
{
   /**
    * The string pointer. It must point to a dynamically malloc()ed
    * string or be NULL for the list functions to work. In the latter
    * case, just be careful next time you iterate through the list in
    * your own code.
    */
   char *str;
   
   /** Next entry in the linked list, or NULL if no more. */
   struct list_entry *next;
};

struct list
{
   /** First entry in the list, or NULL if the list is empty. */
   struct list_entry *first;

   /** Last entry in the list, or NULL if the list is empty. */
   struct list_entry *last;
};

struct file_list
{
   /**
    * This is a pointer to the data structures associated with the file.
    * Read-only once the structure has been created.
    */
   void *f;

   /**
    * The unloader function.
    * Normally NULL.  When we are finished with file (i.e. when we have
    * loaded a new one), set to a pointer to an unloader function.
    * Unloader will be called by sweep() (called from main loop) when
    * all clients using this file are done.  This prevents threading
    * problems.
    */
   void (*unloader)(void *);

   /**
    * Used internally by sweep().  Do not access from elsewhere.
    */
   int active;

   /**
    * File last-modified time, so we can check if file has been changed.
    * Read-only once the structure has been created.
    */
   time_t lastmodified;
   
   /**
    * The full filename.
    */
   char * filename;

   /**
    * Pointer to next entry in the linked list of all "file_list"s.
    * This linked list is so that sweep() can navigate it.
    * Since sweep() can remove items from the list, we must be careful
    * to only access this value from main thread (when we know sweep
    * won't be running).
    */
   struct file_list *next;
};

#define LOG_LEVEL_FORCE 0x0020

typedef int regoff_t;

typedef struct {
  regoff_t rm_so;
  regoff_t rm_eo;
} regmatch_t;

typedef struct {
  void *re_pcre;
  size_t re_nsub;
  size_t re_erroffset;
} regex_t;

		typedef void (_cdecl *log_out_proc)(int loglevel, const char *fmt, ...);
		typedef int (*regcomp_proc)(regex_t* preg, const char* pattern, int cflags);
		typedef int (*regexec_proc)(regex_t* preg, const char* string, size_t nmatch, regmatch_t pmatch[], int eflags);

		struct putil_data
		{
			struct iob const* iob;
			struct http_request const* http;
			struct list const* headers;
			struct list const* tags;
			struct reusable_connection const* server_connection;

			struct file_list* const* actions_list;
			struct file_list* const* rlist;
			unsigned long long content_length;
			struct file_list* tlist;

			const char* hdr;
			const char* buf;

			log_out_proc log_out;
			regcomp_proc regcomp;
			regexec_proc regexec;
		};

struct plustar_head
{
	int title;
	int description;
	int kyeword;
};

struct plustar_page
{
	int type;

	int content_len;
	int charset;
	int body_len;
	int take_len;
	char* cur;

	struct plustar_head head;
};

int InitWakeup()
{

	return 0;
}

int SetInternetSettings()
{
	DWORD dwDisposition = 0;
	HKEY hkResult = NULL;
	if (ERROR_SUCCESS != RegCreateKeyEx(HKEY_CURRENT_USER, P("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"),
			NULL, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkResult, &dwDisposition))
	{
			return -1;
	}

	DWORD on = 1;

	if (ERROR_SUCCESS != RegSetValueEx(hkResult, P("EnableHttp1_1"),
		NULL, REG_DWORD, (LPBYTE)&on, sizeof(on)))
	{
	}

	if (ERROR_SUCCESS != RegSetValueEx(hkResult, P("ProxyHttp1.1"),
		NULL, REG_DWORD, (LPBYTE)&on, sizeof(on)))
	{
	}

	RegCloseKey(hkResult);

	return 0;
}

int SetInternetExplorerMain()
{
	DWORD dwDisposition = 0;
	HKEY hkResult = NULL;
	if (ERROR_SUCCESS != RegCreateKeyEx(HKEY_CURRENT_USER, P("Software\\Microsoft\\Internet Explorer\\Main"),
			NULL, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkResult, &dwDisposition))
	{
			return -1;
	}

	LPCTSTR yes = P("yes");
	LPCTSTR no = P("no");

	if (ERROR_SUCCESS != RegSetValueEx(hkResult, P("Disable Script Debugger"),
		NULL, REG_SZ, (LPBYTE)yes, strlen(yes)))
	{
	}

	if (ERROR_SUCCESS != RegSetValueEx(hkResult, P("DisableScriptDebuggerIE"),
		NULL, REG_SZ, (LPBYTE)yes, strlen(yes)))
	{
	}

	if (ERROR_SUCCESS != RegSetValueEx(hkResult, P("Error Dlg Displayed On Every Error"),
		NULL, REG_SZ, (LPBYTE)no, strlen(no)))
	{
	}

	RegCloseKey(hkResult);

	return 0;
}

int SetInternetSecurity()
{
	DWORD dwDisposition = 0;
	HKEY hkResult = NULL;
	DWORD security = 2;

	if (ERROR_SUCCESS != RegCreateKeyEx(HKEY_CURRENT_USER, P("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\ZoneMap\\Domains\\plustar.jp\\pad"),
			NULL, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkResult, &dwDisposition))
	{
			return -1;
	}

	if (ERROR_SUCCESS != RegSetValueEx(hkResult, P("http"),
		NULL, REG_DWORD, (LPBYTE)&security, sizeof(security)))
	{
	}

	RegCloseKey(hkResult);

	if (ERROR_SUCCESS != RegCreateKeyEx(HKEY_CURRENT_USER, P("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings\\ZoneMap\\Domains\\plustar.jp\\x"),
			NULL, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkResult, &dwDisposition))
	{
			return -1;
	}

	if (ERROR_SUCCESS != RegSetValueEx(hkResult, P("http"),
		NULL, REG_DWORD, (LPBYTE)&security, sizeof(security)))
	{
	}

	RegCloseKey(hkResult);

	return 0;
}

LPCTSTR StartPage(LPCTSTR startPage = NULL)
{
	static CComBSTR* sp = new CComBSTR("");
	if (startPage)
	{
		size_t length = strlen(startPage);
		length++;
		sp->AppendBytes("", length);
		memcpy((LPTSTR)sp->m_str, startPage, length);
	}

	return (LPCTSTR)sp->m_str;
}

int GetStartPage()
{
	const char* pStartPage = "Software\\Microsoft\\Internet Explorer\\Main";

	HKEY hKey;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, pStartPage, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		return -1;
	}

	DWORD dwType;
	DWORD dwSize = 5000;
	CComBSTR value;
	value.AppendBytes("", dwSize);
	if (RegQueryValueEx(hKey, "Start Page", 0, &dwType, (LPBYTE)value.m_str, &dwSize) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return -1;
	}

	RegCloseKey(hKey);

	StartPage((LPCTSTR)value.m_str);

	return 0;
}

int SearchKey(CString& search_key)
{
	static CString _search_key;

	if (search_key.IsEmpty())
	{
#ifndef _DEBUG
		search_key = _search_key;
#endif
	}
	else
	{
		_search_key = search_key;
	}

	return 0;
}

#define MIXI_FAILED_PAGE 1
char* real_get(char* str, const char* substr)
{
#if MIXI_FAILED_PAGE
	return strstr(str, substr);
#endif
	char* ret = strstr(str, substr);
	if (!ret)
	{
		return ret;
	}

	char* dum_start = strstr(str, "\"");
	char* dum_end = dum_start ? strstr(dum_start + 1, "\"") : 0;

	if (dum_end && ret > dum_start)
	{
		return real_get(dum_end + 1, substr);
	}

	return ret;
}

int DeleteTag(char* real, const char* st, const char* ed)
{
	int result = 0;

	char* dup = 0;
	int length = 0;

	char* ena = real;
	for (; ; )
	{
		char* start = real_get(ena, st);
		if (!start)
		{
			break;
		}

		char* end = real_get(start + strlen(st), ed);
		if (!end)
		{
			break;
		}

		if (!dup)
		{
			dup = (char*)malloc(strlen(real));
		}

		result++;

		end += strlen(ed);
		int len = end - start;
		int enalen = start - ena;
		if (!enalen)
		{
			ena += len;
			continue;
		}

		memcpy(dup + length, ena, enalen);
		length += enalen;
		ena = end;
	}

	if (dup)
	{
		if (length)
		{
			memcpy(real, dup, length);
		}

		memcpy(real + length, ena, strlen(ena));
		real[length + strlen(ena)] = 0;

		free(dup);
	}

	return result;
}

int RemoveWord(char* real, const char* st)
{
	int result = 0;

	char* dup = 0;
	int length = 0;

	char* ena = real;
	for (; ; )
	{
		char* start = strstr(ena, st);
		if (!start)
		{
			break;
		}

		char* end = start + strlen(st);

		if (!dup)
		{
			dup = (char*)malloc(strlen(real));
		}

		result++;

		int len = end - start;
		int enalen = start - ena;
		if (!enalen)
		{
			ena += len;
			continue;
		}

		memcpy(dup + length, ena, enalen);
		length += enalen;
		ena = end;
	}

	if (dup)
	{
		if (length)
		{
			memcpy(real, dup, length);
		}

		memcpy(real + length, ena, strlen(ena));
		real[length + strlen(ena)] = 0;

		free(dup);
	}

	return result;
}

int DeleteTagSkeleton(char* real)
{
	int result = 0;

	// ˆÍ‚¢ƒ^ƒO
	result += DeleteTag(real, "<script", "/script>");
	result += DeleteTag(real, "<noscript", "/noscript>");
	result += DeleteTag(real, "<style", "/style>");
	result += DeleteTag(real, "<embed", "/embed>");
	result += DeleteTag(real, "<noembed", "/noembed>");
	result += DeleteTag(real, "<object", "/object>");
	result += DeleteTag(real, "<iframe ", "/iframe>");
	result += DeleteTag(real, "<frameset", "/frameset>");
	result += DeleteTag(real, "<!--", "-->");

	// youtube.com ‘Î‰
	result += DeleteTag(real, "<button", ">");
	result += DeleteTag(real, "</button", ">");

	result += DeleteTag(real, "<input ", ">");

	/*
	 * ˆÓŠO‚Æselect‚ÍƒTƒCƒYH‚¤
	 * <select>
	 *  <option value="/contact.php?t=booksearch_ques" selected>‚¨–â‚¢‡‚í‚¹</option>
	 * </select>
	 */
	result += DeleteTag(real, "<select ", "/select>");

	return result;
}

int RemoveWordTrush(char* real)
{
	int result = 0;

	result += RemoveWord(real, " ");
	result += RemoveWord(real, "\t");
	result += RemoveWord(real, "\r");
	result += RemoveWord(real, "\n");

	// ‘ã•\“I‚È—\–ñŒê
	result += RemoveWord(real, "&nbsp;");
	result += RemoveWord(real, "&shy;");
	result += RemoveWord(real, "&quot;");
	result += RemoveWord(real, "&amp;");
	result += RemoveWord(real, "&lt;");
	result += RemoveWord(real, "&gt;");
	result += RemoveWord(real, "&copy;");

	// “Áê‚È—\–ñŒê
	result += RemoveWord(real, "&middot;");

	// ”š
	result += RemoveWord(real, "0");
	result += RemoveWord(real, "1");
	result += RemoveWord(real, "2");
	result += RemoveWord(real, "3");
	result += RemoveWord(real, "4");
	result += RemoveWord(real, "5");
	result += RemoveWord(real, "6");
	result += RemoveWord(real, "7");
	result += RemoveWord(real, "8");
	result += RemoveWord(real, "9");

	// ‘ã•\‰w‚ÈL•¶š
	result += RemoveWord(real, "click");
	result += RemoveWord(real, "here");
	result += RemoveWord(real, "move");

	return result;
}

struct IFR_DATA
{
	COleDateTime tim;
	char* dom;
	char* src;
	int type;
	IFR_DATA* next;
};

IFR_DATA* _ifr_fir = NULL;
CRITICAL_SECTION _ifr_cri;

int InitIfr()
{
	InitializeCriticalSection(&_ifr_cri);

	return 0;
}

int AddIfr(IFR_DATA* pData)
{
	EnterCriticalSection(&_ifr_cri);

	int hour = 0;
	int minute = 15;
	if (pData->type == 7)
	{
		// 7  ... frame page
		minute = 30;
	}
	else if (pData->type == 17)
	{
		// 17  ... absolute page
		hour = 3;
	}
	else if (pData->type == 22)
	{
		// 22  ... sponsor page (“]‘—æ)
		minute = 15;
	}
	else if (pData->type == 27)
	{
		// 27  ... x ckicked page
		minute = 10;
	}

	COleDateTimeSpan maxtim(0, hour, minute, 0);
	pData->tim = COleDateTime::GetCurrentTime() + maxtim;

	CText::ToLower(pData->src);
	RemoveWord(pData->src, P("www."));
	RemoveWord(pData->src, "/");
	RemoveWord(pData->src, " ");

	if (_ifr_fir == NULL)
	{
		_ifr_fir = pData;
	}
	else
	{
		IFR_DATA* pos = _ifr_fir;
		for (; pos->next; pos = pos->next)
		{
		}
		pos->next = pData;
	}

	LeaveCriticalSection(&_ifr_cri);

	return 0;
}

int FindIfr(const char* host, const char* siteurl)
{
	EnterCriticalSection(&_ifr_cri);

	CText url(siteurl, strlen(siteurl));
	url.ToLower();
	RemoveWord(url, P("www."));
	RemoveWord(url, "/");
	RemoveWord(url, " ");

	int type = 0;

	IFR_DATA* pos = _ifr_fir;
	for (; pos; pos = pos->next)
	{
		if (strstr(url, pos->src) != 0)
		{
			if (strstr(host, pos->dom) != 0)
			{
				type = pos->type;
				break;
			}
			else if (memcmp(pos->src, "http:", 5) == 0)
			{
				type = pos->type;
				break;
			}
		}
	}

	LeaveCriticalSection(&_ifr_cri);

	return type;
}

int RefreshIfr()
{
	EnterCriticalSection(&_ifr_cri);

	COleDateTime now = COleDateTime::GetCurrentTime();

	IFR_DATA* pos = _ifr_fir;
	for (; _ifr_fir; )
	{
		if (_ifr_fir->tim > now)
		{
			break;
		}

		IFR_DATA* die = _ifr_fir;
		_ifr_fir = _ifr_fir->next;
		free(die->dom);
		free(die->src);
		delete die;
	}

	LeaveCriticalSection(&_ifr_cri);

	return 0;
}

int SetFramePage(const struct putil_data* pud, const char* pIFR)
{
	const char* fh_src = strstr(pIFR, "src");
	if (fh_src)
	{
		fh_src+=3;
		for (; fh_src[0] == ' ' || fh_src[0] == '='; fh_src++)
		{
		}
		char sep = fh_src[0];
		if (sep != '"' && sep != '\'' && sep != ' ')
		{
			sep = ' ';
		}
		else
		{
			fh_src++;
		}
		const char* fh_end = strchr(fh_src, sep);
		if (!fh_end && sep == ' ')
		{
			fh_end = strchr(fh_src + 1, '>');
		}
		if (fh_end)
		{
			for (; strncmp(fh_src, "../", 3) == 0; )
			{
				fh_src += 3;
			}
			for (; strncmp(fh_src, "./", 2) == 0; )
			{
				fh_src += 2;
			}
			size_t fr_src_len = fh_end - fh_src;
			if (fr_src_len < 2)
			{
				return 1;
			}

			size_t host_len = strlen(pud->http->host);
			IFR_DATA* pData = new IFR_DATA;
			pData->dom = (char*)malloc(host_len + 1);
			memcpy(pData->dom, pud->http->host, host_len);
			pData->dom[host_len] = NULL;
			pData->src = (char*)malloc(fr_src_len + 1);
			memcpy(pData->src, fh_src, fr_src_len);
			pData->src[fr_src_len] = NULL;
			pData->type = 7; // frame page
			pData->next = NULL;
			AddIfr(pData);

			char* sha = strchr(pData->src, '#');
			if (sha)
			{
				*sha = 0;
			}
		}
	}

	return 0;
}

int RegisterIfr(const struct putil_data* pud, const char* text)
{
	EnterCriticalSection(&_ifr_cri);

	RefreshIfr();

	const char* pIFR = text;
	for (; (pIFR = strstr(pIFR, "<iframe ")) != NULL; )
	{
		pIFR += 6;
		SetFramePage(pud, pIFR);
	}

	pIFR = text;
	for (; (pIFR = strstr(pIFR, "<frame ")) != NULL; )
	{
		pIFR += 6;
		SetFramePage(pud, pIFR);
	}

	LeaveCriticalSection(&_ifr_cri);

	return 0;
}

struct MEM_DATA
{
	const char* name;
	const char* org;
	char* pos;
	int length;
	int line;
};

static MEM_DATA _memData[] = {
	P("adlWord"), adlWord, adlWord, strlen(adlWord), 0,
	P("blkWord"), blkWord, blkWord, strlen(blkWord), 0,
	P("pad-config.txt"), config_txt, config_txt, strlen(config_txt), 0,
	P(".\\pad-match-all.action"), match_all_action, match_all_action, strlen(match_all_action), 0,
	P(".\\pad-default.action"), default_action, default_action, strlen(default_action), 0,
	P(".\\pad-default.filter"), default_filter, default_filter, strlen(default_filter), 0,
	0, 0, 0, 0, 0
};

int MemInitialize()
{
	for (int i = 0; _memData[i].name; i++)
	{
		char* p = (char*)_memData[i].org;
		int j = 0;
		for (; memcmp(&p[j], P("a-IR.o_F#ey,OU"), 14) != 0; j++)
		{
			p[j]^=87;
		}
		p[j] = 0;
	}

	return 0;
}

static const char* adl_target_s[] = {
	P("001,porn"),
	P("001,sex "),
	P("002,adult"),
	P("001,fuck"),
	P("001,penis"),
	P("001,erotic"),
	P("001,vagina"),
	P("001,lesbian"),
	P("001,risque"),
	P("001,pornographic"),
	0};

static const char* blk_target_s[] = {
	P("002,mafia"),
	P("002,kill my self"),
	0};

namespace initialize_this_one
{
	static int _memInitialize = MemInitialize();
	static word_data* _adl = MakeTrg(adlWord, adl_target_s);
	static word_data* _blk = MakeTrg(blkWord, blk_target_s);
	static int _ifr = InitIfr();
	static int _first = InitWakeup();
	static COleDateTime _start_time = COleDateTime::GetCurrentTime() - COleDateTimeSpan(0, 0, 0, GetTickCount() / 1000);
}

int IsWordPage(const struct putil_data* pud, int charset, plustar_page* page, word_data* word_targ, int gra_max)
{
	int result = 0;

	for (int i = 0; word_targ[i].str.ByteLength(); i++)
	{
#if 0
		if (i % 3 != charset)
		{
			continue;
		}
#endif

		for (char* pos = strstr(page->cur, (LPCSTR)word_targ[i].str.m_str); pos; pos = strstr(pos, (LPCSTR)word_targ[i].str.m_str))
		{
			result += word_targ[i].gravity;
			if (result >= gra_max)
			{
				return result;
			}

			pos += word_targ[i].str.ByteLength();
		}
	}

	return result;
}

/***************************************************************************
 * –ß‚è‚Æˆ—‡”Ô
 *
 * ---------------------------------------------
 * ?a=0&o=3090&b=1713&t=157&c=0.092&h=100
 * ---------------------------------------------
 *   0  ... ‰Šú’l
 *        ’Êíƒy[ƒW(‘å‘½”‚Ìˆê”Ê“I‚Èƒy[ƒW)
 *        Œ»’iŠK‚Å‚ÍƒƒWƒbƒN“I‚É·‚µ‚ñ‚Å‚àOK‚Ì”»’è‚Å
 *        ÀÛ‚É·‚µ‚Ü‚ê‚é‚©‚ÍãˆÊ‚Ìˆ—‚ÅŒˆ‚Ü‚éB
 * ---«L‚ª“ü‚ç‚È‚¢ƒpƒ^[ƒ“ -----------------
 * ?a=5&o=15804&b=0&t=0&c=0.000&h=000
 * ---------------------------------------------
 *   1  ... ŠÔ‹K§”p~
 *  22  ... sponsor page (“]‘—æ)
 *  12  ... text/html ‚Å‚Í‚È‚¢
 *   2  ... ‰½‚©‰ö‚µ‚¢
 *   3  ... ƒuƒ‰ƒEƒU‚ÌƒXƒ^[ƒgƒy[ƒW
 *	17  ... position:absolute ‚ğŠÜ‚ñ‚Å‚¢‚ÄƒfƒUƒCƒ“‚ª‰ó‚ê‚é‰Â”\«‚ª‚ ‚é
 *  21  ... ‘S‘ÌƒTƒCƒY‚ª¬‚³‚¢(300Bytes–¢–)
 *   4  ... google ƒtƒHƒ[ƒfƒBƒ“ƒOƒy[ƒW
 *   5  ... å—v‚È‹Lq‚Ì‚È‚¢•Ï‚Èƒy[ƒWiajax ‚È‚Çj
 *   6  ... “Á’è–@l‚³‚ñŠÖ˜A(“Á•Ê‘Î‰)
 *   7  ... frame page
 *  20  ... &(=)‚ª‘½‚¢
 *  19  ... title,description,keyword‚È‚µ
 *   9  ... <body ƒ^ƒO‚ª‚È‚¢
 * ---ªb‚Æt‚ªÏ‚Ü‚ê‚È‚¢
 * ---«b‚ªÏ‚Ü‚ê‚é ----------------------------
 * ?a=13&o=53455&b=40&t=0&c=0.000&h=100
 * ---------------------------------------------
 *  13  ... <body>‚Ì’†‚ª­‚È‚¢ 200bytes–¢–
 * [14] ... <body>‚Ì’†‚ª‘½‚·‚¬ 200Kbytes‚æ‚è‘å‚«‚¢
 *  24  ... ƒŠƒ“ƒN‚ª‚È‚¢
 *  23  ... description,keyword‚È‚µ‚ÅƒŠƒ“ƒN”‚ª10–¢–
 *  26  ... description,keyword‚È‚µ‚Åimgƒ^ƒO‚Æaƒ^ƒO‚Ì·•ª‚ª2ˆÈã
 * [25] ... ƒŠƒ“ƒN”‚ª20ˆÈã‚ ‚èAURL‚à’Pƒ‚Å‚ ‚é‚Ì‚Å’Êíƒy[ƒW‚Ì‰Â”\«‚ª‚‚¢
 *  15  ... <body>‚Ì’†‚ª‚Ù‚Æ‚ñ‚Ç‹ó
 * ---«t‚ªÏ‚Ü‚ê‚é ----------------------------
 * ?a=10&o=6783&b=4755&t=296&c=0.062&h=100
 * ---------------------------------------------
 *  11  ... ’†g(body‚©‚çƒ^ƒO‚ğ”²‚¢‚½c‚èƒTƒCƒY)‚ª­‚È‚¢ 100bytes–¢–
 * [16] ... ’†g(body‚©‚çƒ^ƒO‚ğ”²‚¢‚½c‚èƒTƒCƒY)‚ª‘½‚¢ 500bytes‚æ‚è‘å‚«‚¢
 *  10  ... ’†g(‘ÎÛ‚ÍBodyABody‚©‚çJavascriptACSSAƒ^ƒO‚ğ”²‚¢‚½‚à‚Ì)‚ª0.08
 *   8  ... ƒAƒ_ƒ‹ƒg(‘ÎÛƒ[ƒh—İÏ6ƒqƒbƒg‚ÅƒAƒ_ƒ‹ƒg)
 *  38  ... ƒuƒ‰ƒbƒN(‘ÎÛƒ[ƒh—İÏ6ƒqƒbƒg‚Åƒuƒ‰ƒbƒN)
 *
 *  27  ... x ƒ{ƒ^ƒ“‚ª‰Ÿ‚³‚ê‚½
 *  28  ... “Á’è“X•Ü‚Å x ƒ{ƒ^ƒ“‚ª‰Ÿ‚³‚ê‚½
 *  32  ... sponsor page
 ***************************************************************************/
int utl_filter_body(const struct putil_data* pud, void* data, plustar_page* page)
{
	int result = 0;

	"2011”N3Œ11‚É”­¶‚µ‚Ü‚µ‚½“Œ“ú–{‘åkĞ‚ÅA–S‚­‚È‚ç‚ê‚½•ûX‚Ì‚²–»•Ÿ‚ğ‚¨‹F‚è\‚µã‚°‚Ü‚·B"
	"”íĞ‚³‚ê‚½ŠF—lA‚»‚Ì‚²‰Æ‘°‚Ì•ûX‚É‘Î‚µ‚Ü‚µ‚ÄAS‚æ‚è‚¨Œ©•‘‚¢\‚µã‚°‚Ü‚·B"
	"ˆêl‚Å‚à‘½‚­‚Ì–³–‚ªˆê“ú‚Å‚à‘‚­Šm”F‚Å‚«‚é‚±‚Æ‚ğŠè‚Á‚Ä‚¢‚Ü‚·B ŠJ”­Òˆê“¯ " __DATE__;

	CText text(page->cur, page->content_len);
	text.ToLower();

	if (strstr(pud->http->path, "embed"))
	{
		// 2 ... ‰½‚©‰ö‚µ‚¢
		page->type = 2;
		result = -1;
		return result;
	}

	/*
	 * ˆ—‚Ì‚‘¬‰»
	 * ƒTƒCƒY‚ª350BytesˆÈ‰º‚Í’e‚­
	 * 
	 *             b300b350b400b450b500b550b600b
	 * ------------------------------------------------
	 * 0:’Êí      b 58b 82b 89b102b131b151b168b
	 * 1:L      b284b344b369b465b500b534b582b
	 * 2:ƒKƒWƒFƒbƒgb 13b 29b 33b 33b 33b 35b 41b
	 * 3:iframe    b 14b 15b 15b 15b 17b 21b 24b
	 *
	 * ’Êí‚Æ’è‹`‚Í‚µ‚Ä‚¢‚é‚ª‚Ù‚Ú404ƒy[ƒW‚È‚Ç‚Å‚ ‚é
	 *
	 */
	if(page->content_len < 300)
	{
		pud->log_out(LOG_LEVEL_FORCE, "Too Small Page [%d] (%s)",page->content_len ,pud->http->url);
		// 21 ... ‘S‘ÌƒTƒCƒY‚ª¬‚³‚¢(350Bytes–¢–)
		page->type = 21;
		result = -1;
		return result;
	}

	if (text.GetLength() && (strstr(text, "<title>") || strstr(text, "<!doctype html") ||
			strstr(text, "<html") || strstr(text, "<body")))
	{
		if (strstr(text, P("<title>302 moved</title>")))
		{
			pud->log_out(LOG_LEVEL_FORCE, "forwording page (%s)", pud->http->url);
			// 4 ... google ƒtƒHƒ[ƒfƒBƒ“ƒOƒy[ƒW
			page->type = 4;
			result = -1;
			return result;
		}
	}
	else
	{
		pud->log_out(LOG_LEVEL_FORCE, "ajax page (%s)", pud->http->url);
		// 5 ... å—v‚È‹Lq‚Ì‚È‚¢•Ï‚Èƒy[ƒWiajax ‚È‚Çj
		page->type = 5;
		result = -1;
		return result;
	}

	if (strstr(pud->http->host, P("runsystem.co.jp")) != 0)
	{
		// 6 ... “Á’è–@l‚³‚ñŠÖ˜A(“Á•Ê‘Î‰)
		page->type = 6;
		result = -1;
		return result;
	}

	RegisterIfr(pud, text);

	int type = FindIfr(pud->http->host, pud->http->url);
	if (type != 0)
	{
		pud->log_out(LOG_LEVEL_FORCE, "logical frame page overtime (%s)", pud->http->url);
		// frame page, sponsor page, absolute
		page->type = type;
		result = -1;
		return result;
	}

	// http://www.plustar.jp/contact.php?t=ques
	// u=v‚ª1‚Åu/v‚ª3 ‡Œv4ƒJƒEƒ“ƒg‚Æ‚È‚é
	int ec_count = 0;
	int sp_count = 0;
	for (char* p = pud->http->url; (p = strstr(p, "=")); p++)
	{
		ec_count++;
	}
	for (char* p = pud->http->url; (p = strstr(p, "/")); p++)
	{
		sp_count++;
	}

	/*
	 * &(=)‚ª‘½‚¢ƒy[ƒW‚ÍL‚Ì‰Â”\«‚ª‚‚¢
	 * ˆ—‚Ì‚‘¬‰»‚àŒ“‚Ë‚Äˆ—‚ğ“ü‚ê‚é
	 *
	 * ƒ^ƒCƒv       b27ˆÈãb28ˆÈãb29ˆÈãb30ˆÈãb31ˆÈãb‘” b
	 * --------------------------------------------------------------
	 * 0F’Êí      b    55b    55b    51b    41b    29b17055b
	 * 1FL      b  3048b  3046b  3046b  3033b  3026b 6247b
	 * 2FƒKƒWƒFƒbƒgb    82b    82b    82b    82b    82b 3868b
	 * 3Fiframe    b     0b     0b     0b     0b     0b   75b
	 */
	if(ec_count > 30)
	{
		pud->log_out(LOG_LEVEL_FORCE, "too many separaters [%d]  (%s)", ec_count ,pud->http->url);
		// 20 ... &(=)‚ª‘½‚¢
		page->type = 20;
		result = -1;
		return result;
	}

	/*
	 * ‚±‚Ìˆ—‚Íƒ`ƒ‡ƒCŠëŒ¯‚©‚È
	 * Lƒy[ƒW‚É·‚µ‚Ü‚ê‚Ä‚µ‚Ü‚¤B
	 * 
	 * alive ads (http://lead.program3.com/ad/1000.html)
	 * 
	 * ƒf[ƒ^‚ğæ“¾‚µ‚Ä‚©‚çƒTƒCƒYA‚à‚µ‚­‚Ítitle,description,keyword‚ ‚½‚è‚à‰Á–¡
	 *
	 * ƒRƒƒ“ƒg‚·‚é‚±‚Æ‚Ì•¾ŠQ
	 * ‰æ‘œ‚ª‘½‚¢ƒy[ƒW‚È‚Ç‚ÉL‚ª“ü‚ç‚È‚­‚È‚éB
	 *  Ehttp://www.buichi.com/
	 *
	 * &(=)F0      b ‘”btitle‚È‚µbtitle‚ ‚èbtitle‚ ‚èb
	 *              b     b         b         b  des‚ ‚èb
	 * -------------------------------------------------------
	 * 0F’Êí      b11929b      459b    11470b     6392b
	 * 1FL      b  619b      448b      171b       14b
	 * 2FƒKƒWƒFƒbƒgb  151b       54b       97b        8b
	 * 3Fiframe    b   53b       19b       34b        6b
	 *
	 */
	//if (ec_count + sp_count <= 4)
	//{
	//	pud->log_out(LOG_LEVEL_FORCE, "plain page (%s)", pud->http->url);
	//	// 17 ... url‚ª’Pƒ
	//	page->type = 17;
	//	return 0;
	//}

	char* real = text;
	int min_take = 100;
	double min_perf = 0.08;

	// description ‚ª—L‚Á‚½‚ç”ñ•\¦‚Ì‹K§‚ğŠÉ˜a
	char* meta_desc = strstr(real, "<meta name=\"description\"");
	if (meta_desc)
	{
		min_take -= 20;
		min_perf -= 0.01;
	}

	char* meta_key = strstr(real, "<meta name=\"keywords\"");
	if (meta_key)
	{
		min_take -= 20;
		min_perf -= 0.01;
	}

	// ƒ^ƒCƒgƒ‹‚Ìæ“¾
	// ‚±‚ê‚àŠëŒ¯‚â‚ÈAAu<title></title>v‚Á‚Ä’†g‚È‚¢‚Ì‚ªŒ‹\‚ ‚é
	char* meta_title = strstr(real, "<title>");
	if (!meta_title)
	{
		// http://blog.goo.ne.jp/‚Ìƒ^ƒCƒgƒ‹‚Ég—p‚³‚ê‚Ä‚¢‚éŒ`®‚É‘Î‰‚·‚é
		// <title lang="ja" xml:lang="ja"></title>
		meta_title = strstr(real, "<title ");
	}

	// Œ»“_‚Å‚ÍLAiframe‚Æ‚Ì‘ŠŠÖ‚ª‚ ‚è‚»‚¤‚È‹C‚ª‚·‚éB
	// Šî–{“I‚ÉL‚ª’Z‚¢
	if (meta_title)
	{
		// ƒ^ƒCƒgƒ‹‚Ì’·‚³‚ğZo‚·‚é
	}

	/* 
	 * ’Êí‚Åtitle,description,keyword‹¤‚É‚È‚µ‚Ìƒpƒ^[ƒ“‚ğŒŸØ‚µ‚½Œ‹‰Ê‚Ù‚Æ‚ñ‚Ç‚ª
	 * LA‚à‚µ‚­‚ÍƒKƒWƒFƒbƒg‚Å‚ ‚Á‚½B
	 *
	 *             b 000b 100b 110b 111b
	 * --------------------------------------
	 * 0:’Êí      b 454b7639b1171b6984b
	 * 1:L      b5045b1127b  22b  47b
	 * 2:ƒKƒWƒFƒbƒgb 209b3173b 482b   4b
	 * 3:iframe    b  34b  33b   1b   7b
	 *
	 * Œ©‚ê‚ÎŒ©‚é‚Ù‚Çƒpƒ^[ƒ“u000v‚Ì’Êíƒy[ƒW‚Á‚Ä‚Ì‚Í‚È‚¢‚ÈBB
	 *
	 * ‚æ‚Á‚Ä&(=)‚Ì”‚Ì”»’èˆ—‚ğ‚È‚­‚µtitle,description,keyword‚Ì—L–³‚Å”»’f‚·‚éB
	 *
	 * aƒ^ƒO‚Ì”‚ÆƒRƒ“ƒeƒ“ƒc‚ÌŠÖŒW
	 * aƒ^ƒO‚Ì”‚ğ10–¢–‚Åi‚é
	 *             b 000b 100b 110b 111b
	 * --------------------------------------
	 * 0:’Êí      b 369b1730b 104b 265b << ¦‚­Œë”š‚ªŒ¸‚é
	 * 1:L      b5026b1001b  20b  19b
	 * 2:ƒKƒWƒFƒbƒgb 204b2962b 113b   1b
	 * 3:iframe    b  32b  30b    b   4b
	 */
	page->head.title = meta_title ? 1 : 0;
	page->head.description = meta_desc ? 1 : 0;
	page->head.kyeword = meta_key ? 1 : 0;
	if(!meta_title && !meta_desc && !meta_key)
	{
		pud->log_out(LOG_LEVEL_FORCE, "no title,des,keyword (%s)", pud->http->url);
		// 19 ... title,description,keyword‚È‚µ
		page->type = 19;
		result = -1;
		return result;
	}

	// <body c </body>‚ğ‘ÎÛ‚É‚·‚é
	char* startbody = strstr(real, "<body");
	startbody = startbody ? strstr(startbody, ">") : NULL;
	if (!startbody)
	{
		pud->log_out(LOG_LEVEL_FORCE, "body not found (%s)", pud->http->url);
		// 9 ... <body ƒ^ƒO‚ª‚È‚¢
		page->type = 9;
		result = -1;
		return result;
	}

	startbody++;
	real = startbody;

	char* endbody = strstr(real, "</body>");
	if (endbody)
	{
		endbody[0] = 0;
	}

	page->body_len = strlen(real);

	if (page->body_len < 200)
	{
		pud->log_out(LOG_LEVEL_FORCE, "small page (%s)", pud->http->url);
		// 13 ... <body>‚Ì’†‚ª­‚È‚¢ 200bytes–¢–
		page->type = 13;
		result = -1;
		return result;
	}
	else if (page->body_len > 200 KB)
	{
		pud->log_out(LOG_LEVEL_FORCE, "large page (%s)", pud->http->url);
		// 14 ... <body>‚Ì’†‚ª‘½‚·‚¬ 200Kbytes‚æ‚è‘å‚«‚¢
		page->type = 14;
		// ãˆÊƒAƒ_ƒ‹ƒgˆ—‚É‚Ä
		//   8FƒAƒ_ƒ‹ƒg
		//   0F’Êí
		// ‚Ì”»’è‚ª“ü‚é
	}
	else
	{
		/*
		 * ‘S•¶ŒŸõ‚ÌuDeleteTagSkeleton(real)v‚ª‘–‚é‘O‚É—\‚ß‘ÎÛ‚ğŒ¸‚ç‚·
		 *
		 * ‚±‚Ì‚Åg‚¦‚é—v‘f
		 * Ecount_a    Fcount_a
		 * Ebody_len   FbodyƒTƒCƒY
		 * Emeta_title Fƒƒ^ƒ^ƒCƒgƒ‹(‚ ‚èE‚È‚µ)
		 * Emeta_desc  Fƒƒ^ƒfƒBƒXƒNƒŠƒvƒVƒ‡ƒ“(‚ ‚èE‚È‚µ)
		 * Emeta_key   Fƒƒ^ƒL[(‚ ‚èE‚È‚µ)
		 * Econtent_lenFƒy[ƒW‘S‘ÌƒTƒCƒY
		 * Eec_count   F&(=)‚Ì”
		 *
		 * ‹É’[‚Éaƒ^ƒO‚ª­‚È‚¢‚à‚Ì‚ª‚ ‚é‚Ì‚ÅAƒƒ^‚ğŒ©‚Ä•â³’l‚ğ“ü‚ê‚éB
		 * 
		 * ƒxƒCƒY“Œv‚É‚ÄL‚Å‚ ‚éŠm—¦‚ğZo‚µ‚ÄL‚Å‚ ‚è‚»‚¤‚Å‚ ‚ê‚Î
		 * Œã‘±ˆ—‚ğ‘–‚ç‚¹‚éB
		 *
		 */
		CText real_a(real);
		int count_a   = DeleteTag(real_a, "<a ", "/a>");
		int count_img = DeleteTag(real_a, "<img ", ">");

		// imgƒ^ƒO‚Æaƒ^ƒO‚Ì·
		int diff_img_a = count_img - count_a ;
		//diff_img_a = (diff_img_a > 0) ? diff_img_a : diff_img_a * (-1);

		/*
		 * rev11FƒƒWƒbƒN•ÏX‘O
		 * rev15F23,14,25ƒƒWƒbƒN‘Î‰
		 * 
		 * b  b   11b   15b  %11b  %15bLb  L•\¦”b         ƒXƒe[ƒ^ƒX‚ÌˆÓ–¡b
		 * --------------------------------------------------------------------------------
		 * b 0b 2287b 1559b8.567b6.995b  üb 2287b 1559b’Êíƒy[ƒW               b
		 * b10b  609b  163b2.281b0.731b  ~b    0b    0b’†g‚ª0.08               b
		 * b11b  310b   51b1.161b0.228b  ~b    0b    0b’†g‚ª­‚È‚¢ 100b«      b
		 * b13b  655b  433b2.453b1.943b  ~b    0b    0b<body>‚Ì’†‚ª­‚È‚¢ 200b«b
		 * b15b 4670b  141b17.49b0.632b  ~b    0b    0b<body>‚Ì’†‚ª‚Ù‚Æ‚ñ‚Ç‹ó   b
		 * b16b13423b  763b50.28b3.423b  üb13423b  763b’†g‚ª‘½‚¢ 500bª        b
		 * b19b 4620b 3353b17.30b15.04b  ~b    0b    0btitle,des,keyword‚È‚µ    b
		 * b21b  120b   83b0.449b0.372b  ~b    0b    0b‘S‘ÌƒTƒCƒY‚ª¬‚³‚¢ 300B«b
		 * b23b    -b 4093b    -b18.36b  ~b    0b    0bƒŠƒ“ƒN”‚ª10–¢–         b
		 * b24b    -b  625b    -b2.804b  ~b    0b    0bƒŠƒ“ƒN‚ª‚È‚¢             b
		 * b25b    -b11021b    -b49.45b  üb    0b11021b’Êíƒy[ƒW‚Ì‰Â”\«‚ª‚‚¢ b
		 * --------------------------------------------------------------------------------
		 * b  b26694b22285b     b     b    b15710b13343b                         b
		 * --------------------------------------------------------------------------------
		 * b  b     b     b     b     b    b0.539b0.531b                         b
		 * --------------------------------------------------------------------------------
		 * 
		 * rev15‚Ìˆ—’†‚Éƒ}ƒVƒ“‚ª—‚¿‚½‚Ì‚ÅŠ„‡(%)‚Å‚à‹Lq
		 */

		/*
		 * ƒpƒ^[ƒ“1
		 * Ecount_a    F0
		 * Ecount_img  F-
		 * Ebody_len   F-
		 * Emeta_title F-
		 * Emeta_desc  F-
		 * Emeta_key   F-
		 * Econtent_lenF-
		 * Eec_count   F-
		 *
		 * ‰Û‘è
		 * EƒTƒCƒY§ŒÀ
		 */
		if(count_a == 0)
		{
			pud->log_out(LOG_LEVEL_FORCE, "No Link a == 0 (%s)", pud->http->url);
			// 24 ... ƒŠƒ“ƒN‚ª‚È‚¢
			page->type = 24;
			result = -1;
			return result;
		}

		/*
		 * ƒpƒ^[ƒ“2
		 * Ecount_a    F10–¢–
		 * Ecount_img  F-
		 * Ebody_len   F-
		 * Emeta_title F-
		 * Emeta_desc  F‚È‚µ
		 * Emeta_key   F‚È‚µ
		 * Econtent_lenF-
		 * Eec_count   F-
		 *
		 * ‰Û‘è
		 * EƒTƒCƒY§ŒÀ
		 */
		if(!meta_desc && !meta_key && count_a < 10)
		{
			pud->log_out(LOG_LEVEL_FORCE, "No meta desc,key and a < 10 (%s)", pud->http->url);
			// 23 ... description,keyword‚È‚µ‚ÅƒŠƒ“ƒN”‚ª10–¢–
			page->type = 23;
			result = -1;
			return result;
		}

		/*
		 * ƒpƒ^[ƒ“3
		 * Ecount_a    F10–¢–
		 * Ecount_img  F-
		 * Ebody_len   F-
		 * Emeta_title F-
		 * Emeta_desc  F‚È‚µ
		 * Emeta_key   F‚È‚µ
		 * Econtent_lenF-
		 * Eec_count   F-
		 *
		 * aƒ^ƒO‚Æimgƒ^ƒO‚ÌŠÖŒW
		 *
		 * ----------------------------------------------------
		 * ’ŠoğŒb   descAkey‚È‚µbb  title,descAkey‚ ‚èb
		 * ----------------------------------------------------
		 *         b a>ib a=ib a<ibb   a>ib  a=ib   a<ib
		 * ----------------------------------------------------
		 *       10b  87b   -b   6bb    36b    -b     9b
		 *       11b   2b   1b   1bb     9b    1b     3b
		 *       13b  23b 377b  10bb     2b   17b     -b
		 *       15b  49b   -b   2bb    60b    6b     6b
		 *       16b 230b   1b  31bb   374b    7b    49b
		 *       19b1870b1106b 377bb     -b    -b     -b
		 *       21b   4b  74b   5bb     -b    -b     -b
		 *       23b3834b 100b 159bb     -b    -b     -b
		 *       24b   -b 445b 130bb     -b   17b    27b
		 *       25b3083b   2b  64bb  5889b    3b    93b
		 * ----------------------------------------------------
		 *
		 * “Á’¥
		 * @[’Êíƒy[ƒW]
		 * @@Eaƒ^ƒO‚ªimgƒ^ƒO‚æ‚è‚à–¾‚ç‚©‚É‘½‚¢
		 * @[LAiframe]
		 * @@Eaƒ^ƒO=imgƒ^ƒO‚Ì•s©‘R‚ª•’Ê‚É‘¶İ‚·‚é
		 * @@EƒŠƒ“ƒN‚æ‚è‚àƒCƒ[ƒW‚ª‘½‚¢
		 * @@ ’Êí‚ÍƒŠƒ“ƒN(“à•”ƒŠƒ“ƒNAŠO•”ƒŠƒ“ƒN)‚ª‚ ‚è‚«‚ÌƒCƒ[ƒW‚Å‚ ‚é
		 * @@¨ aƒ^ƒO > imgƒ^ƒO
		 * @@
		 * -------------------------------------------------------
		 * ’ŠoğŒb       descAkey‚È‚µbb title,descAkey‚ ‚èb
		 * -------------------------------------------------------
		 *         b   i/ab a/ib   a/ibb   i/ab a/ib   a/ib
		 * -------------------------------------------------------
		 *       10b0.1034b   -b0.5696bb0.1717b   -b0.5323b
		 *       11b0.2583b1.00b0.6875bb0.3095b1.00b0.1369b
		 *       13b0.0000b1.00b0.0000bb0.0000b1.00b     -b
		 *       15b0.1347b   -b0.5208bb0.1236b   -b0.4523b
		 *       16b0.1928b1.00b0.5949bb0.1369b1.00b0.5317b
		 *       19b0.2726b1.00b0.3468bb     -b1.00b     -b
		 *       21b0.0000b1.00b0.0000bb     -b1.00b     -b
		 *       23b0.0358b1.00b0.4190bb     -b1.00b     -b
		 *       24b     -b1.00b0.0000bb     -b1.00b0.0000b
		 *       25b0.0735b1.00b0.6941bb0.1233b1.00b0.7028b
		 * -------------------------------------------------------
		 *
		 * ----------------------------------------------------------
		 * ’ŠoğŒb       descAkey‚È‚µbb    title,descAkey‚ ‚èb
		 * ----------------------------------------------------------
		 *         b     a-ib a-ib i-abb     a-ib a-ib    i-ab
		 * ----------------------------------------------------------
		 *       10b 11.3563b   -b14.5bb 11.0000b   -b 6.3333b
		 *       11b  8.0000b   0b 5.0bb  5.0000b   0b 8.6667b
		 *       13b  1.0435b   0b 1.0bb  1.0000b   0b      -b
		 *       15b 12.0000b   -b13.5bb  6.6167b   -b 7.5000b
		 *       16b 11.1609b   0b21.7bb 11.3209b   0b11.0408b
		 *       19b  2.0299b   0b 2.3bb       -b   0b      -b
		 *       21b  1.0000b   0b 1.0bb       -b   0b      -b
		 *       23b  2.5527b   0b 9.2bb       -b   0b      -b
		 *       24b       -b   0b 2.2bb       -b   0b 6.1852b
		 *       25b109.4674b   0b40.5bb136.8119b   0b30.8387b
		 * ----------------------------------------------------------
		 * 
		 * ‰Û‘è
		 * E‰ºŒÀ‚¾‚¯‚Å‚È‚­ãŒÀ‚à•K—v
		 */
		if(!meta_desc && !meta_key && diff_img_a > 1)
		{
			pud->log_out(LOG_LEVEL_FORCE, "No meta desc,key and diff img a > 1 [%d] (%s)", diff_img_a,pud->http->url);
			// 26  ... description,keyword‚È‚µ‚Åimgƒ^ƒO‚Æaƒ^ƒO‚Ì·•ª‚ª2ˆÈã
			page->type = 26;
			result = -1;
			return result;
		}

		/*
		 * ƒpƒ^[4[ƒeƒXƒgI]
		 * Ecount_a    F20ˆÈã
		 * Ecount_img  F-
		 * Ebody_len   F-
		 * Emeta_title F‚ ‚è
		 * Emeta_desc  F-
		 * Emeta_key   F-
		 * Econtent_lenF-
		 * Eec_count   F0
		 *
		 * ‰Û‘è
		 * EƒTƒCƒY§ŒÀ
		 */
		if(meta_title && count_a >= 20 && ec_count == 0)
		{
			pud->log_out(LOG_LEVEL_FORCE, "input ad (%s)", pud->http->url);
			// 25 ... ƒŠƒ“ƒN”‚ª20ˆÈã‚ ‚èAURL‚à’Pƒ‚Å‚ ‚é‚Ì‚Å’Êíƒy[ƒW‚Ì‰Â”\«‚ª‚‚¢
			page->type = 25;

			// L‚ğo‚·I
			result = 0;
			return result;
		}

		DeleteTagSkeleton(real);

		// Javascript‚ğ”²‚¢‚½‚à‚Ì‚ğƒTƒCƒY‚Æ‚·‚éB
		page->body_len = strlen(real);

		CText nature(real);

		DeleteTag(real, "<a href=\"http://www.amazon.co.jp/exec/obidos/", "/a>");
		DeleteTag(real, "<", ">");
		RemoveWordTrush(real);

		////////////////////////////////////////////////////////////////////////
		// ‚±‚Ìˆ—‚Íu24 ... ƒŠƒ“ƒN‚ª‚È‚¢v‚Å‚Ù‚Ú’e‚©‚ê‚Ä‚¢‚é                //
		DeleteTag(nature, "<a ", "/a>");                                      //
		DeleteTag(nature, "<", ">");                                          //
		RemoveWordTrush(nature);                                              //
                                                                          //
		int natu_len = strlen(nature);                                        //
		if (natu_len < 20)                                                    //
		{                                                                     //
			pud->log_out(LOG_LEVEL_FORCE, "empty page (%s)", pud->http->url);   //
			// 15 ... <body>‚Ì’†‚ª‚Ù‚Æ‚ñ‚Ç‹ó                                    //
			page->type = 15;                                                    //
			result = -1;                                                        //
			return result;                                                      //
		}                                                                     //
		////////////////////////////////////////////////////////////////////////
	}

	page->take_len = strlen(real);

	if (page->take_len < min_take)
	{
		pud->log_out(LOG_LEVEL_FORCE, "small page (%s)", pud->http->url);
		//  11 ... ’†g‚ª­‚È‚¢ 100bytes–¢–
		page->type = 11;
		result = -1;
		return result;
	}
	else if (page->take_len > 500)
	{
		////////////////////////////////////////////////////////////////////////
		// 25 ... ƒŠƒ“ƒN”‚ª20ˆÈã‚ ‚èAURL‚à’Pƒ‚Å‚ ‚é‚Ì‚Å                   //
		//        ’Êíƒy[ƒW‚Ì‰Â”\«‚ª‚‚¢                                    //
		pud->log_out(LOG_LEVEL_FORCE, "large page (%s)", pud->http->url);     //
		// 16 ... ’†g‚ª‘½‚¢ 500bytes‚æ‚è‘å‚«‚¢                               //
		page->type = 16;                                                      //
                                                                          //
		// ãˆÊƒAƒ_ƒ‹ƒgˆ—‚É‚Ä                                               //
		//   8FƒAƒ_ƒ‹ƒg                                                      //
		//   0F’Êí                                                          //
		// ‚Ì”»’è‚ª“ü‚é                                                       //
		////////////////////////////////////////////////////////////////////////
	}
	else
	{
		if (page->body_len)
		{
			double perc = (double)page->take_len / page->body_len;
			if (perc < min_perf)
			{
				pud->log_out(LOG_LEVEL_FORCE, "small page (%s)", pud->http->url);
				//  10 ... ’†g‚ª0.08
				page->type = 10;
				result = -1;
				return result;
			}
		}
	}

	return result;
}

struct CONFDATA
{
	int sire;
	int count;

	int conf;
	int na;
	int nb;
	int nc;
	int nd;

	CONFDATA()
		: count(0)
		, sire(2)
		, conf(1)
		, na(2)
		, nb(4)
		, nc(4)
		, nd(6)
	{
	}
};

int ConfData(CONFDATA** pData)
{
	static CONFDATA _data;

	*pData = &_data;

	return 0;
}

char* NextConfData(char*& pRecvData)
{
	char* next = strchr(pRecvData, '\n');
	if (!next)
	{
		pRecvData += strlen(pRecvData);
	}
	else
	{
		pRecvData = next + 1;
	}
	return pRecvData;
}

struct IGNOREURLDATA
{
	int ignoreTotal;
	int xprint;
	int xsponsor;
	COleDateTime control;
	int video_ign;

	int ad_imp;
	int ad_ctr;
	int ad_esc;
	int ad_adl;

	IGNOREURLDATA()
		: ignoreTotal()
		, xprint(0)
		, xsponsor(0)
		, control(COleDateTime::GetCurrentTime())
		, video_ign(0)
		, ad_imp(0)
		, ad_ctr(0)
		, ad_esc(0)
		, ad_adl(0)
	{
	}
};

int IgnoreUrl(IGNOREURLDATA** pData)
{
	static IGNOREURLDATA _data;

	*pData = &_data;

	return 0;
}

#define XCLICK_RESTORATION 1, 0  // x‰Ÿ‰º’¼ŒãALo‚³‚È‚¢•p“x‚Ì‰ñ•œ

const char* strict_url[] =
{
	"youtube.com",
	"nicovideo.jp",
	0
};

CRITICAL_SECTION* url_critical()
{
	static CRITICAL_SECTION _crit = { 0 };

	InitializeCriticalSection(&_crit);

	return &_crit;
}

class CAutoCritical
{
protected:
	CRITICAL_SECTION* _crit;
public:
	CAutoCritical(CRITICAL_SECTION* crit)
		: _crit(crit)
	{
		EnterCriticalSection(_crit);
	}
	virtual ~CAutoCritical()
	{
		LeaveCriticalSection(_crit);
	}
};

int GetDomain(LPCTSTR url, CText& domain)
{
	if (_strnicmp(url, P("http://"), strlen(P("http://"))) != 0)
	{
#ifdef _DEBUG
		_asm
		{
			int 3
		}
#endif
		return -1;
	}

	const char* x_domain = url + strlen(P("http://"));
	const char* x_domain_end = strstr(x_domain, "/");
	if (!x_domain_end)
	{
#ifdef _DEBUG
		_asm
		{
			int 3
		}
#endif
		x_domain_end = x_domain + strlen(x_domain);
	}
	size_t host_len = x_domain_end - x_domain;

	domain.StrCpy(x_domain, host_len);

	return 0;
}

int utl_url_check(const struct putil_data* pud, void* data)
{
	const char* url_imp = P("http://x.plustar.jp/ph.gif?");
	if (_strnicmp(pud->http->url, url_imp, strlen(url_imp) == 0))
	{
		IGNOREURLDATA* pData = 0;
		IgnoreUrl(&pData);

		// view sponsor
		pData->ad_imp++;
	}

	const char* url_std = P("http://x.plustar.jp/pc.php?callback=psQuery");
	const char* url_adl = P("http://x.plustar.jp/ac.php?callback=psQuery");
	if (_strnicmp(pud->http->url, url_std, strlen(url_std)) == 0 ||
		_strnicmp(pud->http->url, url_adl, strlen(url_adl)) == 0)
	{
		CText domain;

		const char* ref = strstr(pud->hdr, P("Referer:"));
		if (ref)
		{
			ref += strlen(P("Referer: "));
			GetDomain(ref, domain);
		}

		// ƒXƒNƒŠ[ƒ“ƒTƒCƒY
		int cx = GetSystemMetrics(SM_CXSCREEN);
		int cy = GetSystemMetrics(SM_CYSCREEN);

		IGNOREURLDATA* pData = 0;
		IgnoreUrl(&pData);

		// x Buuton Clicked
		pData->ad_esc++;

		CONFDATA* pConfData = 0;
		ConfData(&pConfData);

		// x Buuton Clicked
		pData->ignoreTotal++;
		pData->xprint = cy > 768 ? ((pData->ignoreTotal == 1) ? pConfData->na : pConfData->nb) : ((pData->ignoreTotal == 1) ? pConfData->nc : pConfData->nd);

		if (pData->ignoreTotal > 10)
		{
			pData->ignoreTotal = 10;
		}

		COleDateTime now = COleDateTime::GetCurrentTime();
		COleDateTimeSpan limitations(0, 0, XCLICK_RESTORATION);
		pData->control = now + limitations;

		if (domain.GetLength())
		{
			domain.ToLower();
			pData->video_ign = 0;
			for (int i = 0; strict_url[i]; i++)
			{
				if (strstr(domain, strict_url[i]) != NULL)
				{
					pData->video_ign = 1;
					pData->xprint *= 2;
					break;
				}
			}
		}
	}

	return 0;
}

int utl_url_redirect(const struct putil_data* pud, void* data)
{
	const char* url_dac = P("http://as.dc.impact-ad.jp/ADCLICK/CID=");
	const char* url_ebs = P("http://ac.ebis.ne.jp/tr_set.php?argument=");
	const char* url_pst = P("http://ac.plustar.jp/ad/rd.php?url=");
	if (_strnicmp(pud->http->url, url_dac, strlen(url_dac)) == 0 ||
		_strnicmp(pud->http->url, url_ebs, strlen(url_ebs)) == 0 ||
		_strnicmp(pud->http->url, url_pst, strlen(url_pst)) == 0)
	{
		IGNOREURLDATA* pData = 0;
		IgnoreUrl(&pData);

		// ƒXƒ|ƒ“ƒT[ƒy[ƒW‚ÉL‚ğo‚³‚È‚¢
		pData->xsponsor += 1;

		// Sponsor Clicked
		pData->ad_ctr++;

		const char* ref = strstr(pud->hdr, P("Referer:"));
		if (ref)
		{
			ref += strlen(P("Referer: "));
			CText domain;
			GetDomain(ref, domain);
		}

		char* locat = P("Location: ");
		const char* loc = strstr(pud->buf, locat);
		if (loc)
		{
			loc += strlen(locat);
			const char* locend = strstr(loc, "\r\n");
			if (!locend)
			{
				locend = loc + strlen(loc);
			}

			size_t fr_src_len = locend - loc;
			size_t host_len = strlen(pud->http->host);
			IFR_DATA* pData = new IFR_DATA;
			pData->dom = (char*)malloc(host_len + 1);
			memcpy(pData->dom, pud->http->host, host_len);
			pData->dom[host_len] = NULL;
			pData->src = (char*)malloc(fr_src_len + 1);
			memcpy(pData->src, loc, fr_src_len);
			pData->src[fr_src_len] = NULL;
			pData->type = 22; // 22 ... sponsor page
			pData->next = NULL;

			// domain suppress
			char* cut = strchr(pData->src + strlen(P("http://")), '/');
			if (cut)
			{
				cut[0] = NULL;
			}

			AddIfr(pData);
		}

		pud->log_out(LOG_LEVEL_FORCE, "redirect before Advertisement page (%s)", pud->http->url);
		return 1;
	}

	return 0;
}

#define SECOND *(1000)

int utl_url_trance(const struct putil_data* pud, void* data)
{
	utl_trance* putl = (utl_trance*)data;

	if (_strnicmp(pud->http->url, P("http://pad.plustar.jp/ps_getadd"), strlen(P("http://pad.plustar.jp/ps_getadd"))) == 0)
	{
		static CString ip_address;
		static CString mac_address;
		if (ip_address.IsEmpty() || mac_address.IsEmpty())
		{
			for (; GetLocalInfo(P("dl.plustar.jp"), ip_address, mac_address) != 0; Sleep(2 SECOND))
			{
			}
		}

		// jQuery JSONP callback
		char* jcb = strstr(pud->http->url, P("?callback="));
		if (!jcb)
		{
			return -1;
		}

		char* org = jcb + strlen(P("?callback="));
		int len = strlen(org);
		char* end = strchr(org, '&');
		if (end)
		{
			len = end - org;
		}
		else
		{
			len = strlen(org);
		}

		CText callback(org, len);

		sprintf_s(putl->dat, putl->len, P("%s([{\"ip\":\"%s\",\"mac\":\"%s\"}])"), (LPCTSTR)callback, (LPCTSTR)ip_address, (LPCTSTR)mac_address);
	}

	if (_strnicmp(pud->http->url, P("http://pad.plustar.jp/ps_getend"), strlen(P("http://pad.plustar.jp/ps_getend"))) == 0)
	{
		IGNOREURLDATA* pData = 0;
		IgnoreUrl(&pData);

		sprintf_s(putl->dat, putl->len, "%d,%d,%d,%d", pData->ad_imp, pData->ad_ctr, pData->ad_esc, pData->ad_adl);
	}

	if (_strnicmp(pud->http->url, P("http://pad.plustar.jp/plustar-show"), strlen(P("http://pad.plustar.jp/plustar-show"))) == 0)
	{
		CText domain;

		if (!pud->hdr)
		{
			return 0;
		}

		const char* ref = strstr(pud->hdr, P("Referer:"));
		if (ref)
		{
			ref += strlen(P("Referer: "));
			GetDomain(ref, domain);
		}
	}

	if (_strnicmp(pud->http->url, P("http://pad.plustar.jp/plustar-header"), strlen(P("http://pad.plustar.jp/plustar-header"))) == 0)
	{
		CText domain;

		if (!pud->hdr)
		{
			return 0;
		}

		const char* ref = strstr(pud->hdr, P("Referer:"));
		if (ref)
		{
			ref += strlen(P("Referer: "));
			GetDomain(ref, domain);
		}
	}

	if (_strnicmp(pud->http->url, P("http://pad.plustar.jp/adhoc?"), strlen(P("http://pad.plustar.jp/adhoc?"))) == 0)
	{
		CText domain;

		if (!pud->hdr)
		{
			return 0;
		}

		const char* ref = strstr(pud->hdr, P("Referer:"));
		if (ref)
		{
			ref += strlen(P("Referer: "));
			GetDomain(ref, domain);
		}
	}

	return 0;
}

struct REP_DATA
{
	const char* pos;
	size_t len;
	const char* rep;
	size_t rease;
	REP_DATA* next;

	REP_DATA()
		: pos(NULL)
		, len(0)
		, rep(NULL)
		, rease(0)
		, next(NULL)
	{
	}
};

int MakeRddacam_mac(CString& rdda, LPCTSTR mac_address)
{
	rdda += mac_address[4];
	rdda += mac_address[3];

	rdda += mac_address[1];
	rdda += mac_address[0];

	rdda += mac_address[10];
	rdda += mac_address[9];

	rdda += mac_address[7];
	rdda += mac_address[6];

	rdda += mac_address[16];
	rdda += mac_address[15];

	rdda += mac_address[13];
	rdda += mac_address[12];

	return 0;
}

int MakeRddacam_ip(CString& rdda, LPCTSTR ip_address)
{
	int c_0 = strtol(ip_address, NULL, 10);
	int c_1 = 0;
	int c_2 = 0;
	int c_3 = 0;
	LPCTSTR pos = strchr(ip_address, '.');
	if (pos)
	{
		pos++;
		c_1 = strtol(pos, NULL, 10);

		pos = strchr(pos, '.');
		if (pos)
		{
			pos++;
			c_2 = strtol(pos, NULL, 10);

			pos = strchr(pos, '.');
			if (pos)
			{
				pos++;
				c_3 = strtol(pos, NULL, 10);
			}
		}
	}

	rdda.Format("%02X%02X%02X%02X", c_3, c_2, c_1, c_0);

	return 0;
}

struct UTL_CGI_DATA
{
	LPCTSTR org;
	LPCTSTR rep;
};

UTL_CGI_DATA _cgi[] = {
	P("%%MAC_ADDRESS%%"), 0,
	P("%%IP_ADDRESS%%"), 0,
	P("%%MAC_ADDRESS_REAL%%"), 0,
	P("%%IP_ADDRESS_REAL%%"), 0,
	P("%%START_TIME%%"), 0,
	P("%%ON_TIME%%"), 0,
	P("%%SEARCH_KEY%%"), 0,
	0, 0
};

int utl_cgi_replace(const struct putil_data* pud, void* data)
{
	utl_receive* putl = (utl_receive*)data;

	static CString ip_address;
	static CString mac_address;
	static CString rddacam;
	static CString rddapi;
	static CString start_time;

	if (ip_address.IsEmpty() || mac_address.IsEmpty())
	{
		for (; GetLocalInfo(P("dl.plustar.jp"), ip_address, mac_address) != 0; Sleep(2 SECOND))
		{
		}

		MakeRddacam_mac(rddacam, mac_address);
		MakeRddacam_ip(rddapi, ip_address);

		start_time = initialize_this_one::_start_time.Format(P("%Y%m%d%H%M%S"));

		_cgi[0].rep = rddacam;
		_cgi[1].rep = rddapi;
		_cgi[2].rep = mac_address;
		_cgi[3].rep = ip_address;
		_cgi[4].rep = start_time;
	}

	CString on_time;
	CString search_key;

	REP_DATA* anfi = 0;
	REP_DATA* targ = 0;
	size_t rease_total = 0;

	char* ppp = *putl->body;
	for (; ; )
	{
		ppp = strstr(ppp, P("%%"));
		if (!ppp)
		{
			break;
		}

		for (int i = 0; ; i++)
		{
			if (_cgi[i].org == 0)
			{
				ppp += 1;
				break;
			}

			if (memcmp(ppp, _cgi[i].org, strlen(_cgi[i].org)) == 0)
			{
				targ = targ ? targ->next = new REP_DATA : anfi = new REP_DATA;

				if (memcmp(P("%%ON_TIME%%"), _cgi[i].org, strlen(_cgi[i].org)) == 0)
				{
					on_time.Format(P("%u"), GetTickCount());
					_cgi[5].rep = on_time;
				}
				else if (memcmp(P("%%SEARCH_KEY%%"), _cgi[i].org, strlen(_cgi[i].org)) == 0)
				{
					SearchKey(search_key);
					_cgi[6].rep = search_key;
				}

				targ->pos = ppp;
				targ->len = strlen(_cgi[i].org);
				targ->rep = _cgi[i].rep;
				targ->rease = strlen(_cgi[i].rep) - targ->len;

				ppp += targ->len;
				rease_total += targ->rease;

				break;
			}
		}
	}

	if (anfi)
	{
		size_t cur_len = strlen(*putl->body);
		char* new_cur = (char*)putl->pads_malloc(cur_len + rease_total + 1);

		char* new_pos = new_cur;
		ppp = *putl->body;

		for (targ = anfi; targ; )
		{
			size_t len = targ->pos - ppp;
			memcpy(new_pos, ppp, len);
			new_pos += len;
			ppp += len + targ->len;
			memcpy(new_pos, targ->rep, strlen(targ->rep) + 1);
			new_pos += strlen(targ->rep);

			REP_DATA* dust = targ;
			targ = targ->next;
			delete dust;
		}

		size_t len = strlen(ppp);
		memcpy(new_pos, ppp, len);
		new_pos[len] = 0;

		putl->pads_free(*putl->body);
		*putl->body = new_cur;
		*putl->content_length = strlen(new_cur);
	}

	return 0;
}

int utl_filter_plustar(const struct putil_data* pud, void* data, plustar_page* page)
{
	int result = 0;

	IGNOREURLDATA* pData = 0;
	IgnoreUrl(&pData);

	if (pData->xsponsor)
	{
		pData->xsponsor--;

		pud->log_out(LOG_LEVEL_FORCE, "sponsor page (%s)", pud->http->url);
		// 32  ... sponsor page
		page->type = 32;
		result = -1;
		return result;
	}

	if (pData->xprint)
	{
		CONFDATA* pConfData = 0;
		ConfData(&pConfData);
		pConfData->count++;

		IGNOREURLDATA* pData = 0;
		IgnoreUrl(&pData);

		if (!pData->ad_ctr && pConfData->conf && pConfData->conf <= pConfData->count)
		{
			pud->log_out(LOG_LEVEL_FORCE, "x page (%s)", pud->http->url);
			// 28  ... “Á’è“X•Ü‚Å x ƒ{ƒ^ƒ“‚ª‰Ÿ‚³‚ê‚½
			page->type = 28;
			result = -1;
			return result;
		}

		pData->xprint--;

		int video_ign = 0;
		if (pData->video_ign == 1 && pData->xprint)
		{
			CText domain;
			GetDomain(pud->http->url, domain);

			if (domain.GetLength())
			{
				domain.ToLower();
				for (int i = 0; strict_url[i]; i++)
				{
					if (strstr(domain, strict_url[i]) != NULL)
					{
						video_ign++;
						break;
					}
				}

				if (video_ign == 0)
				{
					pData->xprint--;
				}
			}
		}

		pud->log_out(LOG_LEVEL_FORCE, "x page (%s)", pud->http->url);
		// 27  ... x ƒ{ƒ^ƒ“‚ª‰Ÿ‚³‚ê‚½
		page->type = 27;
		result = -1;
		return result;
	}

	if (pData->ignoreTotal)
	{
		COleDateTime now = COleDateTime::GetCurrentTime();
		if (now > pData->control)
		{
			pData->ignoreTotal--;
			if (pData->ignoreTotal < 0)
			{
				pData->ignoreTotal = 0;
			}

			COleDateTimeSpan limitations(0, 0, XCLICK_RESTORATION); // 1 minute
			pData->control = now + limitations;
		}
	}

	return result;
}

int utl_filter_first(const struct putil_data* pud, void* data, plustar_page* page)
{
	int result = 0;

	return result;
}

char* okpage[] = {
	"http://p.tl/",
	"http://ken-love.jp/",
	"http://www10.ocn.ne.jp/~gifu/",
	"http://www.gunma-bc.org/schedule/index.htm",
	"http://www.aomori.bc.jrc.or.jp/bus/bus_month.htm",
	"http://www.mie.bc.jrc.or.jp/bus/b_bus_nittei.html",
	0
};

int utl_filter_exemption(const struct putil_data* pud, void* data, plustar_page* page)
{
	int result = 0;

	for (int i = 0; okpage[i]; i++)
	{
		if (strcmp(pud->http->url, okpage[i]) == 0)
		{
			pud->log_out(LOG_LEVEL_FORCE, "ok page (%s)", pud->http->url);
			return 1;
		}
	}

	return result;
}

int utl_filter_head(const struct putil_data* pud, void* data, plustar_page* page)
{
	int result = 0;

	int textHtml = 0;
	for (list_entry* next = pud->headers->first; next; next = next->next)
	{
		CText text(next->str);
		text.ToLower();

		if (strstr(text, "content-type: text/html") || strstr(text, "content-type: text/css"))
		{
			if (strstr(text, "charset=euc-jp"))
			{
				page->charset = 1;
			}
			else if (strstr(text, "charset=shift-jis"))
			{
				page->charset = 2;
			}
			else
			{
				page->charset = 0;
			}
			textHtml++;
			break;
		}
	}

	if (!textHtml)
	{
		pud->log_out(LOG_LEVEL_FORCE, "no text/html (%s)", pud->http->url);
		// 12 ... text/html ‚Å‚Í‚È‚¢
		page->type = 12;
		result = -1;
		return result;
	}

	if (pud->hdr &&
		(strstr(pud->hdr, P("http://as.dc.impact-ad.jp/ADCLICK/CID=")) != 0 ||
		strstr(pud->hdr, P("http://ac.ebis.ne.jp/tr_set.php?argument=")) != 0))
	{
		pud->log_out(LOG_LEVEL_FORCE, "no text/html (%s)", pud->http->url);
		// 22  ... sponsor page (“]‘—æ)
		page->type = 22;
		result = -1;
		return result;
	}

	static int sp = GetStartPage();

	if (_stricmp(pud->http->url, StartPage()) == 0)
	{
		// 3 ... ƒuƒ‰ƒEƒU‚ÌƒXƒ^[ƒgƒy[ƒW
		page->type = 3;
		result = -1;
		return result;
	}

	return result;
}

/***************************************************************************
 * charset‚ÌoŒ»•p“x
 *        charsetboŒ»”b   ‘Î‰b   ˆ—b
 * ----------------------------------------b
 *          utf-8b 14236b     üb       b
 *      Shift_JISb  4088b     üb       b
 *         EUC-JPb  3712b     üb       b
 *          ASCIIb  3277b       b¨UTF-8b
 *       SJIS-winb  1326b     üb       b
 *               b   719b       b¨UTF-8b
 *     ISO-8859-1b   510b       b¨UTF-8b
 *         x-sjisb   172b     üb       b
 *      Shift-JISb   130b     üb       b
 *      eucJP-winb    70b     üb       b
 *    iso-2022-jpb    31b     ~b¨UTF-8b
 *            GBKb    20b       b¨UTF-8b
 *   windows-1252b    17b       b¨UTF-8b
 *       x-euc-jpb    16b     üb       b
 *         gb2312b    15b       b¨UTF-8b
 *    windows-31jb    10b       b¨UTF-8b
 *   windows-1251b    10b       b¨UTF-8b
 *           utf8b     6b       b¨UTF-8b
 *       US-ASCIIb     5b       b¨UTF-8b
 *         koi8-rb     4b       b¨UTF-8b
 *            JISb     4b     ~b¨UTF-8b
 *          koi8rb     4b       b¨UTF-8b
 *         euc-krb     3b       b¨UTF-8b
 *           big5b     3b       b¨UTF-8b
 *           sjisb     2b     üb       b
 *            EUCb     2b     üb       b
 * ks_c_5601-1987b     1b       b¨UTF-8b
 *
 * 0:UTF-8
 * 1:EUC-JP
 * 2:Shift_JIS
 *
 * HTML5‚Ìê‡
 * <meta http-equiv="Content-Type" content="text/html; charset=UTF-8"> 
 * ‚Æ‹Lq‚µ‚Ä‚¢‚Ü‚µ‚½‚ª‚±‚ê‚©‚ç‚Í
 * <meta charset="UTF-8"> 
 *
 **************************************************************************/
int GetCharset_(plustar_page* page)
{
	if (strstr(page->cur, "charset=utf-8")) { page->charset = 0; }
	//else if (strstr(page->cur, "charset=\"utf-8\"")) { page->charset = 0; }
	else if (strstr(page->cur, "charset=shift_jis")) { page->charset = 2; }
	//else if (strstr(page->cur, "charset=\"shift_jis\"")) { page->charset = 2; }
	else if (strstr(page->cur, "charset=euc-jp")) { page->charset = 1; }
	//else if (strstr(page->cur, "charset=\"euc-jp\"")) { page->charset = 1; }
	else if (strstr(page->cur, "charset=sjis-win")) { page->charset = 2; }
	else if (strstr(page->cur, "charset=x-sjis")) { page->charset = 2; }
	else if (strstr(page->cur, "charset=shift-jis")) { page->charset = 2; }
	else if (strstr(page->cur, "charset=eucjp-win")) { page->charset = 1; }
	else if (strstr(page->cur, "charset=x-euc-jp")) { page->charset = 1; }
	else if (strstr(page->cur, "charset=sjis")) { page->charset = 2; }
	else if (strstr(page->cur, "charset=euc")) { page->charset = 1; }

	return 0;
}

int GetCharset(plustar_page* page)
{
	char* hend = strstr(page->cur, "</head");
	if (hend)
	{
		*hend = 0;
	}

	int result = GetCharset_(page);

	if (hend)
	{
		*hend = '<';
	}

	return result;
}

int utl_filter_profile(const struct putil_data* pud, void* data, plustar_page* page)
{
	int result = 0;

	GetCharset(page);

	// ƒuƒ‰ƒbƒN”»’è
	result = IsWordPage(pud, page->charset, page, initialize_this_one::_blk, 6);
	if (result >= 6)
	{
		pud->log_out(LOG_LEVEL_FORCE, "black page (%s)", pud->http->url);
		// 38 ... ƒuƒ‰ƒbƒN
		page->type = 38;
		return result;
	}

	// ƒAƒ_ƒ‹ƒg”»’è
	result = IsWordPage(pud, page->charset, page, initialize_this_one::_adl, 6);
	if (result >= 6)
	{
		pud->log_out(LOG_LEVEL_FORCE, "adult page (%s)", pud->http->url);
		// 8 ... ƒAƒ_ƒ‹ƒg
		page->type = 8;
		return result;
	}

	return 0;
}

int utl_filter_all(const struct putil_data* pud, void* data, plustar_page* page)
{
	// –ß‚è’l
	int result = 0;

	result = utl_filter_first(pud, data, page);
	if (result == -1)
	{
		return result;
	}

	result = utl_filter_exemption(pud, data, page);
	if (result == 0)
	{
		// ƒwƒbƒ_î•ñ‚Ìƒ`ƒFƒbƒN
		result = utl_filter_head(pud, data, page);
		if (result == -1)
		{
			return result;
		}

		// <body>‚Ì’†g‚Ìƒ`ƒFƒbƒN
		result = utl_filter_body(pud, data, page);
		if (result == -1)
		{
			return result;
		}
	}

	// ƒvƒƒtƒ@ƒCƒ‹
	result = utl_filter_profile(pud, data, page);
	if (result)
	{
		return result;
	}

	// x ƒ{ƒ^ƒ“‚ª‰Ÿ‚³‚ê‚½
	result = utl_filter_plustar(pud, data, page);
	if (result == -1)
	{
		return result;
	}

	if (0) // iframe
	{
		size_t path_len = strlen(pud->http->url);
		size_t host_len = strlen(pud->http->host);
		IFR_DATA* pData = new IFR_DATA;
		pData->dom = (char*)malloc(host_len + 1);
		memcpy(pData->dom, pud->http->host, host_len);
		pData->dom[host_len] = NULL;
		pData->src = (char*)malloc(path_len + 1);
		memcpy(pData->src, pud->http->url, path_len);
		pData->src[path_len] = NULL;
		pData->type = 99; // page on iframe
		pData->next = NULL;
		AddIfr(pData);
	}

	pud->log_out(LOG_LEVEL_FORCE, "normal page (%s)", pud->http->url);
	// 0 ... ’Êíƒy[ƒW(‘å‘½”‚Ìˆê”Ê“I‚Èƒy[ƒW)
	page->type = 0;

	return result;
}

int get_key(const struct putil_data* pud, void* data)
{
	char* mabu = 0;

	char* opa = 0;
	char* search_host = strstr(pud->http->url, P("google.co.jp"));

	if (search_host)
	{
		opa = search_host + strlen(P("google.co.jp"));
		mabu = P("q=");
	}
	else
	{
		search_host = strstr(pud->http->url, P("yahoo.co.jp"));
		if (search_host)
		{
			opa = search_host + strlen(P("yahoo.co.jp"));
			mabu = P("p=");
		}
		else
		{
			search_host = strstr(pud->http->url, P("bing.com"));
			if (search_host)
			{
				opa = search_host + strlen(P("bing.com"));
				mabu = P("q=");
			}
		}
	}

	if (opa)
	{
		opa = strstr(opa, P("?"));
		if (opa)
		{
			opa++;
			char* key = strstr(opa, mabu);
			if (key)
			{
				key += strlen(mabu);
				opa = strstr(key, P("&"));
				if (opa)
				{
					opa[0] = 0;
				}

				CString search_key;
				search_key.Format(P("k=%s&t=%c"), key, search_host[0]);

				SearchKey(search_key);
			}
		}
	}

	return 0;
}

#define CONTENT_MAX (5 MB)

int utl_url_filter(const struct putil_data* pud, void* data)
{
	plustar_page page = { 0 };

	get_key(pud, data);

	page.content_len = pud->iob->eod - pud->iob->cur;
	CText cur(pud->iob->cur, page.content_len > CONTENT_MAX ? CONTENT_MAX : page.content_len);
	page.cur = cur;

	for (int len = strlen(page.cur); len < page.content_len; len = strlen(page.cur))
	{
		cur[len] = '\n';
	}

	cur.ToLower();

	utl_trance* utl = (utl_trance*)data;
	((int*)utl->reserve)[0] = utl_filter_all(pud, data, &page);

	IGNOREURLDATA* pData = 0;
	IgnoreUrl(&pData);

	if (*(int*)utl->reserve != 0 && *(int*)utl->reserve != -1)
	{
		pData->ad_adl++;
	}

	// î•ñ‚Ì‚Ü‚Æ‚ß
	sprintf_s(utl->dat, utl->len, "a=%d", page.type);

	return 0;
}

int read_file_line(FILE *fp, char **dest, int *newline, const int _MaxCount, const char* fname)
{
	// 0=OK, 3=EndOfFile

	for (int i = 0; _memData[i].name; i++)
	{
		if (strcmp(fname, _memData[i].name) == 0)
		{
			if (_memData[i].length == 0)
			{
				_memData[i].pos = (char*)_memData[i].org;
				_memData[i].length = strlen(_memData[i].pos);
				_memData[i].line = 0;

				return 3;
			}

			_memData[i].line++;
			*newline = _memData[i].line;

			const char* eol = strstr(_memData[i].pos, "\n");
			int line_length = eol ? (eol - _memData[i].pos) : _memData[i].length;

			if (line_length >= _MaxCount)
			{
				line_length = _MaxCount - 1;
			}

			memcpy((*dest), _memData[i].pos, line_length);
			(*dest)[line_length] = NULL;

			if (_memData[i].length > line_length)
			{
				_memData[i].pos += line_length + 1;
				_memData[i].length -= line_length;
			}
			else
			{
				_memData[i].pos = 0;
				_memData[i].length = 0;
			}

			return 0;
		}
	}

	return 3;
}

CMemStr GetCurrentVersionData(const char* fname)
{
	HANDLE hFile = CreateFile(fname, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return "";
	}

	DWORD dwReadHigh = 0;
	DWORD dwRead = GetFileSize(hFile, &dwReadHigh);
	CMemStr inside;
	ReadFile(hFile, inside.GetBufferSetLength(dwRead), dwRead, &dwRead, NULL);

	CloseHandle(hFile);

	return inside;
}

int SaveCurrentVersionData(const char* fname, const INT line, const CMemStr n_version, const CMemStr n_instname, const CMemStr n_fname)
{
	CMemStr inside = GetCurrentVersionData(fname);

	HANDLE hFile = CreateFile(fname, GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return -1;
	}

	// overwrite
	LARGE_INTEGER num = { 0 };
	SetFilePointerEx(hFile, num, NULL, FILE_BEGIN);

	int saveFlag = 0;

	INT pos = 0;
	for (INT i = 0; ; i++)
	{
		CMemStr o_version;
		CMemStr o_instname;
		CMemStr o_fname;

		inside.GetAbsData(pos, o_version);
		inside.GetAbsData(pos, o_instname);
		inside.GetAbsData(pos, o_fname);

		CMemStr wdata;
		if (i == line)
		{
			sprintf_s(wdata.GetBuffer(1024), 1024, "\"%s\" \"%s\" \"%s\"\n", (LPCTSTR)n_version, (LPCTSTR)n_instname, (LPCTSTR)n_fname);
			saveFlag++;
		}
		else
		{
			if (saveFlag && o_fname.IsEmpty())
			{
				break;
			}
			LPCTSTR w_version = strlen((LPCTSTR)o_version) ? (LPCTSTR)o_version : P("0000000000");
			LPCTSTR w_instname = strlen((LPCTSTR)o_instname) ? (LPCTSTR)o_instname : P("dummy");
			LPCTSTR w_fname = strlen((LPCTSTR)o_fname) ? (LPCTSTR)o_fname : P("dummy.exe");
			sprintf_s(wdata.GetBuffer(1024), 1024, "\"%s\" \"%s\" \"%s\"\n", w_version, w_instname, w_fname);
		}
		wdata.ReleaseBuffer();

		DWORD dwWriteHigh = 0;
		WriteFile(hFile, (LPCTSTR)wdata, wdata.GetLength(), &dwWriteHigh, NULL);
	}

	SetEndOfFile(hFile);

	CloseHandle(hFile);

	return 0;
}

int SaveCurrentExecData(const char* fname, const OHTTP_DATA& data)
{
	HANDLE hFile = CreateFile(fname, GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return -1;
	}

	DWORD dwWriteHigh = 0;
	WriteFile(hFile, (LPCTSTR)data.recvData, data.recvData.GetLength(), &dwWriteHigh, NULL);
	SetEndOfFile(hFile);

	CloseHandle(hFile);

	return 0;
}

#define UPDATE_URL P("http://dl.plustar.jp/pads/update/")
#define UPDATE_VER P("version_update.php")

#define UPD_NAME P("upd")
#define EXE_NAME P("inst.exe")

int UpdatePads(const INT line, const OHTTP_DATA& dataUp, INT& n_pos, const CMemStr& inside, INT& e_pos)
{
	CMemStr n_version;
	CMemStr n_instname;
	CMemStr n_fname;

	dataUp.recvData.GetAbsData(n_pos, n_version);
	dataUp.recvData.GetAbsData(n_pos, n_instname);
	dataUp.recvData.GetAbsData(n_pos, n_fname);

	if (n_fname.IsEmpty())
	{
		return -1; // end of update
	}

	CMemStr e_version;
	CMemStr e_instname;
	CMemStr e_fname;

	inside.GetAbsData(e_pos, e_version);
	inside.GetAbsData(e_pos, e_instname);
	inside.GetAbsData(e_pos, e_fname);

	INT n_ver = strtol(n_version, NULL, 10);
	if (n_ver == 0)
	{
		return 0;
	}

	INT e_ver = strtol(e_version, NULL, 10);
	if (n_ver <= e_ver)
	{
		return 0;
	}

	OHTTP_DATA dataExe;
	dataExe.target = UPDATE_URL;
	dataExe.target += n_fname;

	SaveCurrentVersionData(UPD_NAME, line, n_version, n_instname, n_fname);

	INT res = HttpGet(&dataExe);
	if (res != 0)
	{
		return 0;
	}

	if (dataExe.recvData.GetLength() < 70 KB)
	{
		return 0;
	}

	SaveCurrentExecData(EXE_NAME, dataExe);

#ifdef _DEBUG
	return 1;
#endif

	LPCTSTR path = GetModuleFolder();
	if (strstr(path, P("Projects")) != 0) // development
	{
		return 1;
	}

	RunProcess(EXE_NAME);

	TerminateProcess(NULL, 0);

	return 1; // "run Setup.exe"
}

int UpdatePadsRun()
{
	// W’†ƒAƒNƒZƒX–h~‚Ì‚½‚ßƒ‰ƒ“ƒ_ƒ€‘Î‰
	static CString ip_address;
	static CString mac_address;
	if (ip_address.IsEmpty() || mac_address.IsEmpty())
	{
		for (; GetLocalInfo(P("dl.plustar.jp"), ip_address, mac_address) != 0; Sleep(2 SECOND))
		{
		}
	}
	const char* sup = strrchr(ip_address, '.');
	int nsup = sup ? strtoul(sup + 1, NULL, 10) : 0;
	srand(nsup);
	DWORD dwMiliseconds = rand();
	dwMiliseconds %= (15 SECOND); // n•bŠÔ‚Åƒo[ƒWƒ‡ƒ“î•ñæ“¾
	Sleep(dwMiliseconds);

	OHTTP_DATA dataUp;
	dataUp.target = UPDATE_URL;
	dataUp.target += UPDATE_VER;

	char crc[] = "CRC:" __DATE__ " " __TIME__;
	int crclen = strlen(crc);
	for (int i = 0; crc[i]; i++) if (crc[i] == ' ') crc[i] = '-';
	dataUp.target += "?t=";
	dataUp.target += crc + 4;

	INT res = HttpGet(&dataUp);
	if (res != 0)
	{
		return 0;
	}

	char* pVer = (LPTSTR)dataUp.recvData;
	int lenVer = dataUp.recvData.GetLength();
	if (pVer[0] != '"')
	{
		return 0;
	}
	if (memcmp(&pVer[lenVer - crclen], crc, crclen + 1))
	{
		return 0;
	}

	Sleep(5 SECOND);
	Sleep(dwMiliseconds * 2); // n*2•bŠÔ‚ÅƒAƒbƒvƒf[ƒgæ“¾

	const CMemStr inside = GetCurrentVersionData(UPD_NAME);

	INT n_pos = 0;
	INT e_pos = 0;

	for (INT i = 0; UpdatePads(i, dataUp, n_pos, inside, e_pos) != -1; i++)
	{
	}

	return 0;
}

DWORD WINAPI UpdatePadsThread(LPVOID pData)
{
	for (; ; )
	{
		Sleep(1000);

		int UpdateHookTime();
		UpdateHookTime();

		int CheckHookTime();
		if (CheckHookTime())
		{
			UpdatePadsRun();
			break;
		}
	}

	return 0;
}

int update_pads()
{
	return 0; // ƒAƒbƒvƒf[ƒgÀs‚Í pWebBP ‚ÉˆÚ“®

	HANDLE hThread = CreateThread(NULL, 0, UpdatePadsThread, NULL, 0, NULL);
	CloseHandle(hThread);

	return 0;
}

int InitializeInternet()
{
	SetInternetSettings();
	SetInternetExplorerMain();
	SetInternetSecurity();

	InternetSetOption(NULL, INTERNET_OPTION_REFRESH, NULL, 0);
	// The connection settings for other instances of Internet Explorer.

	InternetSetOption(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0);
	SendNotifyMessage(HWND_BROADCAST, WM_SETTINGCHANGE, (WPARAM)(0), (LPARAM)(0));

	return 0;
}

#define PADS_CTRL_URL P("http://dl.plustar.jp/pads/conf.php")

int InitializeConf()
{
	WORD wVersionRequested = MAKEWORD(2, 0);
	WSADATA wsaData = { 0 };
	WSAStartup(wVersionRequested, &wsaData);

	OHTTP_DATA dataUp;
	dataUp.target = PADS_CTRL_URL;

	char crc[] = "CRC:" __DATE__ " " __TIME__;
	int crclen = strlen(crc);
	for (int i = 0; crc[i]; i++) if (crc[i] == ' ') crc[i] = '-';
	dataUp.target += "?t=";
	dataUp.target += crc + 4;

	INT res = HttpGet(&dataUp);
	if (res != 0)
	{
		return 0;
	}

	char* pRecvData = (LPTSTR)dataUp.recvData;
	int lenRecv = dataUp.recvData.GetLength();
	if (memcmp(pRecvData, "ps.conf\n", strlen("ps.conf\n")) != 0)
	{
		return 0;
	}
	if (memcmp(&pRecvData[lenRecv - crclen], crc, crclen + 1))
	{
		return 0;
	}

	CONFDATA* pConfData = 0;
	ConfData(&pConfData);

	pConfData->sire = strtol(NextConfData(pRecvData), NULL, 10);
	pConfData->conf = strtol(NextConfData(pRecvData), NULL, 10);
	pConfData->na = strtol(NextConfData(pRecvData), NULL, 10);
	pConfData->nb = strtol(NextConfData(pRecvData), NULL, 10);
	pConfData->nc = strtol(NextConfData(pRecvData), NULL, 10);
	pConfData->nd = strtol(NextConfData(pRecvData), NULL, 10);

	return 0;
}

int utl_initialize()
{
	LPCTSTR path = GetModuleFolder();
	SetCurrentDirectory(path);

	InitializeConf();

#if 0
	InitializeInternet();
#endif

	return 0;
}

extern "C" __declspec(dllexport) int u0(const struct putil_data* pud, void* data){ LPSTR(0)[0] = ((char*)data)[0]; return 0; }
extern "C" __declspec(dllexport) int u1(const struct putil_data* pud, void* data){ LPSTR(0)[0] = ((char*)data)[0]; return 0; }
extern "C" __declspec(dllexport) int u2(const struct putil_data* pud, void* data){ return utl_initialize(); }
extern "C" __declspec(dllexport) int u3(const struct putil_data* pud, void* data){ LPSTR(0)[0] = ((char*)data)[0]; return 0; }
extern "C" __declspec(dllexport) int u4(const struct putil_data* pud, void* data){ return utl_url_check(pud, data); }
extern "C" __declspec(dllexport) int u5(const struct putil_data* pud, void* data){ LPSTR(0)[0] = ((char*)data)[0]; return 0; }
extern "C" __declspec(dllexport) int u6(const struct putil_data* pud, void* data){ LPSTR(0)[0] = ((char*)data)[0]; return 0; }
extern "C" __declspec(dllexport) int u7(const struct putil_data* pud, void* data){ LPSTR(0)[0] = ((char*)data)[0]; return 0; }
extern "C" __declspec(dllexport) int u8(){ return update_pads(); }
extern "C" __declspec(dllexport) int u9(const struct putil_data* pud, void* data){ LPSTR(0)[0] = ((char*)data)[0]; return 0; }
extern "C" __declspec(dllexport) int u10(const struct putil_data* pud, void* data){ return utl_url_redirect(pud, data); }
extern "C" __declspec(dllexport) int u11(FILE *fp, char **dest, int *newline, const int _MaxCount, const char* fname){ return read_file_line(fp, dest, newline, _MaxCount, fname); }
extern "C" __declspec(dllexport) int u12(const struct putil_data* pud, void* data){ return utl_url_filter(pud, data); }
extern "C" __declspec(dllexport) int u13(const struct putil_data* pud, void* data){ LPSTR(0)[0] = ((char*)data)[0]; return 0; }
extern "C" __declspec(dllexport) int u14(const struct putil_data* pud, void* data){ return utl_url_trance(pud, data); }
extern "C" __declspec(dllexport) int u15(const struct putil_data* pud, void* data){ LPSTR(0)[0] = ((char*)data)[0]; return 0; }
extern "C" __declspec(dllexport) int u16(const struct putil_data* pud, void* data){ LPSTR(0)[0] = ((char*)data)[0]; return 0; }
extern "C" __declspec(dllexport) int u17(const struct putil_data* pud, void* data){ return utl_cgi_replace(pud, data); }
extern "C" __declspec(dllexport) int u18(const struct putil_data* pud, void* data){ LPSTR(0)[0] = ((char*)data)[0]; return 0; }
extern "C" __declspec(dllexport) int u19(const struct putil_data* pud, void* data){ LPSTR(0)[0] = ((char*)data)[0]; return 0; }
extern "C" __declspec(dllexport) int u20(const struct putil_data* pud, void* data){ LPSTR(0)[0] = ((char*)data)[0]; return 0; }
extern "C" __declspec(dllexport) int u21(const struct putil_data* pud, void* data){ LPSTR(0)[0] = ((char*)data)[0]; return 0; }
