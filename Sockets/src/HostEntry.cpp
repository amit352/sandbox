//
// HostEntry.cpp
//
// $Id: //poco/Main/Sockets/src/HostEntry.cpp#8 $
//
// Library: Sockets
// Package: Address
// Module:  HostEntry
//
// Copyright (c) 2005-2006, Applied Informatics Software Engineering GmbH.
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


#include "Poco/Sockets/HostEntry.h"
#include "Poco/Exception.h"
#include <algorithm>


namespace Poco {
namespace Sockets {


//
// HostEntryImpl
//


HostEntryImpl::HostEntryImpl()
{
}

	
HostEntryImpl::HostEntryImpl(struct hostent* entry)
{
	poco_check_ptr (entry);
	
	_name = entry->h_name;	
	char** alias = entry->h_aliases;
	if (alias)
	{
		while (*alias)
		{
			_aliases.push_back(std::string(*alias));
			++alias;
		}
	}
	char** address = entry->h_addr_list;
	if (address)
	{
		while (*address)
		{
			_addresses.push_back(Address(*address, entry->h_length));
			++address;
		}
	}
}


#if defined(_WIN32) && defined(POCO_HAVE_IPv6)


HostEntryImpl::HostEntryImpl(struct addrinfo* ainfo)
{
	poco_check_ptr (ainfo);
	
	for (struct addrinfo* ai = ainfo; ai; ai = ai->ai_next)
	{
		if (ai->ai_canonname)
			_name.assign(ai->ai_canonname);
		else if (ai->ai_addrlen && ai->ai_addr)
			_addresses.push_back(Address(ai->ai_addr, (poco_socklen_t) ai->ai_addrlen));
	}
}


#endif


HostEntryImpl::~HostEntryImpl()
{
}


//
// HostEntry
//


HostEntry::HostEntry():
	_pImpl(new HostEntryImpl)
{
}

	
HostEntry::HostEntry(struct hostent* entry):
	_pImpl(new HostEntryImpl(entry))
{
}


#if defined(_WIN32) && defined(POCO_HAVE_IPv6)
HostEntry::HostEntry(struct addrinfo* info):
	_pImpl(new HostEntryImpl(info))
{
}
#endif


HostEntry::~HostEntry()
{
}


HostEntry::HostEntry(const HostEntry& entry):
	_pImpl(entry._pImpl)
{
}


HostEntry& HostEntry::operator = (const HostEntry& entry)
{
	HostEntry tmp(entry);
	tmp.swap(*this);
	return *this;
}


} } // namespace Poco::Sockets