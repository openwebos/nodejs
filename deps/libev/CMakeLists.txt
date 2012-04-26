# Use of this file is subject to license terms as set forth in the LICENSE file found this directory.

include(CheckIncludeFiles)
include(CheckFunctionExists)
include(CheckLibraryExists)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${PROJECT_BINARY_DIR}/deps/libev)
add_definitions(-DHAVE_CONFIG_H=1 -DEV_MULTIPLICITY=0)

check_include_files(sys/inotify.h HAVE_SYS_INOTIFY_H)
check_include_files(sys/epoll.h HAVE_SYS_EPOLL_H)
check_include_files(sys/event.h HAVE_SYS_EVENT_H)
check_include_files(sys/queue.h HAVE_SYS_QUEUE_H)
check_include_files(port.h HAVE_PORT_H)
check_include_files(poll.h HAVE_POLL_H)
check_include_files(sys/select.h HAVE_SYS_SELECT_H)
check_include_files(sys/eventfd.h HAVE_SYS_EVENTFD_H)

check_function_exists(inotify_init HAVE_INOTIFY_INIT)
check_function_exists(epoll_ctl HAVE_EPOLL_CTL)
check_function_exists(kqueue HAVE_KQUEUE)
check_function_exists(port_create HAVE_PORT_CREATE)
check_function_exists(poll HAVE_POLL)
check_function_exists(select HAVE_SELECT)
check_function_exists(eventfd HAVE_EVENTFD)
check_function_exists(nanosleep HAVE_NANOSLEEP)

# check first without rt
check_function_exists(clock_gettime HAVE_CLOCK_GETTIME)

check_library_exists(rt clock_gettime "" HAVE_LIBRT)
# then check with rt
check_library_exists(rt clock_gettime "" HAVE_CLOCK_GETTIME)

check_library_exists(m ceil "" HAVE_LIBM)

# webOS BEGIN
include(CheckSymbolExists)

# We should be using <time.h>, but we have to use the raw kernel include file
# <linux/time.h> because glibc is often built against an older kernel
function(check_monotonic_raw)
  # Define some types just for this test so that it compiles (only seems to
  # be needed on the desktop)
  if(NOT WEBOS_DEVICE)
    set(CMAKE_REQUIRED_DEFINITIONS "-Dtime_t=long -Dsuseconds_t=long")
  endif()
  check_symbol_exists(CLOCK_MONOTONIC_RAW "linux/time.h" HAVE_RAW_MONOTONIC_CLOCK)
endfunction()

check_monotonic_raw()

# Use the raw monotonic clock if we find it
# Force it on webOS devices, since we know we do have it
if(HAVE_RAW_MONOTONIC_CLOCK OR WEBOS_DEVICE)
  # We have to #define CLOCK_MONOTONIC_RAW because of a few cascading failures
  # 1) glibc can be based on an older kernel, so the headers won't have
  #    CLOCK_MONOTONIC_RAW
  # 2) We can't include <linux/time.h> directly in the source without
  #    conflicting types with <time.h>
  # 3) We can't run any code when cross-compiling so we can't try to return
  #    the defined value
  add_definitions(-DCLOCK_MONOTONIC_RAW=4 -DHAVE_RAW_MONOTONIC_CLOCK=1 -DEV_USE_MONOTONIC_RAW=1)
endif()
# webOS END

configure_file(config.h.cmake ${PROJECT_BINARY_DIR}/deps/libev/config.h)
add_library (ev ev.c)
