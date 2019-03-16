#include "JsonParser.h"
#include "Json.h"
//#include <iostream>
#include <cctype>
#include <cerrno>//errno, ERANGE

#define ERROR(err) do{ state = err; return Json();}while(0)

inline void JsonParser::expect(char logo)
{
	assert(logo == *p);
	++p;
}

Json JsonParser::parseLiteral(const std::string& str)
{
	expect(str[0]);
	size_t i = 1;//str[0]已经被吞掉了
	for (;i < str.size() && *p != '\0';++p, ++i)
		if (str[i] != *p) break;
	//处理了 nul / noll的情况
	if(i!=str.size())ERROR(JSON_PARSE_EXPECT_VALUE);
	return str[0] == 'n' ? Json() : (str[0] == 'f' ? false : true);
}

inline void JsonParser::parseWhitespace()
{
	char proxy;
	while ((proxy = *p)!='\0' &&
		(proxy =='\n'|| proxy == '\t'||proxy == '\r'||proxy == ' ')
		)++p;
}

Json JsonParser::parseValue()
{
	//std::cout << "complete me? im parseValue()\n";

	//int oriPos = currentPos;
	parseWhitespace();

	char ch = *p;
	switch (ch) {
	case 'n':
		return parseLiteral("null");
	case 'f':
		return parseLiteral("false");
	case 't':
		return parseLiteral("true");
	case '"':
		return parseString();
	case '[':
		return parseArray();
	case '{':
		return parseObject();

	default:
		if (ch == '-' || isdigit(ch))return parseNumber();
		else {
			state = JSON_PARSE_INVALID_VALUE;
			return Json();
		}
	}
}

Json JsonParser::parseNumber()
{
	const char* c = p;
	char* e;
	if (*c == '-')++c;
	//0开头的话只能是单个0
	if (*c == '0')++c;
	else if(isdigit(*c)){
		do { ++c; } while (isdigit(*c));
	}
	else {
		ERROR(JSON_PARSE_INVALID_VALUE);
	}
	if (*c != '.' && *c != 'e' && *c != 'E') {
		errno = 0;
		int64_t integer = std::strtoll(p, &e, 10);
		if (errno == ERANGE)ERROR(JSON_PARSE_NUMBER_TOO_BIG);
		
		p = e;
		return integer;
	}
	//double
	if (*c == '.') {
		++c;
		if (!isdigit(*c))ERROR(JSON_PARSE_EXPECT_VALUE);
		do { ++c; } while (isdigit(*c));
	}
	if (*c == 'e' || *c == 'E') {
		++c;
		if (*c == '-' || *c == '+')++c;
		if (!isdigit(*c))ERROR(JSON_PARSE_EXPECT_VALUE);
		for (++c;isdigit(*c);++c);
	}
	errno = 0;
	double fraction = std::strtod(p, &e);
#ifdef TEST
	//std::cout << p << '\n' << fraction << '\n';
#endif // TEST

	if (errno == ERANGE)ERROR(JSON_PARSE_NUMBER_TOO_BIG);
	p = e;
	return fraction;
}

unsigned JsonParser::parseHex4()
{
	unsigned u = 0;
	for (int i = 0;i < 4;++i) {
		char ch = *p++;
		u <<= 4;
		if (ch >= '0'&&ch <= '9')u |= ch - '0';
		else if (ch >= 'a'&&ch <= 'f')u |= ch - ('a' - 10);
		else if (ch >= 'A'&&ch <= 'F')u |= ch - ('A' - 10);
		else {
			state = JSON_PARSE_INVALID_UNICODE_HEX;
			break;
		}
	}
	return u;
}

void JsonParser::encodeUtf8(unsigned u, std::string& s)
{
	if (u <= 0x7F)
		s.push_back(static_cast<char>(u & 0xFF));
	else if (u <= 0x7FF) {
		s.push_back(static_cast<char>(0xC0 | (u >> 6) & 0xFF));
		s.push_back(0x80 | (u & 0x3F));
	}
	else if (u <= 0xFFFF) {
		s.push_back(0xE0 | ((u >> 12) & 0xFF));
		s.push_back(0x80 | ((u >> 6) & 0x3F));
		s.push_back(0x80 | (u & 0x3F));
	}
	else {
		assert(u <= 0x10FFFF);
		s.push_back(0xF0 | ((u >> 18) & 0xFF));
		s.push_back(0x80 | ((u >> 12) & 0x3F));
		s.push_back(0x80 | ((u >> 6) & 0x3F));
		s.push_back(0x80 | (u & 0x3F));
	}
}

#define STRINGERROR 0

std::string JsonParser::parseString()
{
	expect('\"');
	
	std::string s;
	s.reserve(16);
	for (;;) {
		char ch = *p++;
		switch (ch) {
		case '\"':
			return std::move(s);
		case '\0':
			state = JSON_PARSE_MISS_QUOTATION;
			break;
		case '\\':
		{
			switch (*p++) {
			case 'n':s.push_back('\n');break;
			case 'r':s.push_back('\r');break;
			case 'b':s.push_back('\b');break;
			case 'f':s.push_back('\f');break;
			case 't':s.push_back('\t');break;
			case '\\':s.push_back('\\');break;
			case '/':s.push_back('/');break;
			case '\"':s.push_back('\"');break;
			case 'u':
			{
				unsigned u = parseHex4();
				if (failed())break;
				if (u >= 0xD800 && u <= 0xDBFF) {
					if (*p++ != '\\' || *p++ != 'u') {
						state = JSON_PARSE_INVALID_UNICODE_SURROGATE;
						break;
					}
					unsigned u2 = parseHex4();
					if (failed())break;
					if (u2 < 0xDC00 || u2>0xDFFF) {
						state = JSON_PARSE_INVALID_UNICODE_HEX;
						break;
					}
					u = (((u - 0xD800) << 10) | (u2 - 0xDC00)) + 0x10000;
				}
				encodeUtf8(u, s);
				break;
			}
			default:
				state = JSON_PARSE_INVALID_STRING_ESCAPE;
				break;
			}
			break;
		}
		default:
			if ((unsigned char)ch < 0x20) {
				state = JSON_PARSE_INVALID_STRING_CHAR;
				break;
			}
			s.push_back(ch);
		}
		if (failed())return "";
	}
}
Json JsonParser::parseArray()
{
	expect('[');
	Json::array arr;
	//
	parseWhitespace();
	if (*p == ']') {
		++p;
		return std::move(arr);
	}
	//
	for (;;) {
		arr.emplace_back(parseValue());
		if (failed())return Json();
		//continue
		parseWhitespace();
		if (*p == ',') {
			++p;
			continue;
		}
		else if (*p == ']') {
			++p;
			return std::move(arr);
		}
		else {
			ERROR(JSON_PARSE_MISS_COMMA_OR_SQUARE_BRACKET);
		}
	}
}
Json JsonParser::parseObject()
{
	expect('{');
	Json::object obj;

	parseWhitespace();
	if (*p == '}') {
		++p;
		return std::move(obj);
	}
	for (;;) {
		auto key = parseString();
		if (failed())return Json();
		parseWhitespace();
		if (*p++ != ':')ERROR(JSON_PARSE_MISS_COLON);
		auto value = parseValue();
		if (failed())return Json();
		parseWhitespace();

		obj.emplace(key, value);

		if (*p == ',')++p;//do nothing
		else if (*p == '}') {
			++p;return std::move(obj);
		}
		else ERROR(JSON_PARSE_MISS_COMMA_OR_CURLY_BRACKET);
		parseWhitespace();
	}

}