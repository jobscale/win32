#include "json_create.h"

namespace Json {

Value* create_Value()
{
	return new Value();
}

Value* create_AValue()
{
	return new Value(arrayValue);
}

Value* create_OValue()
{
	return new Value(objectValue);
}

FastWriter* create_FWriter()
{
	return new FastWriter();
}

StyledWriter* create_SWriter()
{
	return new StyledWriter();
}

StyledStreamWriter* create_SSWriter()
{
	return new StyledStreamWriter();
}

Reader* create_Reader()
{
	return new Reader();
}

} // namespace Json