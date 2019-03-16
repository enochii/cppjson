#pragma once

#include <string>
#include <cassert>

#include "Json.h"

#define TEST

#ifdef TEST
#include <iostream>
#endif

enum PARSE_STATE{
	JSON_PARSE_OK = 0,
	JSON_PARSE_EXPECT_VALUE,
	JSON_PARSE_INVALID_VALUE,
	JSON_PARSE_NOT_SINGULAR,
	JSON_PARSE_NUMBER_TOO_BIG,
	JSON_PARSE_MISS_QUOTATION,
	JSON_PARSE_INVALID_STRING_ESCAPE,
	JSON_PARSE_INVALID_STRING_CHAR,
	JSON_PARSE_INVALID_UNICODE_HEX,
	JSON_PARSE_INVALID_UNICODE_SURROGATE,
	JSON_PARSE_MISS_COMMA_OR_SQUARE_BRACKET,
	//JSON_PARSE_ARRAY_EXTRA_COMMA,//extra comma in the end, like [1,2,]
	JSON_PARSE_MISS_COLON,
	JSON_PARSE_MISS_KEY,
	JSON_PARSE_MISS_COMMA_OR_CURLY_BRACKET,
};

class JsonParser {
private:
	std::string context;
	//size_t currentPos = 0;
	const char* p;//current pointer
	PARSE_STATE state = JSON_PARSE_OK;

	void parseWhitespace();
	void expect(char logo);
	
	//null, false, true
	Json parseLiteral(const std::string& str);
	//parse proxy
	Json parseNumber();
	std::string parseString();
	Json parseArray();
	Json parseObject();
	//

public:
	//关于接口有两种设计：
	//1. parse函数自带初始化parser就是那几个private mem，这样就没有parseMulti
	//2. 初始化parser在constructor做，这样可以parse多次
	JsonParser() = default;
	//初始化列表的顺序是按照成员的声明顺序哦亲
	JsonParser(const std::string& str):context(str),p(context.c_str()){}
	JsonParser(std::string&& str):context(std::move(str)), p(context.c_str()) {}

	Json parse(const std::string& str)
	{
		context = str;
		p = context.c_str();
		state = JSON_PARSE_OK;
		//
		return parse();
	}
	//这个可以 被parseMulti调用
	Json parse()
	{
		assert(p != NULL);
		//std::cout << "complete me? im parse()\n";
		return parseValue();
	}

	bool failed()const { return state != JSON_PARSE_OK; }

private:
	Json parseValue();
	unsigned parseHex4();
	void encodeUtf8(unsigned u, std::string& s);
};