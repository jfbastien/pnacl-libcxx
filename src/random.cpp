//===-------------------------- random.cpp --------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#if defined(_WIN32)
// Must be defined before including stdlib.h to enable rand_s().
#define _CRT_RAND_S
#include <stdio.h>
#endif

#include "random"
#include "system_error"

#ifdef __sun__
#define rename solaris_headers_are_broken
#endif
#if !defined(_WIN32)
#include <fcntl.h>
#include <unistd.h>
#endif // defined(_WIN32)
#include <errno.h>
// @LOCALMOD-START
#if defined(__native_client__)
#include <nacl/nacl_random.h>
#endif
// @LOCALMOD-END

_LIBCPP_BEGIN_NAMESPACE_STD

#if defined(_WIN32)
random_device::random_device(const string&)
{
}

random_device::~random_device()
{
}

unsigned
random_device::operator()()
{
    unsigned r;
    errno_t err = rand_s(&r);
    if (err)
        __throw_system_error(err, "random_device rand_s failed.");
    return r;
}
#else
random_device::random_device(const string& __token)
// @LOCALMOD-START
#if defined(__native_client__)
{
    if (__token != "/dev/urandom")
        __throw_system_error(ENODEV, ("random device not supported " + __token).c_str());
    int error = nacl_secure_random_init();
    if (error)
        __throw_system_error(error, ("random device failed to open " + __token).c_str());
}
#else
    : __f_(open(__token.c_str(), O_RDONLY))
{
    if (__f_ <= 0)
        __throw_system_error(errno, ("random_device failed to open " + __token).c_str());
}
#endif
// @LOCALMOD-END

random_device::~random_device()
{
// @LOCALMOD-START
#if !defined(__native_client__)
    close(__f_);
#endif
// @LOCALMOD-END
}

unsigned
random_device::operator()()
{
    unsigned r;
// @LOCALMOD-START
#if defined(__native_client__)
    size_t bytes_written;
    int error = nacl_secure_random(&r, sizeof(r), &bytes_written);
    if (error != 0)
        __throw_system_error(error, "random_device failed getting bytes");
    else if (bytes_written != sizeof(r))
        __throw_runtime_error("random_device failed obtaining enough bytes");
#else
    read(__f_, &r, sizeof(r));
#endif
// @LOCALMOD-END
    return r;
}
#endif // defined(_WIN32)

double
random_device::entropy() const _NOEXCEPT
{
    return 0;
}

_LIBCPP_END_NAMESPACE_STD
