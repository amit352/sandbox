//
// HttpServletRequest.h
//
//
// Library: Servlet
// Package: Http
// Module:  HttpServletRequest
//
// Definition of the HttpServletRequest class.
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
//
// The code in this file is derived from Apache Tomcat project
// Copyright (c) 2004 The Apache Software Foundation.
// http://www.apache.org/licenses/LICENSE-2.0
// 


#ifndef Servlet_Http_HttpServletRequest_INCLUDED
#define Servlet_Http_HttpServletRequest_INCLUDED


#include "Poco/Servlet/ServletRequest.h"
#include "Poco/Servlet/Cookie.h"
#include "Poco/Servlet/HttpSession.h"
#include "Poco/Servlet/HttpServletResponse.h"
#include "Poco/Servlet/ServletBase.h"
#include <string>
#include <vector>
#include <map>


namespace Poco {
namespace Servlet {


class Servlet_API HttpServletRequest : virtual public ServletRequest 
{
public:
	HttpServletRequest();
		/// Creates HttpServletRequest.

	virtual ~HttpServletRequest();
		/// Destroys HttpServletRequest.

	static const std::string BASIC_AUTH;
		/// String identifier for Basic authentication. Value "BASIC"

	static const std::string FORM_AUTH;
		///String identifier for Form authentication. Value "FORM"

	static const std::string CLIENT_CERT_AUTH;
		///String identifier for Client Certificate authentication. Value "CLIENT_CERT"

	static const std::string DIGEST_AUTH;
		///String identifier for Digest authentication. Value "DIGEST"

	virtual std::string getAuthType() = 0;
		/// Returns the name of the authentication scheme used to protect
		/// the servlet. All servlet containers support basic, form and client 
		/// certificate authentication, and may additionally support digest 
		/// authentication.
		/// If the servlet is not authenticated null is returned. 
		/// Same as the value of the CGI variable AUTH_TYPE.
		/// returns		one of the static members BASIC_AUTH, 
		/// FORM_AUTH, CLIENT_CERT_AUTH, DIGEST_AUTH
		/// (suitable for == comparison) or the container-specific 
		/// string indicating the authentication scheme, or null if the request 
		/// was not authenticated. 

	virtual const std::vector<Cookie>& getCookies() const = 0;
		/// Returns an array containing all of the Cookie
		/// objects the client sent with this request.
		/// This method returns null if no cookies were sent.

	virtual long getDateHeader(const std::string& name) = 0;
		/// Returns the value of the specified request header
		/// as a long value that represents a 
		/// Date object. Use this method with
		/// headers that contain dates, such as
		/// If-Modified-Since. 
		/// The date is returned as
		/// the number of milliseconds since January 1, 1970 GMT.
		/// The header name is case insensitive.
		/// If the request did not have a header of the
		/// specified name, this method returns -1. If the header
		/// can't be converted to a date, the method throws
		/// an IllegalArgumentException.
		/// Returns a long value representing the date specified
		/// in the header expressed as the number of milliseconds
		/// since January 1, 1970 GMT, or -1 if the named header
		///	was not included with the request
		///
		/// Throws	InvalidArgumentException	If the header value
		/// can't be converted to a date.
		///

	virtual std::string getHeader(const std::string& name) = 0; 
		/// Returns the value of the specified request header
		/// as a String. If the request did not include a header
		/// of the specified name, this method returns empty string.
		/// If there are multiple headers with the same name, this method
		/// returns the first head in the request.
		/// The header name is case insensitive. You can use
		/// this method with any request header.

	virtual std::vector<std::string> getHeaders(const std::string& name) = 0; 
		/// Returns all the values of the specified request header
		/// as an std::vector<std::string> of std::tring objects.
		/// Some headers, such as Accept-Language can be sent
		/// by clients as several headers each with a different value rather than
		/// sending the header as a comma separated list.
		/// If the request did not include any headers
		/// of the specified name, this method returns an empty
		/// std::vector<std::string>.
		/// The header name is case insensitive. You can use
		/// this method with any request header.

	virtual std::vector<std::string> getHeaderNames() = 0;
		/// Returns an enumeration of all the header names
		/// this request contains. If the request has no
		/// headers, this method returns an empty enumeration.
		/// Some servlet containers do not allow
		/// servlets to access headers using this method, in
		/// which case this method returns null.

	virtual int getIntHeader(const std::string& name) = 0;
		/// Returns the value of the specified request header
		/// as an int. If the request does not have a header
		/// of the specified name, this method returns -1. If the
		/// header cannot be converted to an integer, this method
		/// throws a NumberFormatException.
		/// The header name is case insensitive.

	virtual std::string getMethod() = 0;
		/// Returns the name of the HTTP method with which this 
		/// request was made, for example, GET, POST, or PUT.
		/// Same as the value of the CGI variable REQUEST_METHOD.

	virtual std::string getPathInfo() = 0;
		/// Returns any extra path information associated with
		/// the URL the client sent when it made this request.
		/// The extra path information starts with a "/" character.
		/// This method returns null if there was no extra path information.
		/// Same as the value of the CGI variable PATH_INFO.
	 
	virtual std::string getPathTranslated() const = 0;
		/// Returns any extra path information before the query string,
		/// and translates it to a real path.
		/// Same as the value of the CGI variable PATH_TRANSLATED.
		/// If the URL does not have any extra path information,
		/// or the servlet container cannot translate the virtual path 
		/// to a real path for any reason (such as when the web application 
		/// is executed from an archive), this method returns null.
		/// The web container does not decode this string.

	virtual std::string getContextPath() const = 0;
		/// Returns the portion of the request URI that indicates the context
		/// of the request.  The context path always comes first in a request
		/// URI.  The path starts with a "/" character but does not end with a "/"
		/// character.  For servlets in the default (root) context, this method
		/// returns "". The container does not decode this string.

	virtual std::string getQueryString() = 0;
		/// Returns the query string that is contained in the request
		/// URL after the path. This method returns null
		/// if the URL does not have a query string. Same as the value
		/// of the CGI variable QUERY_STRING.     

	virtual std::string getRemoteUser() = 0;
		/// Returns the login of the user making this request, if the
		/// user has been authenticated, or empty string if the user 
		/// has not been authenticated.
		/// Whether the user name is sent with each subsequent request
		/// depends on the browser and type of authentication. Same as the 
		/// value of the CGI variable REMOTE_USER.

	virtual bool isUserInRole(const std::string& role) = 0;
		/// Returns a bool indicating whether the authenticated user is included
		/// in the specified logical "role".  Roles and role membership can be
		/// defined using deployment descriptors.  If the user has not been
		/// authenticated, the method returns false.

	//TODO virtual java.security.Principal getUserPrincipal() = 0;
		///Returns a java.security.Principal object containing
		///the name of the current authenticated user. If the user has not been
		///authenticated, the method returns null.

	virtual std::string getRequestedSessionId() const = 0;
		/// Returns the session ID specified by the client. This may
		/// not be the same as the ID of the current valid session
		/// for this request.
		/// If the client did not specify a session ID, this method returns
		/// "".

	virtual std::string getRequestURI() = 0;
		/// Returns this request's URI.

	virtual std::string getRequestURL() = 0;
		/// Calls getRequestURI().

	virtual std::string getServletPath() = 0;
		/// Returns the part of this request's URL that calls
		/// the servlet. This path starts with a "/" character
		/// and includes either the servlet name or a path to
		/// the servlet, but does not include any extra path
		/// information or a query string. Same as the value of
		/// the CGI variable SCRIPT_NAME.
		/// This method will return an empty string ("") if the
		/// servlet used to process this request was matched using
		/// the "/*" pattern.

	virtual const HttpSession* getSession(bool create) = 0;
		/// Returns the current HttpSession
		/// associated with this request or, if there is no
		/// current session and create is true, returns 
		/// a new session.
		/// If create is false and the request has no valid HttpSession,
		/// this method returns null.
		/// To make sure the session is properly maintained,
		/// you must call this method before the response is committed.

	virtual const HttpSession* getSession() = 0;
		/// Returns the current session associated with this request,
		/// or if the request does not have a session, creates one.

	virtual bool isRequestedSessionIdValid() const = 0;
		/// Checks whether the requested session ID is still valid.

	virtual bool isRequestedSessionIdFromCookie() const = 0;
		/// Checks whether the requested session ID came in as a cookie.

	virtual bool isRequestedSessionIdFromURL() const = 0;
		/// Checks whether the requested session ID came in as part of the 
		/// request URL.

	virtual bool isRequestedSessionIdFromUrl() const = 0;
		/// Deprecated.	As of Version 2.1 of the Java Servlet
		/// API, use isRequestedSessionIdFromURL instead.
};


} } // namespace Poco::Servlet


#endif // Servlet_Http_HttpServletRequest_INCLUDED