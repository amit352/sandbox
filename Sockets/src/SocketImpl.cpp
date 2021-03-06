//
// SocketImpl.cpp
//
// $Id: //poco/svn/Net/src/SocketImpl.cpp#3 $
//
// Library: Net
// Package: Sockets
// Module:  SocketImpl
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


#include "Poco/Sockets/SocketImpl.h"
#include "Poco/Sockets/SocketException.h"
#include "Poco/Sockets/StreamSocketImpl.h"
#include "Poco/NumberFormatter.h"
#include "Poco/Timestamp.h"
#include "Poco/File.h"
//#include <string.h> // FD_SET needs memset on some platforms, so we can't use <cstring>
#include <cstring> // g++ 4.3 won't compile without <cstring>
#if defined(POCO_HAVE_FD_POLL)
	#include <poll.h>
#endif


using Poco::IOException;
using Poco::TimeoutException;
using Poco::InvalidArgumentException;
using Poco::NumberFormatter;
using Poco::Timespan;
using Poco::File;


namespace Poco {
namespace Sockets {


SocketImpl::SocketImpl():
	_sockfd(POCO_INVALID_SOCKET),
	_blocking(true)
{
}


SocketImpl::SocketImpl(poco_socket_t sockfd):
	_sockfd(sockfd),
	_blocking(true)
{
}


SocketImpl::~SocketImpl()
{
	close();
#ifdef POCO_OS_FAMILY_UNIX
	if (!_path.empty())
	{
		try
		{
			File f(_path);
			if (f.exists()) f.remove();
		} catch (Exception&) { }
	}
#endif
}

	
SocketImpl* SocketImpl::acceptConnection(SocketAddress& clientAddr)
{
	poco_assert (_sockfd != POCO_INVALID_SOCKET);

	char buffer[SocketAddress::MAX_ADDRESS_LENGTH] = { 0 };
	struct sockaddr* pSA = reinterpret_cast<struct sockaddr*>(buffer);
	poco_socklen_t saLen = sizeof(buffer);
	poco_socket_t sd;
	do
	{
		sd = ::accept(_sockfd, pSA, &saLen);
	}
	while (sd == POCO_INVALID_SOCKET && lastError() == POCO_EINTR);
	if (sd != POCO_INVALID_SOCKET)
	{
		clientAddr = SocketAddress(pSA, saLen);
		return new StreamSocketImpl(sd);
	}
	error(); // will throw
	return 0;
}


void SocketImpl::connect(const SocketAddress& address, const SocketAddress* pFromAddress)
{
	if (_sockfd == POCO_INVALID_SOCKET)
	{
		init(address.af());
#ifdef POCO_OS_FAMILY_UNIX
		if (pFromAddress && (AF_LOCAL == address.af()))
			bind(*pFromAddress, true);
		else if (!pFromAddress && (AF_LOCAL == address.af()))
			throw InvalidArgumentException("UNIX domain socket requires client address.");
#endif
	}

	int rc;
	do
	{
		rc = ::connect(_sockfd, address.addr(), address.length());
	}
	while (rc != 0 && lastError() == POCO_EINTR);
	if (rc != 0) error(address.toString());
}


void SocketImpl::connect(const SocketAddress& address,
	const Poco::Timespan& timeout,
	const SocketAddress* pFromAddress)
{
	poco_assert (_sockfd == POCO_INVALID_SOCKET);
	
	init(address.af());

#ifdef POCO_OS_FAMILY_UNIX
	if (pFromAddress && (AF_LOCAL == address.af()))
		bind(*pFromAddress, true);
	else if (!pFromAddress && (AF_LOCAL == address.af()))
		throw InvalidArgumentException("UNIX domain socket requires client address.");
#endif

	setBlocking(false);
	try
	{
		int rc = ::connect(_sockfd, address.addr(), address.length());
		if (rc != 0)
		{
			if (lastError() != POCO_EINPROGRESS && lastError() != POCO_EWOULDBLOCK)
				error(address.toString());
			if (!poll(timeout, SELECT_READ | SELECT_WRITE))
				throw Poco::TimeoutException("connect timed out", address.toString());
			int err = socketError();
			if (err != 0) error(err);
		}
	}
	catch (Poco::Exception&)
	{
		setBlocking(true);
		throw;
	}
	setBlocking(true);
}


void SocketImpl::connectNB(const SocketAddress& address, const SocketAddress* pFromAddress)
{
	if (_sockfd == POCO_INVALID_SOCKET)
	{
		init(address.af());
#ifdef POCO_OS_FAMILY_UNIX
		if (pFromAddress && (AF_LOCAL == address.af()))
			bind(*pFromAddress, true);
		else if (!pFromAddress && (AF_LOCAL == address.af()))
			throw InvalidArgumentException("UNIX domain socket requires client address.");
#endif
	}

	setBlocking(false);
	int rc = ::connect(_sockfd, address.addr(), address.length());
	if (rc != 0)
	{
		if (lastError() != POCO_EINPROGRESS && lastError() != POCO_EWOULDBLOCK)
			error(address.toString());
	}
}


void SocketImpl::bind(const SocketAddress& address, bool reuseAddress)
{
	if (_sockfd == POCO_INVALID_SOCKET)
	{
		init(address.af());
	}
	if (reuseAddress)
	{
		setReuseAddress(true);
		setReusePort(true);
	}

#ifdef POCO_OS_FAMILY_UNIX
	if (AF_LOCAL == address.af()) _path = address.toString();
#endif

	int rc = ::bind(_sockfd, address.addr(), address.length());
	if (rc != 0) error(address.toString());
}

	
void SocketImpl::listen(int backlog)
{
	poco_assert (_sockfd != POCO_INVALID_SOCKET);
	
	int rc = ::listen(_sockfd, backlog);
	if (rc != 0) error();
}


void SocketImpl::close()
{
	if (_sockfd != POCO_INVALID_SOCKET)
	{
		poco_closesocket(_sockfd);
		_sockfd = POCO_INVALID_SOCKET;
	}
}


void SocketImpl::shutdownReceive()
{
	poco_assert (_sockfd != POCO_INVALID_SOCKET);

	int rc = ::shutdown(_sockfd, 0);
	if (rc != 0) error();
}

	
void SocketImpl::shutdownSend()
{
	poco_assert (_sockfd != POCO_INVALID_SOCKET);

	int rc = ::shutdown(_sockfd, 1);
	if (rc != 0) error();
}

	
void SocketImpl::shutdown()
{
	poco_assert (_sockfd != POCO_INVALID_SOCKET);

	int rc = ::shutdown(_sockfd, 2);
	if (rc != 0) error();
}


int SocketImpl::sendBytes(const void* buffer, int length, int flags)
{
	poco_assert (_sockfd != POCO_INVALID_SOCKET);

#if defined(POCO_BROKEN_TIMEOUTS)
	if (_sndTimeout.totalMicroseconds() != 0)
	{
		if (!poll(_sndTimeout, SELECT_WRITE))
			throw TimeoutException();
	}
#endif

	int rc;
	do
	{
		rc = ::send(_sockfd, reinterpret_cast<const char*>(buffer), length, flags);
	}
	while (rc < 0 && lastError() == POCO_EINTR);
	if (rc < 0) error();
	return rc;
}


int SocketImpl::receiveBytes(void* buffer, int length, int flags)
{
	poco_assert (_sockfd != POCO_INVALID_SOCKET);

#if defined(POCO_BROKEN_TIMEOUTS)
	if (_recvTimeout.totalMicroseconds() != 0)
	{
		if (!poll(_recvTimeout, SELECT_READ))
			throw TimeoutException();
	}
#endif

	int rc;
	do
	{
		rc = ::recv(_sockfd, reinterpret_cast<char*>(buffer), length, flags);
	}
	while (rc < 0 && lastError() == POCO_EINTR);
	if (rc < 0) 
	{
		if (lastError() == POCO_EAGAIN || lastError() == POCO_ETIMEDOUT)
			throw TimeoutException();
		else
			error();
	}
	return rc;
}


int SocketImpl::sendTo(const void* buffer, int length, const SocketAddress& address, int flags)
{
	poco_assert (_sockfd != POCO_INVALID_SOCKET);

	int rc;
	do
	{
		rc = ::sendto(_sockfd, reinterpret_cast<const char*>(buffer), length, flags, address.addr(), address.length());
	}
	while (rc < 0 && lastError() == POCO_EINTR);
	if (rc < 0) error();
	return rc;
}


int SocketImpl::receiveFrom(void* buffer, int length, SocketAddress& address, int flags)
{
	poco_assert (_sockfd != POCO_INVALID_SOCKET);

#if defined(POCO_BROKEN_TIMEOUTS)
	if (_recvTimeout.totalMicroseconds() != 0)
	{
		if (!poll(_recvTimeout, SELECT_READ))
			throw TimeoutException();
	}
#endif
	
	char abuffer[SocketAddress::MAX_ADDRESS_LENGTH] = { 0 };
	struct sockaddr* pSA = reinterpret_cast<struct sockaddr*>(abuffer);
	poco_socklen_t saLen = sizeof(abuffer);
	int rc;
	do
	{
		rc = ::recvfrom(_sockfd, reinterpret_cast<char*>(buffer), length, flags, pSA, &saLen);
	}
	while (rc < 0 && lastError() == POCO_EINTR);
	if (rc >= 0)
	{
		address = SocketAddress(pSA, saLen);
	}
	else
	{
		if (lastError() == POCO_EAGAIN || lastError() == POCO_ETIMEDOUT)
			throw TimeoutException();
		else
			error();
	}
	return rc;
}


void SocketImpl::sendUrgent(unsigned char data)
{
	int rc = ::send(_sockfd, reinterpret_cast<const char*>(&data), sizeof(data), MSG_OOB);
	if (rc < 0) error();
}


int SocketImpl::available()
{
	int result;
	ioctl(FIONREAD, result);
	return result;
}

bool SocketImpl::poll(const Poco::Timespan& timeout, int mode)
{
#if defined(POCO_HAVE_FD_POLL)

	pollfd pollBuf;

	std::memset(&pollBuf, 0, sizeof(pollfd));
	pollBuf.fd = _sockfd;
	if (mode & SELECT_READ) pollBuf.events |= POLLIN;
	if (mode & SELECT_WRITE) pollBuf.events |= POLLOUT;

	Poco::Timespan remainingTime(timeout);
	int rc;
	do
	{
		Poco::Timestamp start;
		rc = ::poll(&pollBuf, 1, remainingTime.totalMilliseconds());
		
		if (rc < 0 && lastError() == POCO_EINTR)
		{
			Poco::Timestamp end;
			Poco::Timespan waited = end - start;
			if (waited < remainingTime)
				remainingTime -= waited;
			else
				remainingTime = 0;
		}
	} while (rc < 0 && lastError() == POCO_EINTR);

#else

	fd_set fdRead;
	fd_set fdWrite;
	fd_set fdExcept;
	FD_ZERO(&fdRead);
	FD_ZERO(&fdWrite);
	FD_ZERO(&fdExcept);
	if (mode & SELECT_READ)
	{
		FD_SET(_sockfd, &fdRead);
	}
	if (mode & SELECT_WRITE)
	{
		FD_SET(_sockfd, &fdWrite);
	}
	if (mode & SELECT_ERROR)
	{
		FD_SET(_sockfd, &fdExcept);
	}
	Poco::Timespan remainingTime(timeout);
	int rc;
	do
	{
		struct timeval tv;
		tv.tv_sec  = (long) remainingTime.totalSeconds();
		tv.tv_usec = (long) remainingTime.useconds();
		Poco::Timestamp start;
		rc = ::select(int(_sockfd) + 1, &fdRead, &fdWrite, &fdExcept, &tv);
		if (rc < 0 && lastError() == POCO_EINTR)
		{
			Poco::Timestamp end;
			Poco::Timespan waited = end - start;
			if (waited < remainingTime)
				remainingTime -= waited;
			else
				remainingTime = 0;
		}
	}
	while (rc < 0 && lastError() == POCO_EINTR);

#endif

	if (rc < 0) error();
	return rc > 0; 
}

	
void SocketImpl::setSendBufferSize(int size)
{
	setOption(SOL_SOCKET, SO_SNDBUF, size);
}

	
int SocketImpl::getSendBufferSize()
{
	int result;
	getOption(SOL_SOCKET, SO_SNDBUF, result);
	return result;
}


void SocketImpl::setReceiveBufferSize(int size)
{
	setOption(SOL_SOCKET, SO_RCVBUF, size);
}

	
int SocketImpl::getReceiveBufferSize()
{
	int result;
	getOption(SOL_SOCKET, SO_RCVBUF, result);
	return result;
}


void SocketImpl::setSendTimeout(const Poco::Timespan& timeout)
{
#if defined(_WIN32)
	int value = (int) timeout.totalMilliseconds();
	setOption(SOL_SOCKET, SO_SNDTIMEO, value);
#elif defined(POCO_BROKEN_TIMEOUTS)
	_sndTimeout = timeout;
#else
	setOption(SOL_SOCKET, SO_SNDTIMEO, timeout);
#endif
}


Poco::Timespan SocketImpl::getSendTimeout()
{
	Timespan result;
#if defined(_WIN32)
	int value;
	getOption(SOL_SOCKET, SO_SNDTIMEO, value);
	result = Timespan::TimeDiff(value)*1000;
#elif defined(POCO_BROKEN_TIMEOUTS)
	result = _sndTimeout;
#else
	getOption(SOL_SOCKET, SO_SNDTIMEO, result);
#endif
	return result;
}


void SocketImpl::setReceiveTimeout(const Poco::Timespan& timeout)
{
#ifndef POCO_BROKEN_TIMEOUTS
#if defined(_WIN32)
	int value = (int) timeout.totalMilliseconds();
	setOption(SOL_SOCKET, SO_RCVTIMEO, value);
#else
  setOption(SOL_SOCKET, SO_RCVTIMEO, timeout);
#endif
#else
	_recvTimeout = timeout;
#endif
}


Poco::Timespan SocketImpl::getReceiveTimeout()
{
	Timespan result;
#if defined(_WIN32)
	int value;
	getOption(SOL_SOCKET, SO_RCVTIMEO, value);
	result = Timespan::TimeDiff(value)*1000;
#elif defined(POCO_BROKEN_TIMEOUTS)
	result = _recvTimeout;
#else
	getOption(SOL_SOCKET, SO_RCVTIMEO, result);
#endif
	return result;
}

	
SocketAddress SocketImpl::address()
{
	poco_assert (_sockfd != POCO_INVALID_SOCKET);
	
	char buffer[SocketAddress::MAX_ADDRESS_LENGTH];
	struct sockaddr* pSA = reinterpret_cast<struct sockaddr*>(buffer);
	poco_socklen_t saLen = sizeof(buffer);
	int rc = ::getsockname(_sockfd, pSA, &saLen);
	if (rc == 0)
		return SocketAddress(pSA, saLen);
	else 
		error();
	return SocketAddress();
}

	
SocketAddress SocketImpl::peerAddress()
{
	poco_assert (_sockfd != POCO_INVALID_SOCKET);
	
	char buffer[SocketAddress::MAX_ADDRESS_LENGTH];
	struct sockaddr* pSA = reinterpret_cast<struct sockaddr*>(buffer);
	poco_socklen_t saLen = sizeof(buffer);
	int rc = ::getpeername(_sockfd, pSA, &saLen);
	if (rc == 0)
		return SocketAddress(pSA, saLen);
	else 
		error();
	return SocketAddress();
}


void SocketImpl::setOption(int level, int option, int value)
{
	setRawOption(level, option, &value, sizeof(value));
}


void SocketImpl::setOption(int level, int option, unsigned value)
{
	setRawOption(level, option, &value, sizeof(value));
}


void SocketImpl::setOption(int level, int option, unsigned char value)
{
	setRawOption(level, option, &value, sizeof(value));
}


void SocketImpl::setOption(int level, int option, const Address& value)
{
	setRawOption(level, option, value.addr(), value.length());
}


void SocketImpl::setOption(int level, int option, const Poco::Timespan& value)
{
	struct timeval tv;
	tv.tv_sec  = (long) value.totalSeconds();
	tv.tv_usec = (long) value.useconds();
	
	setRawOption(level, option, &tv, sizeof(tv));
}

	
void SocketImpl::setRawOption(int level, int option, const void* value, poco_socklen_t length)
{
	poco_assert (_sockfd != POCO_INVALID_SOCKET);
	
	int rc = ::setsockopt(_sockfd, level, option, reinterpret_cast<const char*>(value), length);
	if (rc == -1) error();
}


void SocketImpl::getOption(int level, int option, int& value)
{
	poco_socklen_t len = sizeof(value);
	getRawOption(level, option, &value, len);
}


void SocketImpl::getOption(int level, int option, unsigned& value)
{
	poco_socklen_t len = sizeof(value);
	getRawOption(level, option, &value, len);
}


void SocketImpl::getOption(int level, int option, unsigned char& value)
{
	poco_socklen_t len = sizeof(value);
	getRawOption(level, option, &value, len);
}


void SocketImpl::getOption(int level, int option, Poco::Timespan& value)
{
	struct timeval tv;
	poco_socklen_t len = sizeof(tv);
	getRawOption(level, option, &tv, len);
	value.assign(tv.tv_sec, tv.tv_usec);
}


void SocketImpl::getOption(int level, int option, Address& value)
{
	char buffer[Address::MAX_ADDRESS_LENGTH];
	poco_socklen_t len = sizeof(buffer);
	getRawOption(level, option, buffer, len);
	value = Address(buffer, len);
}


void SocketImpl::getRawOption(int level, int option, void* value, poco_socklen_t& length)
{
	poco_assert (_sockfd != POCO_INVALID_SOCKET);
	
	int rc = ::getsockopt(_sockfd, level, option, reinterpret_cast<char*>(value), &length);
	if (rc == -1) error();
}


void SocketImpl::setLinger(bool on, int seconds)
{
	struct linger l;
	l.l_onoff  = on ? 1 : 0;
	l.l_linger = seconds;
	setRawOption(SOL_SOCKET, SO_LINGER, &l, sizeof(l));
}

	
void SocketImpl::getLinger(bool& on, int& seconds)
{
	struct linger l;
	poco_socklen_t len = sizeof(l);
	getRawOption(SOL_SOCKET, SO_LINGER, &l, len);
	on      = l.l_onoff != 0;
	seconds = l.l_linger;
}


void SocketImpl::setNoDelay(bool flag)
{
	int value = flag ? 1 : 0;
	setOption(IPPROTO_TCP, TCP_NODELAY, value);
}


bool SocketImpl::getNoDelay()
{
	int value;
	getOption(IPPROTO_TCP, TCP_NODELAY, value);
	return value != 0;
}


void SocketImpl::setKeepAlive(bool flag)
{
	int value = flag ? 1 : 0;
	setOption(SOL_SOCKET, SO_KEEPALIVE, value);
}


bool SocketImpl::getKeepAlive()
{
	int value;
	getOption(SOL_SOCKET, SO_KEEPALIVE, value);
	return value != 0;
}


void SocketImpl::setReuseAddress(bool flag)
{
	int value = flag ? 1 : 0;
	setOption(SOL_SOCKET, SO_REUSEADDR, value);
}


bool SocketImpl::getReuseAddress()
{
	int value;
	getOption(SOL_SOCKET, SO_REUSEADDR, value);
	return value != 0;
}


void SocketImpl::setReusePort(bool flag)
{
#ifdef SO_REUSEPORT
	try
	{
		int value = flag ? 1 : 0;
		setOption(SOL_SOCKET, SO_REUSEPORT, value);
	}
	catch (IOException&)
	{
		// ignore error, since not all implementations
		// support SO_REUSEPORT, even if the macro
		// is defined.
	}
#endif
}


bool SocketImpl::getReusePort()
{
#ifdef SO_REUSEPORT
	int value;
	getOption(SOL_SOCKET, SO_REUSEPORT, value);
	return value != 0;
#else
	return false;
#endif
}


void SocketImpl::setOOBInline(bool flag)
{
	int value = flag ? 1 : 0;
	setOption(SOL_SOCKET, SO_OOBINLINE, value);
}


bool SocketImpl::getOOBInline()
{
	int value;
	getOption(SOL_SOCKET, SO_OOBINLINE, value);
	return value != 0;
}


void SocketImpl::setBroadcast(bool flag)
{
	int value = flag ? 1 : 0;
	setOption(SOL_SOCKET, SO_BROADCAST, value);
}

	
bool SocketImpl::getBroadcast()
{
	int value;
	getOption(SOL_SOCKET, SO_BROADCAST, value);
	return value != 0;
}


void SocketImpl::setBlocking(bool flag)
{
	int arg = flag ? 0 : 1;
	ioctl(FIONBIO, arg);
	_blocking = flag;
}


int SocketImpl::socketError()
{
	int result;
	getOption(SOL_SOCKET, SO_ERROR, result);
	return result;
}


void SocketImpl::init(int af)
{
	initSocket(af, SOCK_STREAM);
}


void SocketImpl::initSocket(int af, int type, int proto)
{
	poco_assert (_sockfd == POCO_INVALID_SOCKET);

	_sockfd = ::socket(af, type, proto);
	if (_sockfd == POCO_INVALID_SOCKET)
		error();
}


void SocketImpl::ioctl(int request, int& arg)
{
#if defined(_WIN32)
	int rc = ioctlsocket(_sockfd, request, reinterpret_cast<u_long*>(&arg));
#else
	int rc = ::ioctl(_sockfd, request, &arg);
#endif
	if (rc != 0) error();
}


void SocketImpl::ioctl(int request, void* arg)
{
#if defined(_WIN32)
	int rc = ioctlsocket(_sockfd, request, reinterpret_cast<u_long*>(arg));
#else
	int rc = ::ioctl(_sockfd, request, arg);
#endif
	if (rc != 0) error();
}


void SocketImpl::setSockfd(poco_socket_t aSocket)
{
	poco_assert(sockfd() == POCO_INVALID_SOCKET);
	_sockfd = aSocket;
}


void SocketImpl::error()
{
	std::string empty;
	error(lastError(), empty);
}


void SocketImpl::error(const std::string& arg)
{
	error(lastError(), arg);
}


void SocketImpl::error(int code)
{
	std::string arg;
	error(code, arg);
}


void SocketImpl::error(int code, const std::string& arg)
{
	switch (code)
	{
	case POCO_ESYSNOTREADY:
		throw SocketException("Net subsystem not ready", code);
	case POCO_ENOTINIT:
		throw SocketException("Net subsystem not initialized", code);
	case POCO_EINTR:
		throw IOException("Interrupted", code);
	case POCO_EACCES:
		throw IOException("Permission denied", code);
	case POCO_EFAULT:
		throw IOException("Bad address", code);
	case POCO_EINVAL:
		throw InvalidArgumentException(code);
	case POCO_EMFILE:
		throw IOException("Too many open files", code);
	case POCO_EWOULDBLOCK:
		throw IOException("Operation would block", code);
	case POCO_EINPROGRESS:
		throw IOException("Operation now in progress", code);
	case POCO_EALREADY:
		throw IOException("Operation already in progress", code);
	case POCO_ENOTSOCK:
		throw IOException("Socket operation attempted on non-socket", code);
	case POCO_EDESTADDRREQ:
		throw SocketException("Destination address required", code);
	case POCO_EMSGSIZE:
		throw SocketException("Message too long", code);
	case POCO_EPROTOTYPE:
		throw SocketException("Wrong protocol type", code);
	case POCO_ENOPROTOOPT:
		throw SocketException("Protocol not available", code);
	case POCO_EPROTONOSUPPORT:
		throw SocketException("Protocol not supported", code);
	case POCO_ESOCKTNOSUPPORT:
		throw SocketException("Socket type not supported", code);
	case POCO_ENOTSUP:
		throw SocketException("Operation not supported", code);
	case POCO_EPFNOSUPPORT:
		throw SocketException("Protocol family not supported", code);
	case POCO_EAFNOSUPPORT:
		throw SocketException("Address family not supported", code);
	case POCO_EADDRINUSE:
		throw SocketException("Address already in use", arg, code);
	case POCO_EADDRNOTAVAIL:
		throw SocketException("Cannot assign requested address", arg, code);
	case POCO_ENETDOWN:
		throw SocketException("Network is down", code);
	case POCO_ENETUNREACH:
		throw SocketException("Network is unreachable", code);
	case POCO_ENETRESET:
		throw SocketException("Network dropped connection on reset", code);
	case POCO_ECONNABORTED:
		throw ConnectionAbortedException(code);
	case POCO_ECONNRESET:
		throw ConnectionResetException(code);
	case POCO_ENOBUFS:
		throw IOException("No buffer space available", code);
	case POCO_EISCONN:
		throw SocketException("Socket is already connected", code);
	case POCO_ENOTCONN:
		throw SocketException("Socket is not connected", code);
	case POCO_ESHUTDOWN:
		throw SocketException("Cannot send after socket shutdown", code);
	case POCO_ETIMEDOUT:
		throw TimeoutException(code);
	case POCO_ECONNREFUSED:
		throw ConnectionRefusedException(arg, code);
	case POCO_EHOSTDOWN:
		throw SocketException("Host is down", code);
	case POCO_EHOSTUNREACH:
		throw SocketException("No route to host", code);
	default:
		throw IOException(NumberFormatter::format(code) + arg, code);
	}
}


} } // namespace Poco::Sockets
