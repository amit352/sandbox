//
// Mutex_WIN32.h
//
// $Id: //poco/svn/Foundation/include/Poco/Interlocked.h#3 $
//
// Library: Foundation
// Package: Threading
// Module:  Mutex
//
// Definition of the interlocked definitions for Windows.
//
// Copyright (c) 2004-2008, Applied Informatics Software Engineering GmbH.
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
// Adapted from:
//
//  boost/detail/interlocked.hpp
//
//  Copyright 2005 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef Poco_Interlocked_INCLUDED
#define Poco_Interlocked_INCLUDED

#if !defined (POCO_OS_FAMILY_WINDOWS)
	#error ("Interlocked.h is Windows-only header file.")
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#if (POCO_OS == POCO_OS_WINDOWS_CE)

	extern "C" long __cdecl InterlockedIncrement( long* );
	extern "C" long __cdecl InterlockedDecrement( long* );
	extern "C" long __cdecl InterlockedCompareExchange( long*, long, long );
	extern "C" long __cdecl InterlockedExchange( long*, long );
	extern "C" long __cdecl InterlockedExchangeAdd( long*, long );

	# define POCO_INTERLOCKED_INCREMENT InterlockedIncrement
	# define POCO_INTERLOCKED_DECREMENT InterlockedDecrement
	# define POCO_INTERLOCKED_COMPARE_EXCHANGE InterlockedCompareExchange
	# define POCO_INTERLOCKED_EXCHANGE InterlockedExchange
	# define POCO_INTERLOCKED_EXCHANGE_ADD InterlockedExchangeAdd

	# define POCO_INTERLOCKED_COMPARE_EXCHANGE_POINTER(dest,exchange,compare) \
		((void*)POCO_INTERLOCKED_COMPARE_EXCHANGE((long*)(dest),(long)(exchange),(long)(compare)))
	# define POCO_INTERLOCKED_EXCHANGE_POINTER(dest,exchange) \
		((void*)POCO_INTERLOCKED_EXCHANGE((long*)(dest),(long)(exchange)))

#else

	extern "C" long __cdecl _InterlockedIncrement(long volatile*);
	extern "C" long __cdecl _InterlockedDecrement(long volatile*);
	extern "C" long __cdecl _InterlockedCompareExchange(long volatile*, long, long);
	extern "C" long __cdecl _InterlockedExchange(long volatile*, long);
	extern "C" long __cdecl _InterlockedExchangeAdd(long volatile*, long);

	# pragma intrinsic(_InterlockedIncrement)
	# pragma intrinsic(_InterlockedDecrement)
	# pragma intrinsic(_InterlockedCompareExchange)
	# pragma intrinsic(_InterlockedExchange)
	# pragma intrinsic(_InterlockedExchangeAdd)
	# pragma intrinsic(_ReadWriteBarrier)

	# if defined(_M_IA64) || defined(_M_AMD64)

		extern "C" void* __cdecl _InterlockedCompareExchangePointer(void* volatile *, void*, void*);
		extern "C" void* __cdecl _InterlockedExchangePointer(void* volatile *, void*);

		#pragma intrinsic(_InterlockedCompareExchangePointer)
		#pragma intrinsic(_InterlockedExchangePointer)

		#define POCO_INTERLOCKED_COMPARE_EXCHANGE_POINTER _InterlockedCompareExchangePointer
		#define POCO_INTERLOCKED_EXCHANGE_POINTER _InterlockedExchangePointer

	# else

		#  define POCO_INTERLOCKED_COMPARE_EXCHANGE_POINTER(dest,exchange,compare) \
			((void*)POCO_INTERLOCKED_COMPARE_EXCHANGE((long volatile*)(dest),(long)(exchange),(long)(compare)))
		#  define POCO_INTERLOCKED_EXCHANGE_POINTER(dest,exchange) \
			((void*)POCO_INTERLOCKED_EXCHANGE((long volatile*)(dest),(long)(exchange)))

	# endif

	# define POCO_INTERLOCKED_INCREMENT _InterlockedIncrement
	# define POCO_INTERLOCKED_DECREMENT _InterlockedDecrement
	# define POCO_INTERLOCKED_COMPARE_EXCHANGE _InterlockedCompareExchange
	# define POCO_INTERLOCKED_EXCHANGE _InterlockedExchange
	# define POCO_INTERLOCKED_EXCHANGE_ADD _InterlockedExchangeAdd

#endif

#if (_MSC_VER == 1400)
	extern "C" unsigned char _interlockedbittestandset(long *a,long b);
	extern "C" unsigned char _interlockedbittestandreset(long *a,long b);
#else
	extern "C" unsigned char _interlockedbittestandset(volatile long *a,long b);
	extern "C" unsigned char _interlockedbittestandreset(volatile long *a,long b);
#endif

#endif // Poco_Interlocked_INCLUDED
