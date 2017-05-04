#ifndef __OVER_LIB_H__
#define __OVER_LIB_H__

extern int InitializeLibrary();
extern FARPROC advance_load_lib(const char* pname);

#define ADVANCE_LOAD(x) advance_load_lib("\0 " #x + 2);

#endif // __OVER_LIB_H__
