//
// ServletResponse.h
//
//
// Library: Servlet
// Package: Servlet
// Module:  ServletResponse
//
// Definition of the ServletResponse class.
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


#ifndef Servlet_ServletResponse_INCLUDED
#define Servlet_ServletResponse_INCLUDED


#include "Poco/Servlet/ServletBase.h"
#include "Poco/Servlet/ServletException.h"
#include "Poco/Servlet/ServletOutputStream.h"
#include "Poco/Servlet/RequestDispatcher.h"
#include "Poco/Servlet/PrintWriter.h"
#include <string>
#include <vector>
#include <map>


namespace Poco {
namespace Servlet {


class Servlet_API ServletResponse 
	/// Defines an object to assist a servlet in sending a response to the client. The servlet
	/// container creates a ServletResponse object and passes it as an argument to the
	/// servlet�s service method.
	///
	/// To send binary data in a MIME body response, use the ServletOutputStream
	/// returned by getOutputStream() . To send character data, use the PrintWriter
	/// object returned by getWriter() . To mix binary and text data, for example, to
	/// create a multipart response, use a ServletOutputStream and manage the character
	/// sections manually.
	///
	/// The charset for the MIME body response can be specified explicitly using the
	/// setCharacterEncoding(String) and setContentType(String) methods, or
	/// implicitly using the setLocale(Locale) method. Explicit specifications take
	/// precedence over implicit specifications. If no charset is specified, ISO-8859-1
	/// will be used. The setCharacterEncoding, setContentType, or setLocale
	/// method must be called before getWriter and before committing the response for
	/// the character encoding to be used.
	///
	/// See the Internet RFCs such as RFC 2045 (http://www.ietf.org/rfc/rfc2045.txt) for
	/// more information on MIME. Protocols such as SMTP and HTTP define profiles
	/// of MIME, and those standards are still evolving.
{
public:

	ServletResponse();
		/// Creates response.

	virtual ~ServletResponse();
		/// Destroys response.

	virtual void flushBuffer() = 0;
		/// Forces any content in the buffer to be written to the client. A call to this
		/// method automatically commits the response, meaning the status code and
		/// headers will be written.

	virtual int getBufferSize() = 0;
		/// Returns the actual buffer size used for the response. If no buffering is used,
		/// this method returns 0.

	virtual const std::string getCharacterEncoding() const = 0;
		/// Returns the name of the character encoding (MIME charset) used for the
		/// body sent in this response. The character encoding may have been specified
		/// explicitly using the setCharacterEncoding(String) or
		/// setContentType(string) methods, or implicitly using the
		/// setLocale(locale) method. Explicit specifications take precedence over
		/// implicit specifications. Calls made to these methods after getWriter has
		/// been called or after the response has been committed have no effect on the
		/// character encoding. If no character encoding has been specified, ISO-8859-1
		/// is returned.
		/// See RFC 2047 (http://www.ietf.org/rfc/rfc2047.txt) for more information
		/// about character encoding and MIME.

	virtual std::string getContentType() = 0;
		/// Returns the content type used for the MIME body sent in this response. The
		/// content type proper must have been specified using
		/// setContentType(String) before the response is committed. If no content
		/// type has been specified, this method returns null. If a content type has been
		/// specified and a character encoding has been explicitly or implicitly specified
		/// as described in getCharacterEncoding() , the charset parameter is included
		/// in the string returned. If no character encoding has been specified, the charset
		/// parameter is omitted.

	virtual std::locale getLocale() = 0;
		/// Returns the locale specified for this response using the setLocale(Locale)
		/// method. Calls made to setLocale after the response is committed have no
		/// effect. If no locale has been specified, the container�s default locale is
		/// returned.

	virtual ServletOutputStream& getOutputStream() = 0;
		/// Returns a ServletOutputStream suitable for writing binary data in the
		/// response. The servlet container does not encode the binary data.
		/// Calling flush() on the ServletOutputStream commits the response. Either this
		/// method or getWriter() may be called to write the body, not both.

	virtual PrintWriter& getWriter() = 0;
		/// Returns a PrintWriter object that can send character text to the client. The
		/// PrintWriter uses the character encoding returned by
		/// getCharacterEncoding() . If the response�s character encoding has not
		/// been specified as described in getCharacterEncoding (i.e., the method just
		/// returns the default value ISO-8859-1), getWriter updates it to ISO-8859-1.
		/// Calling flush() on the PrintWriter commits the response.
		/// Either this method or getOutputStream() may be called to write the body,
		/// not both.
		
	virtual bool isCommitted() = 0;
		/// Returns a boolean indicating if the response has been committed. A committed
		/// response has already had its status code and headers written.

	virtual void reset() = 0;
		/// Clears any data that exists in the buffer as well as the status code and headers.
		/// If the response has been committed, this method throws an IllegalStateException.

	virtual void resetBuffer() = 0;
		/// Clears the content of the underlying buffer in the response without clearing
		/// headers or status code. If the response has been committed, this method
		/// throws an IllegalStateException.

	virtual void setBufferSize(int size) = 0;
		/// Sets the preferred buffer size for the body of the response. The servlet container
		/// will use a buffer at least as large as the size requested. The actual buffer
		/// size used can be found using getBufferSize.
		/// A larger buffer allows more content to be written before anything is actually
		/// sent, thus providing the servlet with more time to set appropriate status codes
		/// and headers. A smaller buffer decreases server memory load and allows the
		/// client to start receiving data more quickly.
		/// This method must be called before any response body content is written; if
		/// content has been written or the response object has been committed, this
		/// method throws an IllegalStateException.

	virtual void setCharacterEncoding(const std::string& charset) = 0;
		/// Sets the character encoding (MIME charset) of the response being sent to the
		/// client, for example, to UTF-8. If the character encoding has already been set
		/// by setContentType(String) or setLocale(Locale) , this method overrides
		/// it. Calling setContentType(String) with the String of text/html
		/// and calling this method with the String of UTF-8 is equivalent with calling
		/// setContentType with the String of text/html; charset=UTF-8.
		/// This method can be called repeatedly to change the character encoding. This
		/// method has no effect if it is called after getWriter has been called or after the
		/// response has been committed.
		/// Containers must communicate the character encoding used for the servlet
		/// response�s writer to the client if the protocol provides a way for doing so. In
		/// the case of HTTP, the character encoding is communicated as part of the
		/// Content-Type header for text media types. Note that the character encoding
		/// cannot be communicated via HTTP headers if the servlet does not specify a
		/// content type; however, it is still used to encode text written via the servlet
		/// response�s writer.

	virtual void setContentLength(int len) = 0;
		/// Sets the length of the content body in the response In HTTP servlets, this
		/// method sets the HTTP Content-Length header.

	virtual void setContentType(const std::string& type) = 0;
		/// Sets the content type of the response being sent to the client, if the response
		/// has not been committed yet. The given content type may include a character
		/// encoding specification, for example, text/html;charset=UTF-8. The
		/// response�s character encoding is only set from the given content type if this
		/// method is called before getWriter is called.
		/// This method may be called repeatedly to change content type and character
		/// encoding. This method has no effect if called after the response has been
		/// committed. It does not set the response�s character encoding if it is called
		/// after getWriter has been called or after the response has been committed.
		/// Containers must communicate the content type and the character encoding
		/// used for the servlet response�s writer to the client if the protocol provides a
		/// way for doing so. In the case of HTTP, the Content-Type header is used.

	virtual void setLocale(const std::locale& loc) = 0;
		/// Sets the locale of the response, if the response has not been committed yet. It
		/// also sets the response�s character encoding appropriately for the locale, if the
		/// character encoding has not been explicitly set using
		/// setContentType(String) or setCharacterEncoding(String) , get-
		/// Writer hasn�t been called yet, and the response hasn�t been committed yet. If
		/// the deployment descriptor contains a locale-encoding-mapping-list element,
		/// and that element provides a mapping for the given locale, that mapping
		/// is used. Otherwise, the mapping from locale to character encoding is container
		/// dependent.
		/// This method may be called repeatedly to change locale and character encoding.
		/// The method has no effect if called after the response has been committed.
		/// It does not set the response�s character encoding if it is called after
		/// setContentType(String) has been called with a charset specification, after
		/// setCharacterEncoding(String) has been called, after getWriter has been
		/// called, or after the response has been committed.
		/// Containers must communicate the locale and the character encoding used for
		/// the servlet response�s writer to the client if the protocol provides a way for
		/// doing so. In the case of HTTP, the locale is communicated via the Content-
		/// Language header, the character encoding as part of the Content-Type header
		/// for text media types. Note that the character encoding cannot be communicated
		/// via HTTP headers if the servlet does not specify a content type; however,
		/// it is still used to encode text written via the servlet response�s writer.
};


} } // namespace Poco::Servlet


#endif // Servlet_ServletResponse_INCLUDED