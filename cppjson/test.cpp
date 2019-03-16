#include "JsonParser.h"
#include <iostream>

JsonParser p;
Json json;

void testLiteral()
{
	json = p.parse("null");
	assert(json.isNull());
	json = p.parse("false");
	assert(json.isBool() && json.getBool() == false);
	json = p.parse("true");
	assert(json.isBool() && json.getBool() == true);
}

void testExpectValue()
{
	//emmm
}

void testNumber()
{
	json = p.parse("1.222222");
	assert(json.isNumber() && json.getDouble() == 1.222222);
}

void testString()
{
	json = p.parse("\"sch\\nsb\"");
	assert(json.isString()&&json.getString() == "sch\nsb");
}

void testArray()
{
	json = p.parse("[]");
	assert(json.isArray()&&json.getArray().empty());
	json = p.parse("[1,   2]");
	assert(json.isArray() && json.getArray().size() == 2);
	json = p.parse("[  1,2  ]");
	assert(json.isArray() && json.getArray().size() == 2);
	json = p.parse(" [11111111,\"schsb\",1.22,[1,2,3]]");
	assert(json.isArray() && json.getArray().size() == 4);
	assert(json[0].getInt() == (int64_t)11111111);
	assert(json[1].getString() == "schsb");
	assert(json[2].getDouble() == 1.22);
	assert(json[3] .isArray()&&json[3].getArray().size() == 3);
	assert(json[3].getArray()[0].getInt() == 1);
}

void testObject()
{
	json = p.parse("{\"k1\":1}");
	assert(!p.failed());
	assert(json.isObject() && json.getObject().size() == 1);
	assert(json["k1"].getInt() == 1);
	json = p.parse("{\"k1\" : [false], \"k2\" : {\"k3\":null}}");
	assert(!p.failed());
	auto& o = json.getObject();
	assert(o.size() == 2);
	auto o1 = o.at("k1");
	assert(o1.isArray() && o1[0].getBool() == false);
	auto o2 = o.at("k2");
	assert(o2.isObject());
	assert(o2["k3"].isNull());
}

int main()
{	
	testLiteral();
	//testExpectValue();
	testString();
	testNumber();
	testArray();
	testObject();

	system("pause");
	return 0;
}