#pragma once
#include <memory>
#include <vector>
#include <map>

#define TEST

#ifdef TEST
#include <iostream>
#endif // TEST


enum ValueType {
	NUL, BOOLEAN, INT, DOUBLE, STRING, ARRAY, OBJECT,
};

class Value;

class Json {
	std::shared_ptr<Value> mptr;
	static Json null;

public:
	using array = std::vector<Json>;
	using object = std::map < std::string, Json>;

	//ctr
	Json(Json&&) = default;
	//先埋一波炸弹
	Json(const Json&) {
		std::cout << __LINE__ << '\n';
	}
	Json() = default;
	Json(void*) = delete;

	//可以隐式转换一波
	Json(int i);
	Json(double d);
	Json(bool b);
	Json(std::string&& str);
	Json(const std::string& str);
	Json(array&& arr);
	Json(const array& arr);
	Json(object&& obj);
	Json(const object& obj);

	//geter
	int getInt()const;
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
	bool isInt()const;
	bool isDouble()const;
	bool isArray()const;
	bool isObject()const;
	bool isString()const;
};

