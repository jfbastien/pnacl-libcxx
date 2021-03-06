# Get sources
file(GLOB LIBCXX_SOURCES ../src/*.cpp)

# @LOCALMOD-START Bake libcxxabi into the libcxx build except on Mac, where
# we use the system libcxxabi. When we do this, also don't link against
# libcxxabi when building shared libraries.
file(GLOB LIBCXXABI_SOURCES ../../libcxxabi/src/*.cpp)
file(GLOB PNACL_EH_SUPPORT ../../libcxxabi/src/cxa_pnacl_sjlj_exception.cpp)
if ( NOT CMAKE_SYSTEM_NAME STREQUAL "nacl" )
  # Don't include pnacl EH support for non-nacl builds.
  list(REMOVE_ITEM LIBCXXABI_SOURCES ${PNACL_EH_SUPPORT})
endif()
  list(APPEND LIBCXX_SOURCES ${LIBCXXABI_SOURCES})
  set(LIBCXX_CXX_ABI_LIBRARY)
# @LOCALMOD-END

if(WIN32)
  file(GLOB LIBCXX_WIN32_SOURCES ../src/support/win32/*.cpp)
  list(APPEND LIBCXX_SOURCES ${LIBCXX_WIN32_SOURCES})
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "SunOS")
  file(GLOB LIBCXX_SOLARIS_SOURCES ../src/support/solaris/*.c)
  list(APPEND LIBCXX_SOURCES ${LIBCXX_SOLARIS_SOURCES})
endif()

# Add all the headers to the project for IDEs.
if (MSVC_IDE OR XCODE)
  file(GLOB_RECURSE LIBCXX_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/../include/*)
  if(WIN32)
    file( GLOB LIBCXX_WIN32_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/../include/support/win32/*.h)
    list(APPEND LIBCXX_HEADERS ${LIBCXX_WIN32_HEADERS})
  endif()
  # Force them all into the headers dir on MSVC, otherwise they end up at
  # project scope because they don't have extensions.
  if (MSVC_IDE)
    source_group("Header Files" FILES ${LIBCXX_HEADERS})
  endif()
endif()

if (LIBCXX_ENABLE_SHARED)
  add_library(cxx SHARED
    ${LIBCXX_SOURCES}
    ${LIBCXX_HEADERS}
    )
else()
  add_library(cxx STATIC
    ${LIBCXX_SOURCES}
    ${LIBCXX_HEADERS}
    )
endif()

if (DEFINED LIBCXX_CXX_ABI_DEPS)
  add_dependencies(cxx LIBCXX_CXX_ABI_DEPS)
endif()

# Generate library list.
set(libraries ${LIBCXX_CXX_ABI_LIBRARY})
append_if(libraries LIBCXX_HAS_PTHREAD_LIB pthread)
append_if(libraries LIBCXX_HAS_C_LIB c)
append_if(libraries LIBCXX_HAS_M_LIB m)
append_if(libraries LIBCXX_HAS_RT_LIB rt)
append_if(libraries LIBCXX_HAS_GCC_S_LIB gcc_s)

#if LIBCXX_CXX_ABI_LIBRARY_PATH is defined we want to add it to the search path.
if (DEFINED LIBCXX_CXX_ABI_LIBRARY_PATH)
  target_link_libraries(cxx "-L${LIBCXX_CXX_ABI_LIBRARY_PATH}")
endif()
target_link_libraries(cxx ${libraries})

# Setup flags.
# @LOCALMOD removed append_if(LIBCXX_COMPILE_FLAGS LIBCXX_HAS_FPIC_FLAG -fPIC)
append_if(LIBCXX_LINK_FLAGS LIBCXX_HAS_NODEFAULTLIBS_FLAG -nodefaultlibs)

# @LOCALMOD-START
if ( CMAKE_SYSTEM_NAME STREQUAL "Darwin" )
  # Needed to build against OSX 10.6 SDK:
  # http://lists.cs.uiuc.edu/pipermail/cfe-dev/2011-January/012999.html
  list(APPEND LIBCXX_COMPILE_FLAGS "-U__STRICT_ANSI__")
  # Do not re-export libc++abi symbols since they do not come from another dylib
  list(APPEND LIBCXX_LINK_FLAGS
      "-Wl,-unexported_symbols_list,${CMAKE_CURRENT_SOURCE_DIR}/libc++unexp.exp")
endif()

if ( False ) # @LOCALMOD-END
  if ( CMAKE_OSX_DEPLOYMENT_TARGET STREQUAL "10.6" )
    list(APPEND LIBCXX_COMPILE_FLAGS "-U__STRICT_ANSI__")
    list(APPEND LIBCXX_LINK_FLAGS
      "-compatibility_version 1"
      "-current_version 1"
      "-install_name /usr/lib/libc++.1.dylib"
      "-Wl,-reexport_library,/usr/lib/libc++abi.dylib"
      "-Wl,-unexported_symbols_list,${CMAKE_CURRENT_SOURCE_DIR}/libc++unexp.exp"
      "/usr/lib/libSystem.B.dylib")
  else()
    if ( ${CMAKE_OSX_SYSROOT} )
      list(FIND ${CMAKE_OSX_ARCHITECTURES} "armv7" OSX_HAS_ARMV7)
      if (OSX_HAS_ARMV7)
        set(OSX_RE_EXPORT_LINE
          "${CMAKE_OSX_SYSROOT}/usr/lib/libc++abi.dylib"
          "-Wl,-reexported_symbols_list,${CMAKE_CURRENT_SOURCE_DIR}/libc++sjlj-abi.exp")
      else()
        set(OSX_RE_EXPORT_LINE
          "-Wl,-reexport_library,${CMAKE_OSX_SYSROOT}/usr/lib/libc++abi.dylib")
      endif()
    else()
      set (OSX_RE_EXPORT_LINE "/usr/lib/libc++abi.dylib -Wl,-reexported_symbols_list,${CMAKE_CURRENT_SOURCE_DIR}/libc++abi${LIBCXX_LIBCPPABI_VERSION}.exp")
    endif()

    list(APPEND LIBCXX_LINK_FLAGS
      "-compatibility_version 1"
      "-install_name /usr/lib/libc++.1.dylib"
      "-Wl,-unexported_symbols_list,${CMAKE_CURRENT_SOURCE_DIR}/libc++unexp.exp"
      "${OSX_RE_EXPORT_LINE}"
      "-Wl,-force_symbols_not_weak_list,${CMAKE_CURRENT_SOURCE_DIR}/notweak.exp"
      "-Wl,-force_symbols_weak_list,${CMAKE_CURRENT_SOURCE_DIR}/weak.exp")
  endif()
endif()

string(REPLACE ";" " " LIBCXX_COMPILE_FLAGS "${LIBCXX_COMPILE_FLAGS}")
string(REPLACE ";" " " LIBCXX_LINK_FLAGS "${LIBCXX_LINK_FLAGS}")

set_target_properties(cxx
  PROPERTIES
    COMPILE_FLAGS "${LIBCXX_COMPILE_FLAGS}"
    LINK_FLAGS    "${LIBCXX_LINK_FLAGS}"
    OUTPUT_NAME   "c++"
    VERSION       "1.0"
    SOVERSION     "1"
  )

install(TARGETS cxx
  LIBRARY DESTINATION lib${LLVM_LIBDIR_SUFFIX}
  ARCHIVE DESTINATION lib${LLVM_LIBDIR_SUFFIX}
  )
