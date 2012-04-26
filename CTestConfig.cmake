# Use of this file is subject to license terms as set forth in the LICENSE file found in the root directory of the project.

set(CTEST_PROJECT_NAME "node")
set(CTEST_NIGHTLY_START_TIME "00:00:00 EST")

set(CTEST_DROP_METHOD "http")
if(WEBOS)
  set(CTEST_DROP_SITE "cdash-dev.palm.com")
else()
  set(CTEST_DROP_SITE "my.cdash.org")
endif()
set(CTEST_DROP_LOCATION "/submit.php?project=node")
set(CTEST_DROP_SITE_CDASH TRUE)

if(WEBOS)
  set(VALGRIND_COMMAND_OPTIONS "-q --tool=memcheck --leak-check=full --show-reachable=yes --leak-resolution=low --workaround-gcc296-bugs=no --num-callers=50 --track-origins=yes")
endif()