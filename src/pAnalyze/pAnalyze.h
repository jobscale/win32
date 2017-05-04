#ifndef _ANALYZE_H_
#define _ANALYZE_H_

class CAnalyze
{
public:
	CAnalyze();
	virtual ~CAnalyze();
	size_t Length(BSTR str);
};

typedef HANDLE (* ca_Create_func)();
typedef void (* ca_Release_func)(HANDLE src);
typedef size_t (* ca_Length_func)(HANDLE mod, BSTR src);

#endif // _ANALYZE_H_
