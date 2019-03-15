#include "JsonParser.h"
#include "Json.h"
#include <iostream>

inline void JsonParser::expect(char logo)
{
	assert(logo == context[currentPos]);
	currentPos++;
}

PARSE_STATE JsonParser::parseLiteral(const std::string& str)
{
	expect(str[0]);
	if(context.size() - currentPos<str.size() - 1)return PARSE_STATE::JSON_PARSE_EXPECT_VALUE;
	for (size_t i = 1;i < str.size();i++, currentPos++)
		if (str[i] != context[currentPos]) {
			return PARSE_STATE::JSON_PARSE_EXPECT_VALUE;
		}
	return PARSE_STATE::JSON_PARSE_OK;
}

inline void JsonParser::parseWhitespace()
{
	char proxy;
	while (currentPos < context.size() &&
		((proxy = context[currentPos])=='\n'||
			proxy == '\t'||proxy == '\r'||proxy == ' '
			)
		)currentPos++;
}

