//
// JSONTest.cpp
//
// $Id: //poco/Main/Util/testsuite/src/JSONTest.cpp#6 $
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "JSONTest.h"
#include "CppUnit/TestCaller.h"
#include "CppUnit/TestSuite.h"
#include "Poco/Web/JSONEntity.h"
#include "Poco/Web/JSONParser.h"
#include "Poco/Web/JSONPrinter.h"
#include "Poco/Web/JSONCondenser.h"
#include "Poco/Web/ExtJS/DirectHandler.h"
#include "Poco/Web/ExtJS/DirectAction.h"
#include "Poco/Web/ExtJS/DirectResponse.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/SharedPtr.h"
#include <sstream>


using Poco::Web::JSONEntity;
using Poco::Web::JSONParser;
using Poco::Web::JSONPrinter;
using Poco::Web::JSONCondenser;
using Poco::Web::ExtJS::DirectHandler;
using Poco::Web::ExtJS::DirectAction;
using Poco::Web::ExtJS::DirectResponse;
using Poco::Dynamic::Var;
using Poco::SharedPtr;


class TestAction: public DirectAction
{
public:
	TestAction(DirectResponse::Ptr pResponse): DirectAction(pResponse)
	{
	}

	void invoke(const std::string& method, const DirectHandler::ArrayType* pArgs)
	{
		std::ostringstream os;
		os << method << '(';

		DirectHandler::ArrayType::const_iterator it = pArgs->begin();
		DirectHandler::ArrayType::const_iterator end = pArgs->end();
		for (; it != end;)
		{
			if (it->isEmpty())
				os << "null";
			else
				os << it->convert<std::string>();

			if (++it != end) os << ',';
			else os << ')';
		}

		response().write(os.str());
	}
};


class TestArrayAction: public DirectAction
{
public:
	TestArrayAction(DirectResponse::Ptr pResponse): DirectAction(pResponse)
	{
	}

	void invoke(const std::string& method, const DirectHandler::ArrayType* pArgs)
	{
		response().writeArray(*pArgs);
	}
};


JSONTest::JSONTest(const std::string& name): CppUnit::TestCase(name)
{
}


JSONTest::~JSONTest()
{
}


void JSONTest::testEncoding()
{
	// TODO: Unicode
	std::vector<char> vc;
	vc.push_back('"');
	vc.push_back('\b');
	vc.push_back('\f');
	vc.push_back('\n');
	vc.push_back('\r');
	vc.push_back('\t');
	vc.push_back('/');
	vc.push_back('\\');

	JSONEntity::String str;
	std::vector<char>::iterator it = vc.begin();
	std::vector<char>::iterator end = vc.end();
	for (; it != end; ++it) str += *it;

	JSONEntity entity(JSONEntity::JSON_T_STRING, str);
	JSONEntity::String encStr = entity.toString();
	assert (encStr == "\\\"\\b\\f\\n\\r\\t\\/\\");
}


void JSONTest::testPrinter()
{
	const std::string str("{"
		"\"firstName\": \"John\","
		"\"lastName\": \"Smith\","
		"\"address\": {"
			"\"streetAddress\": \"21 2nd Street\","
			"\"city\": \"New York\","
			"\"state\": \"NY\","
			"\"postalCode\": 10021"
		"},"
	"\"phoneNumbers\": ["
		"\"212 555-1234\","
		"\"646 555-4567\""
	"],"
	"\"weight\": {"
		"\"value\": 123.456, \"units\": \"lbs\""
		"}"
	"}");

	std::ostringstream os;
	JSONParser jp(new JSONPrinter(os));
	jp.parse(str);

	std::string s1 = os.str();

	std::ostringstream ros;
	ros << "{" << std::endl;
	ros << "key = 'firstName', value = string: 'John'" << std::endl;
	ros << "key = 'lastName', value = string: 'Smith'" << std::endl;
	ros << "key = 'address', value = {" << std::endl;
	ros << "key = 'streetAddress', value = string: '21 2nd Street'" << std::endl;
	ros << "key = 'city', value = string: 'New York'" << std::endl;
	ros << "key = 'state', value = string: 'NY'" << std::endl;
	ros << "key = 'postalCode', value = integer: 10021" << std::endl;
	ros << "\t}" << std::endl;
	ros << "key = 'phoneNumbers', value = [" << std::endl;
	ros << "\t\tstring: '212 555-1234'" << std::endl;
	ros << "\t\tstring: '646 555-4567'" << std::endl;
	ros << "\t]" << std::endl;
	ros << "key = 'weight', value = {" << std::endl;
	ros << "key = 'value', value = float: 123.456" << std::endl;
	ros << "key = 'units', value = string: 'lbs'" << std::endl;
	ros << "\t}" << std::endl;
	ros << "}" << std::endl;

	assert (ros.str() == os.str());
}


void JSONTest::testCondenser()
{
	const std::string str("{"
		"\"firstName\": \"John\","
		"\"lastName\":  \"Smith\","
		"\"address\": {"
			"\"streetAddress\": \"21 2nd Street\","
			"\"city\":          \"New York\","
			"\"state\":         \"NY\","
			"\"postalCode\":     10021"
		"},"
	"\"phoneNumbers\": ["
		"\"212 555-1234\","
		"\"646 555-4567\""
	"],"
	"\"weight\": {"
		"\"value\": 123.456, \"units\": \"lbs\""
		"}"
	"}");

	std::ostringstream os;
	JSONParser jp(new JSONCondenser(os));
	jp.parse(str);

	std::string s1 = os.str();

	std::ostringstream ros;
	ros << "{\"firstName\":\"John\",\"lastName\":\"Smith\","
		"\"address\":{"
			"\"streetAddress\":\"21 2nd Street\","
			"\"city\":\"New York\","
			"\"state\":\"NY\","
			"\"postalCode\":10021},"
	"\"phoneNumbers\":[\"212 555-1234\",\"646 555-4567\"],"
	"\"weight\":{\"value\":123.456,\"units\":\"lbs\"}}";

	assert (ros.str() == os.str());
}


void JSONTest::testExtJSDirectHandler()
{
	std::string str = "{\"action\":\"DataList\",\"method\":\"getAll\",\"data\":[\"abc\",456,1.5,null,true,false],\"type\":\"rpc\",\"tid\":123}";
	std::ostringstream os;
	SharedPtr<DirectResponse> pTR = new DirectResponse(os);
	SharedPtr<TestArrayAction> pTA = new TestArrayAction(pTR);
	SharedPtr<DirectHandler> pDH = new DirectHandler(pTA);
	JSONParser jp(pDH);
	jp.parse(str);

	assert (pDH->action() == "DataList");
	assert (pDH->method() == "getAll");
	assert (pDH->type() == DirectHandler::DIRECT_TYPE_RPC);
	assert (pDH->tid() == 123);
	assert (pDH->get(0) == "abc");
	assert (pDH->get(1) == 456);
	assert (pDH->get(2) == 1.5);
	assert (pDH->get(3).isEmpty());
	assert (pDH->get(4) == true);
	assert (pDH->get(5) == false);
	
	assert (pTR->getAction() == "DataList");
	assert (pTR->getMethod() == "getAll");
	assert (pTR->getType() == "rpc");
	assert (pTR->getTID() == 123);

	assert (os.str() == "{\"type\":\"rpc\","
		"\"tid\":123,"
		"\"action\":\"DataList\","
		"\"method\":\"getAll\","
		"\"result\":[\"abc\",456,1.5,null,true,false]}");

	os.str("");
	SharedPtr<DirectResponse> pTR2 = new DirectResponse(os);
	SharedPtr<TestAction> pTA2 = new TestAction(pTR2);
	SharedPtr<DirectHandler> pDH2 = new DirectHandler(pTA2);
	JSONParser jp2(pDH2);
	jp2.parse(str);

	assert (os.str() == "{\"type\":\"rpc\","
		"\"tid\":123,"
		"\"action\":\"DataList\","
		"\"method\":\"getAll\","
		"\"result\":\"getAll(abc,456,1.5,null,true,false)\"}");
}


void JSONTest::setUp()
{
}


void JSONTest::tearDown()
{
}


CppUnit::Test* JSONTest::suite()
{
	CppUnit::TestSuite* pSuite = new CppUnit::TestSuite("JSONTest");

	CppUnit_addTest(pSuite, JSONTest, testEncoding);
	CppUnit_addTest(pSuite, JSONTest, testPrinter);
	CppUnit_addTest(pSuite, JSONTest, testCondenser);
	CppUnit_addTest(pSuite, JSONTest, testExtJSDirectHandler);

	return pSuite;
}
