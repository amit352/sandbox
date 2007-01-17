//
// TestServlet.h
//
//
// Library: TestServlet
// Package: TestServlet
// Module:  TestServlet
//
// Definition of the TestServlet class.
//
// Copyright (c) 2006, Aleksandar Fabijanic and Contributors.
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


#ifndef Servlet_Http_TestServlet_INCLUDED
#define Servlet_Http_TestServlet_INCLUDED


#include "Poco/Servlet/HttpServlet.h"
#include "Poco/Timestamp.h"


#if defined(_WIN32) && defined(POCO_SERVLET_DLL)
#	if defined(SERVLETTEST_EXPORTS)
#		define POCO_SERVLETTEST_DLL_API __declspec(dllexport)
#	else
#		define POCO_SERVLETTEST_DLL_API __declspec(dllimport)
#	endif
#else
#	define POCO_SERVLETTEST_DLL_API
#endif


namespace Poco {
namespace Servlet {


class POCO_SERVLETTEST_DLL_API TestServlet : public HttpServlet
{
 public: 
	 TestServlet();
	 ~TestServlet();

	void doGet(HttpServletRequest& req, HttpServletResponse& resp);
	void doPost(HttpServletRequest& req, HttpServletResponse& resp);

 private:
	int _counter;
	std::string _sessionId;
};


class POCO_SERVLETTEST_DLL_API SingletonTestServlet : public HttpServlet
{
 public: 
	SingletonTestServlet();
	~SingletonTestServlet();
	void doGet(HttpServletRequest& req, HttpServletResponse& resp);
	void doPost(HttpServletRequest& req, HttpServletResponse& resp);

 private:
	int _counter;
};


class POCO_SERVLETTEST_DLL_API DefaultTestServlet : public HttpServlet
{
 public: 
	DefaultTestServlet();
	~DefaultTestServlet();
	void doGet(HttpServletRequest& req, HttpServletResponse& resp);
	void doPost(HttpServletRequest& req, HttpServletResponse& resp);

 private:
	int _counter;
};


class POCO_SERVLETTEST_DLL_API FormTestServlet : public HttpServlet
{
 public: 
	FormTestServlet();
	~FormTestServlet();
	void doGet(HttpServletRequest& req, HttpServletResponse& resp);
	void doPost(HttpServletRequest& req, HttpServletResponse& resp);

 private:
	int _counter;
};


class POCO_SERVLETTEST_DLL_API JSPTestServlet : public HttpServlet
{
 public: 
	JSPTestServlet();
	~JSPTestServlet();
	void doGet(HttpServletRequest& req, HttpServletResponse& resp);
	void doPost(HttpServletRequest& req, HttpServletResponse& resp);

 private:
	int _counter;
};


class POCO_SERVLETTEST_DLL_API ForwardTestServlet : public HttpServlet
{
 public: 
	ForwardTestServlet();
	~ForwardTestServlet();
	void doGet(HttpServletRequest& req, HttpServletResponse& resp);
};


class POCO_SERVLETTEST_DLL_API ForwardedTestServlet : public HttpServlet
{
 public: 
	ForwardedTestServlet();
	~ForwardedTestServlet();
	void doGet(HttpServletRequest& req, HttpServletResponse& resp);
};


class POCO_SERVLETTEST_DLL_API IncludeTestServlet : public HttpServlet
{
 public: 
	IncludeTestServlet();
	~IncludeTestServlet();
	void doGet(HttpServletRequest& req, HttpServletResponse& resp);
};


class POCO_SERVLETTEST_DLL_API IncludedTestServlet : public HttpServlet
{
 public: 
	IncludedTestServlet();
	~IncludedTestServlet();
	void doGet(HttpServletRequest& req, HttpServletResponse& resp);
};


class POCO_SERVLETTEST_DLL_API RefreshTestServlet : public HttpServlet
{
 public: 
	RefreshTestServlet();
	~RefreshTestServlet();
	void doGet(HttpServletRequest& req, HttpServletResponse& resp);

private:
	Poco::Timestamp _started;
	Poco::UInt64    _hitCounter;
};


} } // namespace Poco::Servlet


#endif //Servlet_Http_TestServlet_INCLUDED
