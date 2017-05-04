#ifndef __O_MY_JSON_H__
#define __O_MY_JSON_H__

#define USE_JSONCPP //評価必要ということなのでコードを切り分けます

#include <windows.h>
#include "../jsoncpp/json_create.h"

namespace omyjson
{
	extern func_create_Value    create_Value;
	extern func_create_AValue   create_AValue;
	extern func_create_OValue   create_OValue;
	extern func_create_FWriter  create_FWriter;
	extern func_create_SWriter  create_SWriter;
	extern func_create_SSWriter create_SSWriter;
	extern func_create_Reader   create_Reader;

	int InitMyJson(HMODULE);
}

#ifndef __OMYJSON_CPP__
#define create_Value    omyjson::create_Value
#define create_AValue   omyjson::create_AValue
#define create_OValue   omyjson::create_OValue
#define create_FWriter  omyjson::create_FWriter
#define create_SWriter  omyjson::create_SWriter
#define create_SSWriter omyjson::create_SSWriter
#define create_Reader   omyjson::create_Reader
#endif

#endif //__O_MY_JSON_H__