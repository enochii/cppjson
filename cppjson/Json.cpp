
#include <cassert>
#include <cstdlib>
//#include <string>

#include "Json.h"
//#define TEST

Json Json::null;

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
	virtual double getDoubleUnsafe();
	virtual int64_t getIntUnsafe();
	virtual Json::array& getArrayUnsafe();
	virtual Json::object& getObjectUnsafe();
	virtual std::string& getStringUnsafe();
	//*/
	virtual Json& getArrayItemUnsafe(size_t i);
	virtual const Json& getArrayItemUnsafe(size_t i)const;
	virtual Json& getObjectItemUnsafe(const std::string& key);
	virtual const Json& getObjectItemUnsafe(const std::string& key)const;


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
		//std::cout << value;
		return value;
	}
	explicit JsonBool(bool b):JsonValue(BOOLEAN, b){}
};

class JsonInt :public JsonValue<int64_t> {
public:
	int64_t getIntUnsafe()override
	{
		return value;
	}
	explicit JsonInt(int64_t i):JsonValue(NUMBER, i){}
};

class JsonDouble :public JsonValue<double> {
public:
	double getDoubleUnsafe()override
	{
		return value;
	}
	explicit JsonDouble(double d):JsonValue(NUMBER, d){}
};

class JsonArray:public JsonValue<Json::array>{
public:
	Json::array& getArrayUnsafe()override
	{
		return value;
	}
	explicit JsonArray(const Json::array& arr):JsonValue(ARRAY,arr){}
	explicit JsonArray(Json::array&& arr) :JsonValue(ARRAY, arr) {}

	Json& getArrayItemUnsafe(size_t i)override
	{
		return const_cast<Json&>((static_cast<const JsonArray&>(*this)).getArrayItemUnsafe(i));
	}
	const Json& getArrayItemUnsafe(size_t i)const override
	{
		assert(value.size() > i);
		return value[i];
	}
};

class JsonObject :public JsonValue<Json::object> {
public:
	Json::object& getObjectUnsafe() override
	{
		return value;
	}
	explicit JsonObject(const Json::object& obj):JsonValue(OBJECT, obj){}
	explicit JsonObject(Json::object&& obj) :JsonValue(OBJECT, obj) {}

	Json& getObjectItemUnsafe(const std::string& key)override
	{
		return value[key];
	}
	const Json& getObjectItemUnsafe(const std::string& key)const override
	{
		return value.at(key);
	}
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
Json& Value::getArrayItemUnsafe(size_t i)
{
	return Json::null;
}
const Json& Value::getArrayItemUnsafe(size_t i)const
{
	return Json::null;

}
Json& Value::getObjectItemUnsafe(const std::string& key)
{
	return Json::null;
}
const Json& Value::getObjectItemUnsafe(const std::string& key)const
{
	return Json::null;
}

bool Value::getBoolUnsafe()
{
	return false;
}

int64_t Value::getIntUnsafe()
{
	return 0;
}

double Value::getDoubleUnsafe()
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

/*
//模板的非类型参数比如这里的tag推导不出来，总感觉肯定用词不当
template<typename T>
JsonValue<T> make_jsonValue(ValueType tag, T&& value)
{
	return JsonValue<T>(tag, std::move(value));
}
*/

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

bool Json::isNumber()const
{
	return getType() == NUMBER;
}

bool Json::isNull()const
{
	return mptr == nullptr;//
		//getType() == NUL;
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

int64_t Json::getInt()const
{
	assert(isNumber());
	return mptr->getIntUnsafe();
}

double Json::getDouble()const
{
	assert(isNumber());
	return mptr->getDoubleUnsafe();
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
Json::Json(int64_t i):mptr(std::make_shared<JsonInt>(JsonInt(i))){}
Json::Json(bool b):mptr(std::make_shared<JsonBool>(JsonBool(b))){}
Json::Json(double d):mptr(std::make_shared<JsonDouble>(d)){
	//std::cout << "JsonDouble make_shared!\n";
}
Json::Json(const std::string& s):mptr(std::make_shared<JsonString>(s)) {}
Json::Json(std::string&& s):mptr(std::make_shared<JsonString>(std::move(s))){}
Json::Json(const Json::array& a):mptr(std::make_shared<JsonArray>(a)){}
Json::Json(Json::array&& a):mptr(std::make_shared<JsonArray>(std::move(a))){}
Json::Json(const Json::object& o):mptr(std::make_shared<JsonObject>(o)){}
Json::Json(Json::object&& o):mptr(std::make_shared<JsonObject>(std::move(o))){}

Json& Json::operator[](size_t i)
{
	assert(isArray());
	return mptr->getArrayItemUnsafe(i);
}
const Json& Json::operator[](size_t i)const
{
	assert(isArray());
	return mptr->getArrayItemUnsafe(i);
}
Json& Json::operator[](const std::string& key)
{
	assert(isObject());
	return mptr->getObjectItemUnsafe(key);
}
const Json& Json::operator[](const std::string& key)const
{
	assert(isObject());
	return mptr->getObjectItemUnsafe(key);
}