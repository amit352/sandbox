//
// SharedArray.h
//
// $Id: //poco/svn/Foundation/include/Poco/SharedArray.h#2 $
//
// Library: Foundation
// Package: Core
// Module: SharedArray
//
// Definition of the SharedArray template class.
//
// Copyright (c) 2005-2008, Applied Informatics Software Engineering GmbH.
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


#ifndef Foundation_SharedArray_INCLUDED
#define Foundation_SharedArray_INCLUDED


#include "Poco/SharedPtr.h"

namespace Poco {

template <class C, class RC = ReferenceCounter>
class SharedArray
/// SharedArray is a "smart" pointer for classes implementing
/// reference counting based garbage collection.
/// SharedArray is thus similar to SharedPtr. Unlike the
/// SharedPtr template, which can only be used with
/// single object pointer management, SharedArray
/// can be used with dynamically allocated array (Dynamically allocated
/// array are allocated with the C++ new[] expression).
///
/// SharedArray works as SharedPtr but using delete[] expression to
/// destroy the array object.
{
public:
    SharedArray(): _pCounter(new RC), _ptr(0)
    {
    }

    SharedArray(C* ptr): _pCounter(new RC), _ptr(ptr)
    {
    }

    template <class Other>
        SharedArray(const SharedArray<Other, RC>& ptr): _pCounter(ptr._pCounter), _ptr(const_cast<Other*>(ptr.get()))
    {
        _pCounter->duplicate();
    }

    SharedArray(const SharedArray& ptr): _pCounter(ptr._pCounter), _ptr(ptr._ptr)
    {
        _pCounter->duplicate();
    }

    ~SharedArray()
    {
        release();
    }

    SharedArray& assign(C* ptr)
    {
        if (get() != ptr)
        {
            RC* pTmp = new RC;
            release();
            _pCounter = pTmp;
            _ptr = ptr;
        }
        return *this;
    }

    SharedArray& assign(const SharedArray& ptr)
    {
        if (&ptr != this)
        {
            SharedArray tmp(ptr);
            swap(tmp);
        }
        return *this;
    }

    template <class Other>
        SharedArray& assign(const SharedArray<Other, RC>& ptr)
        {
            if (ptr.get() != _ptr)
            {
                SharedArray tmp(ptr);
                swap(tmp);
            }
            return *this;
        }

    SharedArray& operator = (C* ptr)
    {
        return assign(ptr);
    }

    SharedArray& operator = (const SharedArray& ptr)
    {
        return assign(ptr);
    }

    template <class Other>
        SharedArray& operator = (const SharedArray<Other, RC>& ptr)
        {
            return assign<Other>(ptr);
        }

    void swap(SharedArray& ptr)
    {
        std::swap(_ptr, ptr._ptr);
        std::swap(_pCounter, ptr._pCounter);
    }

    C& operator [] (std::ptrdiff_t index)
    {
        return deref(index);
    }

    const C& operator [] (std::ptrdiff_t index) const
    {
        return deref(index);
    }

    C* get()
    {
        return _ptr;
    }

    const C* get() const
    {
        return _ptr;
    }

    operator C* ()
    {
        return _ptr;
    }

    operator const C* () const
    {
        return _ptr;
    }

    operator bool () const
    {
        return isNull();
    }

    bool operator ! () const
    {
        return _ptr == 0;
    }

    bool isNull() const
    {
        return _ptr == 0;
    }

    bool operator == (const SharedArray& ptr) const
    {
        return get() == ptr.get();
    }

    bool operator == (const C* ptr) const
    {
        return get() == ptr;
    }

    bool operator == (C* ptr) const
    {
        return get() == ptr;
    }

    bool operator != (const SharedArray& ptr) const
    {
        return get() != ptr.get();
    }

    bool operator != (const C* ptr) const
    {
        return get() != ptr;
    }

    bool operator != (C* ptr) const
    {
        return get() != ptr;
    }

    bool operator < (const SharedArray& ptr) const
    {
        return get() < ptr.get();
    }

    bool operator < (const C* ptr) const
    {
        return get() < ptr;
    }

    bool operator < (C* ptr) const
    {
        return get() < ptr;
    }

    bool operator <= (const SharedArray& ptr) const
    {
        return get() <= ptr.get();
    }

    bool operator <= (const C* ptr) const
    {
        return get() <= ptr;
    }

    bool operator <= (C* ptr) const
    {
        return get() <= ptr;
    }

    bool operator > (const SharedArray& ptr) const
    {
        return get() > ptr.get();
    }

    bool operator > (const C* ptr) const
    {
        return get() > ptr;
    }

    bool operator > (C* ptr) const
    {
        return get() > ptr;
    }

    bool operator >= (const SharedArray& ptr) const
    {
        return get() >= ptr.get();
    }

    bool operator >= (const C* ptr) const
    {
        return get() >= ptr;
    }

    bool operator >= (C* ptr) const
    {
        return get() >= ptr;
    }

private:
    C& deref(std::ptrdiff_t index)
    {
        if (!_ptr)
            throw NullPointerException();

        return _ptr[index];
    }

    const C& deref(std::ptrdiff_t index) const
    {
        if (!_ptr)
            throw NullPointerException();

        return _ptr[index];
    }

    void release()
    {
        poco_assert_dbg (_pCounter);
        int i = _pCounter->release();
        if (i == 0)
        {
            if (_ptr)
                delete [] _ptr;
            _ptr = 0;

            delete _pCounter;
            _pCounter = 0;
        }
    }

    SharedArray(RC* pCounter, C* ptr): _pCounter(pCounter), _ptr(ptr)
                                       /// for cast operation
    {
        poco_assert_dbg (_pCounter);
        _pCounter->duplicate();
    }

private:
    RC* _pCounter;
    C* _ptr;

    template <class OtherC, class OtherRC> friend class SharedArray;
};


template <class C, class RC>
inline void swap(SharedArray<C, RC>& p1, SharedArray<C, RC>& p2)
{
    p1.swap(p2);
}


} // namespace Poco


#endif // Foundation_SharedArray_INCLUDED

