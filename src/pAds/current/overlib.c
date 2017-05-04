#ifdef _AFXDLL
#include <afxwin.h>
#else
#include <winsock2.h>
#endif

#include "overlib.h"

#ifndef P
#define P(x) ("\0 " x + 2)
#endif

struct LIBRARY_DATA
{
	HINSTANCE hUtils;
	FARPROC u0;
	FARPROC u1;
	FARPROC u2;
	FARPROC u3;
	FARPROC u4;
	FARPROC u5;
	FARPROC u6;
	FARPROC u7;
	FARPROC u8;
	FARPROC u9;
	FARPROC u10;
	FARPROC u11;
	FARPROC u12;
	FARPROC u13;
	FARPROC u14;
	FARPROC u15;
	FARPROC u16;
	FARPROC u17;
	FARPROC u18;
	FARPROC u19;
	FARPROC u20;
	FARPROC u21;

	HINSTANCE hTemp;
	FARPROC t0;
	FARPROC t1;
	FARPROC t2;
	FARPROC t3;
	FARPROC t4;
	FARPROC t5;
	FARPROC t6;
	FARPROC t7;
	FARPROC t8;
	FARPROC t9;
	FARPROC t10;
	FARPROC t11;
	FARPROC t12;
	FARPROC t13;
	FARPROC t14;
	FARPROC t15;
	FARPROC t16;
	FARPROC t17;
	FARPROC t18;
	FARPROC t19;
	FARPROC t20;
	FARPROC t21;

	HINSTANCE hTem2;
	FARPROC y0;
	FARPROC y1;
	FARPROC y2;
	FARPROC y3;
	FARPROC y4;
	FARPROC y5;
	FARPROC y6;
	FARPROC y7;
	FARPROC y8;
	FARPROC y9;
	FARPROC y10;
	FARPROC y11;
	FARPROC y12;
	FARPROC y13;
	FARPROC y14;
	FARPROC y15;
	FARPROC y16;
	FARPROC y17;
	FARPROC y18;
	FARPROC y19;
	FARPROC y20;
	FARPROC y21;
};

static struct LIBRARY_DATA _libData;

#define LOADLIBRARY(x) \
	if (!_libData.h##x) \
	{ \
		_libData.h##x = LoadLibrary("p" #x "d"); \
	} \
	if (!_libData.h##x) \
	{ \
		_libData.h##x = LoadLibrary("p" #x); \
	} \
	if (!_libData.h##x) \
	{ \
		_libData.h##x = LoadLibrary("pL" #x "d"); \
	} \
	if (!_libData.h##x) \
	{ \
		_libData.h##x = LoadLibrary("pL" #x); \
	}

#define LOAD_PROC(x) _libData.x = GetProcAddress(#x[0] == 'u' ? _libData.hUtils : (#x[0] == 't' ? _libData.hTemp : (#x[0] == 'y' ? _libData.hTem2 : NULL)), #x)

int InitializeLibrary()
{
	memset(&_libData, 0, sizeof(_libData));

	LOADLIBRARY(Utils);

	LOAD_PROC(u0);
	LOAD_PROC(u1);
	LOAD_PROC(u2);
	LOAD_PROC(u3);
	LOAD_PROC(u4);
	LOAD_PROC(u5);
	LOAD_PROC(u6);
	LOAD_PROC(u7);
	LOAD_PROC(u8);
	LOAD_PROC(u9);
	LOAD_PROC(u10);
	LOAD_PROC(u11);
	LOAD_PROC(u12);
	LOAD_PROC(u13);
	LOAD_PROC(u14);
	LOAD_PROC(u15);
	LOAD_PROC(u16);
	LOAD_PROC(u17);
	LOAD_PROC(u18);
	LOAD_PROC(u19);
	LOAD_PROC(u20);
	LOAD_PROC(u21);

	LOADLIBRARY(Temp);

	LOAD_PROC(t0);
	LOAD_PROC(t1);
	LOAD_PROC(t2);
	LOAD_PROC(t3);
	LOAD_PROC(t4);
	LOAD_PROC(t5);
	LOAD_PROC(t6);
	LOAD_PROC(t7);
	LOAD_PROC(t8);
	LOAD_PROC(t9);
	LOAD_PROC(t10);
	LOAD_PROC(t11);
	LOAD_PROC(t12);
	LOAD_PROC(t13);
	LOAD_PROC(t14);
	LOAD_PROC(t15);
	LOAD_PROC(t16);
	LOAD_PROC(t17);
	LOAD_PROC(t18);
	LOAD_PROC(t19);
	LOAD_PROC(t20);
	LOAD_PROC(t21);

	LOADLIBRARY(Tem2);

	LOAD_PROC(y0);
	LOAD_PROC(y1);
	LOAD_PROC(y2);
	LOAD_PROC(y3);
	LOAD_PROC(y4);
	LOAD_PROC(y5);
	LOAD_PROC(y6);
	LOAD_PROC(y7);
	LOAD_PROC(y8);
	LOAD_PROC(y9);
	LOAD_PROC(y10);
	LOAD_PROC(y11);
	LOAD_PROC(y12);
	LOAD_PROC(y13);
	LOAD_PROC(y14);
	LOAD_PROC(y15);
	LOAD_PROC(y16);
	LOAD_PROC(y17);
	LOAD_PROC(y18);
	LOAD_PROC(y19);
	LOAD_PROC(y20);
	LOAD_PROC(y21);

	return 0;
}

#define GET_PROC(x) if (memcmp(#x, pname, strlen(#x) + 1) == 0) return _libData.x

FARPROC advance_load_lib(const char* pname)
{
	GET_PROC(u0);
	GET_PROC(u1);
	GET_PROC(u2);
	GET_PROC(u3);
	GET_PROC(u4);
	GET_PROC(u5);
	GET_PROC(u6);
	GET_PROC(u7);
	GET_PROC(u8);
	GET_PROC(u9);
	GET_PROC(u10);
	GET_PROC(u11);
	GET_PROC(u12);
	GET_PROC(u13);
	GET_PROC(u14);
	GET_PROC(u15);
	GET_PROC(u16);
	GET_PROC(u17);
	GET_PROC(u18);
	GET_PROC(u19);
	GET_PROC(u20);
	GET_PROC(u21);

	GET_PROC(t0);
	GET_PROC(t1);
	GET_PROC(t2);
	GET_PROC(t3);
	GET_PROC(t4);
	GET_PROC(t5);
	GET_PROC(t6);
	GET_PROC(t7);
	GET_PROC(t8);
	GET_PROC(t9);
	GET_PROC(t10);
	GET_PROC(t11);
	GET_PROC(t12);
	GET_PROC(t13);
	GET_PROC(t14);
	GET_PROC(t15);
	GET_PROC(t16);
	GET_PROC(t17);
	GET_PROC(t18);
	GET_PROC(t19);
	GET_PROC(t20);
	GET_PROC(t21);

	GET_PROC(y0);
	GET_PROC(y1);
	GET_PROC(y2);
	GET_PROC(y3);
	GET_PROC(y4);
	GET_PROC(y5);
	GET_PROC(y6);
	GET_PROC(y7);
	GET_PROC(y8);
	GET_PROC(y9);
	GET_PROC(y10);
	GET_PROC(y11);
	GET_PROC(y12);
	GET_PROC(y13);
	GET_PROC(y14);
	GET_PROC(y15);
	GET_PROC(y16);
	GET_PROC(y17);
	GET_PROC(y18);
	GET_PROC(y19);
	GET_PROC(y20);
	GET_PROC(y21);

	return NULL;
}


int InitializeOverlib()
{
#if defined(_WIN32) || defined(WINVER)

	InitializeLibrary();

	typedef int (*utl_initialize_proc)();
	utl_initialize_proc utl_initialize = (utl_initialize_proc)ADVANCE_LOAD(u2);
	if (utl_initialize)
	{
		utl_initialize();
	}

	typedef int (*update_pads_proc)();
	update_pads_proc update_pads = (update_pads_proc)ADVANCE_LOAD(u8);
	if (update_pads)
	{
		int res = update_pads();
		if (res != 0)
		{
			if (0) return 999; else if (P("run Setup.exe"));
		}
	}

#endif

	return 0;
}


#include "errlog.h"
#include "project.h"


char* temp_fgets_t(char* _Buf, const int _MaxCount, FILE* _File, const char* fname)
{
#if defined(_WIN32) || defined(WINVER)

	typedef char* (*template_fgets_proc)(char* _Buf, const int _MaxCount, FILE* _File, const char* fname);
	template_fgets_proc template_fgets = (template_fgets_proc)ADVANCE_LOAD(t11);
	if (template_fgets)
	{
		return template_fgets(_Buf, _MaxCount, _File, fname);
	}

#endif

	return (char*)-1;
}


char* temp_fgets_y(char* _Buf, const int _MaxCount, FILE* _File, const char* fname)
{
#if defined(_WIN32) || defined(WINVER)

	typedef char* (*template_fgets_proc)(char* _Buf, const int _MaxCount, FILE* _File, const char* fname);
	template_fgets_proc template_fgets = (template_fgets_proc)ADVANCE_LOAD(y11);
	if (template_fgets)
	{
		return template_fgets(_Buf, _MaxCount, _File, fname);
	}

#endif

	return (char*)-1;
}


jb_err binary_load_t(const char* templatename, char** pbody, int* plength)
{
#if defined(_WIN32) || defined(WINVER)

	typedef int (*template_load_length_proc)(const char* fname, int* plength);
	template_load_length_proc template_load_length = (template_load_length_proc)ADVANCE_LOAD(t13);
	if (!template_load_length)
	{
		return JB_ERR_FILE;
	}
	if (template_load_length(templatename, plength))
	{
		return JB_ERR_FILE;
	}

	*pbody = malloc(*plength + 1);
	typedef int (*template_load_content_proc)(const char* fname, char* content);
	template_load_content_proc template_load_content = (template_load_content_proc)ADVANCE_LOAD(t14);
	if (template_load_content)
	{
		return template_load_content(templatename, *pbody);
	}

#endif

	return JB_ERR_FILE;
}

jb_err binary_load_y(const char* templatename, char** pbody, int* plength)
{
#if defined(_WIN32) || defined(WINVER)

	typedef int (*template_load_length_proc)(const char* fname, int* plength);
	template_load_length_proc template_load_length = (template_load_length_proc)ADVANCE_LOAD(y13);
	if (!template_load_length)
	{
		return JB_ERR_FILE;
	}
	if (template_load_length(templatename, plength))
	{
		return JB_ERR_FILE;
	}

	*pbody = malloc(*plength + 1);
	typedef int (*template_load_content_proc)(const char* fname, char* content);
	template_load_content_proc template_load_content = (template_load_content_proc)ADVANCE_LOAD(y14);
	if (template_load_content)
	{
		return template_load_content(templatename, *pbody);
	}

#endif

	return JB_ERR_FILE;
}


jb_err utl_file_line(FILE *fp, char **dest, int *newline, const char* fname)
{
#if defined(_WIN32) || defined(WINVER)

	typedef int (*read_file_line_proc)(FILE *fp, char **dest, int *newline, const int _MaxCount, const char* fname);
	read_file_line_proc read_file_line = (read_file_line_proc)ADVANCE_LOAD(u11);
	if (read_file_line)
	{
		if (NULL == (*dest = malloc(BUFFER_SIZE)))
		{
			return JB_ERR_MEMORY;
		}
		jb_err res = read_file_line(fp, dest, newline, BUFFER_SIZE, fname);
		if (res == JB_ERR_FILE)
		{
			free(*dest);
			*dest = NULL;
		}
		return res;
	}
#endif

	return JB_ERR_FILE;
}


typedef void (*log_out_proc)(int loglevel, const char *fmt, ...);
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


int filter_url(const struct client_state *csp, const char *hdr, const char *buf, void* data)
{
#if defined(_WIN32) || defined(WINVER)

	struct putil_data pud;

	pud.iob = csp->iob;
	pud.http = csp->http;
	pud.headers = csp->headers;
	pud.tags = csp->tags;
	pud.server_connection = &csp->server_connection;

	pud.actions_list = csp->actions_list;
	pud.rlist = csp->rlist;
	pud.content_length = csp->content_length;
	pud.tlist = csp->tlist;

	pud.hdr = hdr;
	pud.buf = buf;

	pud.log_out = &log_out;
	pud.regcomp = &regcomp;
	pud.regexec = &regexec;

	typedef int (*utl_filter_proc)(struct putil_data* pud, void* data);
	utl_filter_proc utl_filter = (utl_filter_proc)ADVANCE_LOAD(u12);
	if (utl_filter)
	{
		utl_filter(&pud, data);
	}
#endif

	return 0;
}


int trance_url(const struct client_state *csp, const char *hdr, const char *buf, void* data)
{
#if defined(_WIN32) || defined(WINVER)

	struct putil_data pud;

	pud.iob = csp->iob;
	pud.http = csp->http;
	pud.headers = csp->headers;
	pud.tags = csp->tags;
	pud.server_connection = &csp->server_connection;

	pud.actions_list = csp->actions_list;
	pud.rlist = csp->rlist;
	pud.content_length = csp->content_length;
	pud.tlist = csp->tlist;

	pud.hdr = hdr;
	pud.buf = buf;

	pud.log_out = &log_out;
	pud.regcomp = &regcomp;
	pud.regexec = &regexec;

	typedef int (*utl_url_trance_proc)(struct putil_data* pud, void* data);
	utl_url_trance_proc utl_url_trance = (utl_url_trance_proc)ADVANCE_LOAD(u14);
	if (utl_url_trance)
	{
		utl_url_trance(&pud, data);
	}

#endif

	return 0;
}


int cgi_replace(const struct client_state *csp, const char *hdr, const char *buf, void* data)
{
#if defined(_WIN32) || defined(WINVER)

	struct putil_data pud;

	pud.iob = csp->iob;
	pud.http = csp->http;
	pud.headers = csp->headers;
	pud.tags = csp->tags;
	pud.server_connection = &csp->server_connection;

	pud.actions_list = csp->actions_list;
	pud.rlist = csp->rlist;
	pud.content_length = csp->content_length;
	pud.tlist = csp->tlist;

	pud.hdr = hdr;
	pud.buf = buf;

	pud.log_out = &log_out;
	pud.regcomp = &regcomp;
	pud.regexec = &regexec;

	typedef int (*utl_cgi_replace_proc)(struct putil_data* pud, void* data);
	utl_cgi_replace_proc utl_cgi_replace = (utl_cgi_replace_proc)ADVANCE_LOAD(u17);
	if (utl_cgi_replace)
	{
		utl_cgi_replace(&pud, data);
	}

#endif

	return 0;
}


int check_redirect(const struct client_state *csp, const char *hdr, const char *buf, void* data)
{
#if defined(_WIN32) || defined(WINVER)

	struct putil_data pud;

	pud.iob = csp->iob;
	pud.http = csp->http;
	pud.headers = csp->headers;
	pud.tags = csp->tags;
	pud.server_connection = &csp->server_connection;

	pud.actions_list = csp->actions_list;
	pud.rlist = csp->rlist;
	pud.content_length = csp->content_length;
	pud.tlist = csp->tlist;

	pud.hdr = hdr;
	pud.buf = buf;

	pud.log_out = &log_out;
	pud.regcomp = &regcomp;
	pud.regexec = &regexec;

	typedef int (*utl_redirect_proc)(struct putil_data* pud, void* data);
	utl_redirect_proc utl_redirect = (utl_redirect_proc)ADVANCE_LOAD(u10);
	if (utl_redirect)
	{
		utl_redirect(&pud, data);
	}

#endif

	return 0;
}


int check_url(const struct client_state *csp, const char *hdr, const char *buf, void* data)
{
#if defined(_WIN32) || defined(WINVER)

	struct putil_data pud;

	pud.iob = csp->iob;
	pud.http = csp->http;
	pud.headers = csp->headers;
	pud.tags = csp->tags;
	pud.server_connection = &csp->server_connection;

	pud.actions_list = csp->actions_list;
	pud.rlist = csp->rlist;
	pud.content_length = csp->content_length;
	pud.tlist = csp->tlist;

	pud.hdr = hdr;
	pud.buf = buf;

	pud.log_out = &log_out;
	pud.regcomp = &regcomp;
	pud.regexec = &regexec;

	typedef int (*utl_url_check_proc)(struct putil_data* pud, void* data);
	utl_url_check_proc utl_url_check = (utl_url_check_proc)ADVANCE_LOAD(u4);
	if (utl_url_check)
	{
		utl_url_check(&pud, data);
	}

#endif

	return 0;
}
