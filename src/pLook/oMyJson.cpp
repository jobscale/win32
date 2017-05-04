#define __OMYJSON_CPP__
#include <windows.h>
#include "oMyJson.h"

namespace omyjson
{
	func_create_Value    create_Value    = NULL;
	func_create_AValue   create_AValue   = NULL;
	func_create_OValue   create_OValue   = NULL;
	func_create_FWriter  create_FWriter  = NULL;
	func_create_SWriter  create_SWriter  = NULL;
	func_create_SSWriter create_SSWriter = NULL;
	func_create_Reader   create_Reader   = NULL;

	int InitMyJson(HMODULE hModule)
	{
		//pSpiceに組み込まれたのでdllのロードはそちらで行います。
		//HMODULE hModule = LoadLibrary("jsoncpp.dll");

		create_Value    = (func_create_Value)   GetProcAddress(hModule, "create_Value");
		create_AValue   = (func_create_AValue)  GetProcAddress(hModule, "create_AValue");
		create_OValue   = (func_create_OValue)  GetProcAddress(hModule, "create_OValue");
		create_FWriter  = (func_create_FWriter) GetProcAddress(hModule, "create_FWriter");
		create_SWriter  = (func_create_SWriter) GetProcAddress(hModule, "create_SWriter");
		create_SSWriter = (func_create_SSWriter)GetProcAddress(hModule, "create_SSWriter");
		create_Reader   = (func_create_Reader)  GetProcAddress(hModule, "create_Reader");

		return 0;
	}

	//pSpiceに組み込まれたのでdllのロードはそちらで行います。
	//int init_myjson = InitMyJson();
}
