//
// RegExpFilter.h
//
// $Id: //poco/1.3/Logging/include/Poco/Logging/RegExpFilter.h#2 $
//
// Library: Logging
// Package: Filters
// Module:  RegExpFilter
//
// Definition of the RegExpFilter class.
//
// Copyright (c) 2006-2009, Applied Informatics Software Engineering GmbH.
// All rights reserved.
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


#ifndef Logging_RegExpFilter_INCLUDED
#define Logging_RegExpFilter_INCLUDED


#include "Poco/Logging/Logging.h"
#include "Poco/Logging/AbstractFilter.h"
#include "Poco/RegularExpression.h"


namespace Poco {
namespace Logging {


class Logging_API RegExpFilter: public AbstractFilter
	/// A base class for all filters that implement
	/// regular expression based filtering.
{
public:
	RegExpFilter();
		/// Creates the RegExpFilter with no regular expression.

	RegExpFilter(const std::string& expr);
		/// Creates the RegExpFilter with the given regular expression.

	RegExpFilter(const std::string& expr, int options);
		/// Creates the RegExpFilter with the given regular expression and
		/// options. 
		/// See the Poco::RegularExpression class for a list of
		/// valid options.

	~RegExpFilter();
		/// Destroys the RegExpFilter.

	void setExpression(const std::string& expr);
		/// Sets the regular expression for filtering.
		
	const std::string& getExpression() const;
		/// Returns the regular expression used for filtering.

	void setOptions(int options);
		/// Sets options for the regular expression.
		/// See the Poco::RegularExpression class for a list of
		/// valid options.

	int getOptions() const;
		/// Returns the options for the regular expression.

	void setProperty(const std::string& name, const std::string& value);
		/// Only the "expr" and "options" properties are supported, which allows
		/// setting the regular expression and its options.
		///
		/// The following options are supported:
		///   - caseless: case insensitive matching
		///   - unanchored: the expression can match any substring and
		///                 must not match from the first character on.
		
	std::string getProperty(const std::string& name);
		/// Only the "expr" and "options" properties are supported.

	static void registerChannel();
		/// Registers the channel with the global LoggingFactory.

protected:
	bool match(const std::string& subject) const;
		/// Matches the subject against the regular expression.
	
	static int defaultOptions();
		/// Returns the default options.
		
private:
	std::string _expr;
	int _options;
	Poco::RegularExpression* _pExpr;
};


} } // namespace Poco::Logging


#endif // Logging_RegExpFilter_INCLUDED
