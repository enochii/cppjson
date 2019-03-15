
#include <cassert>
//#include <string>

#include "Json.h"
#define TEST

//base class, generalize the value type
class Value {
protected:
	ValueType tag;
public:

	ValueType getType() { return tag; }
	Value(ValueType tag) :tag(tag) {}

	//getter
	///*
	virtual bool getBoolUnsafe();
	virtual double getDouleUnsafe();
	virtual int getIntUnsafe();
	virtual Json::array& getArrayUnsafe();
	virtual Json::object& getObjectUnsafe();
	virtual std::string& getStringUnsafe();
	//*/


	Value() {}
	virtual ~Value() {}
};

//发现作用鸡肋的JsonValue所处的位置有点尴尬
template <typename T>
class JsonValue :public Value {
protected:
	T value;//
public:
	JsonValue(ValueType tag, T&& val) :Value(tag), value(std::move(val)) {
#ifdef TEST
		std::cout << "move me!?\n";
#endif
	}
	JsonValue(ValueType tag, const T& val) : Value(tag), value(val) {}
	virtual ~JsonValue(){}
};

class JsonNull :public JsonValue<void*> {};

class JsonBool :public JsonValue<bool> {
public:
	bool getBoolUnsafe()override
	{
		return value;
	}
	explicit JsonBool(bool b):JsonValue(BOOLEAN, b){}
};

class JsonInt :public JsonValue<int> {
public:
	int getIntUnsafe()override
	{
		return value;
	}
	explicit JsonInt(int i):JsonValue(INT, i){}
};

class JsonDouble :public JsonValue<double> {
public:
	double getDoubleUnsafe() 
	{
		return value;
	}
	explicit JsonDouble(double d):JsonValue(DOUBLE, d){}
};

class JsonArray:public JsonValue<Json::array>{
public:
	Json::array& getArrayUnsafe()
	{
		return value;
	}
	explicit JsonArray(const Json::array& arr):JsonValue(ARRAY,arr){}
	explicit JsonArray(Json::array&& arr) :JsonValue(ARRAY, arr) {}
};

class JsonObject :public JsonValue<Json::object> {
public:
	Json::object& getObjectUnsafe() 
	{
		return value;
	}
	explicit JsonObject(const Json::object& obj):JsonValue(OBJECT, obj){}
	explicit JsonObject(Json::object&& obj) :JsonValue(OBJECT, obj) {}
};

class JsonString :public JsonValue<std::string> {
public:
	std::string& getStringUnsafe()override
	{
		return value;
	}
	explicit JsonString(const std::string& s):JsonValue(STRING, s){}
	explicit JsonString(std::string&& s) :JsonValue(STRING, s) {}
};

//
bool Value::getBoolUnsafe()
{
	return false;
}

int Value::getIntUnsafe()
{
	return 0;
}

double Value::getDouleUnsafe()
{
	return 0.0;
}

Json::array& Value::getArrayUnsafe()
{
	static Json::array sarr;
	return sarr;
}

Json::object& Value::getObjectUnsafe()
{
	static Json::object sobj;
	return sobj;
}

std::string& Value::getStringUnsafe()
{
	static std::string s;
	return s;
}

///*
//模板的非类型参数比如这里的tag推导不出来，总感觉肯定用词不当
template<typename T>
JsonValue<T> make_jsonValue(ValueType tag, T&& value)
{
	return JsonValue<T>(tag, std::move(value));
}
//*/

inline int Json::getType()const
{
	return mptr->getType();
}

bool Json::isArray()const
{
	return getType() == ARRAY;
}

bool Json::isBool()const
{
	return getType() == BOOLEAN;
}

bool Json::isDouble()const
{
	return getType() == DOUBLE;
}

bool Json::isInt()const
{
	return getType() == INT;
}

bool Json::isNull()const
{
	return getType() == NUL;
}

bool Json::isObject()const
{
	return getType() == OBJECT;
}

bool Json::isString()const
{
	return getType() == STRING;
}

//getter
const Json::array& Json::getArray()const
{
	assert(isArray());
	return mptr->getArrayUnsafe();
}

const Json::object& Json::getObject()const
{
	assert(isObject());
	return mptr->getObjectUnsafe();
}

int Json::getInt()const
{
	assert(isInt());
	return mptr->getIntUnsafe();
}

double Json::getDouble()const
{
	assert(isDouble());
	return mptr->getDouleUnsafe();
}

const std::string& Json::getString()const
{
	assert(isString());
	return mptr->getStringUnsafe();
}

bool Json::getBool()const
{
	assert(isBool());
	return mptr->getBoolUnsafe();
}

//Json ctr
Json::Json(int i):mptr(std::make_shared<Value>(JsonInt(i))){}
Json::Json(bool b):mptr(std::make_shared<Value>(JsonBool(b))){}
Json::Json(double d):mptr(std::make_shared<Value>(JsonDouble(d))){}
Json::Json(const std::string& s):mptr(std::make_shared<Value>(JsonString(s))) {}
Json::Json(std::string&& s):mptr(std::make_shared<Value>(JsonString(std::move(s)))){}
Json::Json(const Json::array& a):mptr(std::make_shared<Value>(JsonArray(a))){}
Json::Json(Json::array&& a):mptr(std::make_shared<Value>(JsonArray(std::move(a)))){}
Json::Json(const Json::object& o):mptr(std::make_shared<Value>(JsonObject(o))){}
Json::Json(Json::object&& o):mptr(std::make_shared<Value>(JsonObject(std::move(o)))){}