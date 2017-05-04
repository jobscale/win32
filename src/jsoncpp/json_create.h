#ifndef __JSON_CREATE_H__
#define __JSON_CREATE_H__

#include "json.h"

#if defined(JSONCPP_EXPORTS) || defined(PSPICE_EXPORTS)
# define JSONCPP_API __declspec(dllexport)
#else
# define JSONCPP_API
#endif

typedef Json::Value*              (*func_create_Value)(void);
typedef Json::Value*              (*func_create_AValue)(void);
typedef Json::Value*              (*func_create_OValue)(void);
typedef Json::FastWriter*         (*func_create_FWriter)(void);
typedef Json::StyledWriter*       (*func_create_SWriter)(void);
typedef Json::StyledStreamWriter* (*func_create_SSWriter)(void);
typedef Json::Reader*             (*func_create_Reader)(void);

namespace Json {
	extern "C" {
		JSONCPP_API Value*              create_Value();
		JSONCPP_API Value*              create_AValue(); // Value(arrayValue)
		JSONCPP_API Value*              create_OValue(); // Value(objectValue)
		JSONCPP_API FastWriter*         create_FWriter();
		JSONCPP_API StyledWriter*       create_SWriter();
		JSONCPP_API StyledStreamWriter* create_SSWriter();
		JSONCPP_API Reader*             create_Reader();
	}
}

#endif //__JSON_CREATE_H__