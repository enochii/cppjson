#pragma once
#include <memory>
#include <vector>
#include <map>

#define TEST

#ifdef TEST
#include <iostream>
#endif // TEST


enum ValueType {
	NUL, BOOLEAN, NUMBER, STRING, ARRAY, OBJECT,
};

class Value;

class Json {
	//friend class JsonParser;
	friend class Value;

	std::shared_ptr<Value> mptr;
	static Json null;

public:
	using array = std::vector<Json>;
	using object = std::map < std::string, Json>;

	//ctr
	Json(Json&&) = default;
	//先埋一波炸弹
	Json(const Json&) = default;
	Json& operator=(Json&&) = default;
	Json& operator=(const Json&) = default;
	Json() = default;
	Json(void*) = delete;

	//可以隐式转换一波
	Json(int64_t i);
	Json(double d);
	Json(bool b);
	Json(std::string&& str);
	Json(const std::string& str);
	Json(array&& arr);
	Json(const array& arr);
	Json(object&& obj);
	Json(const object& obj);

	//geter
	int64_t getInt()const;
	double getDouble()const;
	bool getBool()const;
	const array& getArray()const;
	const object& getObject()const;
	const std::string& getString()const;

	Json& operator[](size_t i);
	const Json& operator[](size_t i)const;
	Json& operator[](const std::string& key);
	const Json& operator[](const std::string& key)const;

	int getType()const;

	bool isNull()const;
	bool isBool()const;
	bool isNumber()const;
	bool isArray()const;
	bool isObject()const;
	bool isString()const;
};

