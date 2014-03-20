# @LOCALMOD-START The PNaCl driver automatically adds dependencies to
#                 libc++ which hasn't been built yet. The tests for
#                 flags therefore fail because the library can't be
#                 found. We should fix this in a different CL, and add
#                 proper detection for newlib's lack of locale_t
#                 support. See:
#                  https://code.google.com/p/nativeclient/issues/detail?id=3661

set(LIBCXX_HAS_STDCXX0X_FLAG 1)
set(LIBCXX_HAS_FPIC_FLAG 0) # Not quite true, but untested in PNaCl.
set(LIBCXX_HAS_NODEFAULTLIBS_FLAG 1)
set(LIBCXX_HAS_NOSTDINCXX_FLAG 1)
set(LIBCXX_HAS_WALL_FLAG 1)
set(LIBCXX_HAS_W_FLAG 1)
set(LIBCXX_HAS_WNO_UNUSED_PARAMETER_FLAG 1)
set(LIBCXX_HAS_WWRITE_STRINGS_FLAG 1)
set(LIBCXX_HAS_WNO_LONG_LONG_FLAG 1)
set(LIBCXX_HAS_PEDANTIC_FLAG 1)
set(LIBCXX_HAS_WERROR_FLAG 1)
set(LIBCXX_HAS_WNO_ERROR_FLAG 1)
set(LIBCXX_HAS_FNO_EXCEPTIONS_FLAG 1)
set(LIBCXX_HAS_FNO_RTTI_FLAG 1)
# MSVC flags:
set(LIBCXX_HAS_WX_FLAG 0)
set(LIBCXX_HAS_NO_WX_FLAG 0)
set(LIBCXX_HAS_EHSC_FLAG 0)
set(LIBCXX_HAS_NO_EHS_FLAG 0)
set(LIBCXX_HAS_NO_EHA_FLAG 0)
set(LIBCXX_HAS_NO_GR_FLAG 0)

set(LIBCXX_HAS_PTHREAD_LIB 1)
set(LIBCXX_HAS_C_LIB 1)
set(LIBCXX_HAS_M_LIB 1)
set(LIBCXX_HAS_RT_LIB 0)
set(LIBCXX_HAS_GCC_S_LIB 0)
# @LOCALMOD-END

# Check C++0x features
if (LIBCXX_ENABLE_CXX0X)
  if (LIBCXX_HAS_STDCXX0X_FLAG)
    set(CMAKE_REQUIRED_DEFINITIONS -std=c++0x)
  endif()
else()
  set(LIBCXX_HAS_STDCXX0X_FLAG FALSE)
endif()
