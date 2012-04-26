# Use of this file is subject to license terms as set forth in the LICENSE file available in the root directory of the project.

#
# tests
#

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
  set(test_bin_dir debug)
  get_target_property(node_bin node DEBUG_LOCATION)
else()
  set(test_bin_dir default)
  get_target_property(node_bin node LOCATION)
endif()

file(GLOB_RECURSE node_tests ${CMAKE_SOURCE_DIR}/test/*)

# add all tests with add_test
foreach(test ${node_tests})
  if(test MATCHES ".*/test-[^./\ ]*.\\.js"
      AND NOT test MATCHES ".*disabled.*")

    if(WEBOS)
      # Check for "// Files:" or "// Flags:"
      set(extra_test_flags "")
      set(extra_test_files "")

      set(flag_regex "^//[ ]*Flags:[ ]*(.*)$")
      file(STRINGS ${test} extra_test_flags REGEX ${flag_regex})
      string(REGEX REPLACE "${flag_regex}" "\\1" extra_test_flags "${extra_test_flags}")

      set(files_regex "^//[ ]*Files:[ ]*(.*)$")
      file(STRINGS ${test} extra_test_files REGEX ${files_regex})
      string(REGEX REPLACE "${files_regex}" "\\1" extra_test_files "${extra_test_files}")
    endif()

    # build a fancy name for each test
    string(REPLACE ${CMAKE_SOURCE_DIR}/test/ "" test_name ${test})
    string(REPLACE test- "" test_name ${test_name})
    string(REPLACE ".js" "" test_name ${test_name})
    string(REPLACE "/" "-" test_name ${test_name})

    if(WEBOS)
      # List of tests to set an unlimited heap size
      set(unlimited_heap
        simple-http-buffer-sanity
        pummel-https-large-response
        pummel-tls-throttle
        simple-child-process-stdout-flush
        simple-stdout-to-file)
      list(FIND unlimited_heap ${test_name} is_unlimited)
      if(NOT is_unlimited EQUAL -1)
        add_test(${test_name} ${node_bin} --max_old_space_size=0 ${extra_test_flags} ${extra_test_files} ${test})
      else()
        add_test(${test_name} ${node_bin} ${extra_test_flags} ${extra_test_files} ${test})
      endif()
    else()
      add_test(${test_name} ${node_bin} ${test})
    endif()
    set_tests_properties(${test_name} PROPERTIES TIMEOUT 1200)
  endif()
endforeach()

# the CTest custom config makes ctest recreate the tmp directory before and after
# each run
configure_file(${CMAKE_SOURCE_DIR}/cmake/CTestCustom.cmake ${CMAKE_BINARY_DIR}/CTestCustom.cmake COPYONLY)

add_custom_command(
  TARGET node POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/${test_bin_dir}
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${node_bin} ${PROJECT_BINARY_DIR}/${test_bin_dir}
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

# this target gets overriden by ctest's test target
# add_custom_target(
#   test
#   COMMAND python tools/test.py --mode=release simple message
#   DEPENDS node
#   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
#   )

add_custom_target(test-all
  COMMAND python tools/test.py --mode=debug,release
  DEPENDS node
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  )

add_custom_target(test-release
  COMMAND python tools/test.py --mode=release
  DEPENDS node
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  )

add_custom_target(test-debug
  COMMAND python tools/test.py --mode=debug
  DEPENDS node
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  )

add_custom_target(test-message
  COMMAND python tools/test.py message
  DEPENDS node
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  )

add_custom_target(test-simple
  COMMAND python tools/test.py simple
  DEPENDS node
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  )

add_custom_target(test-pummel
  COMMAND python tools/test.py pummel
  DEPENDS node
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  )

add_custom_target(test-internet
  COMMAND python tools/test.py internet
  DEPENDS node
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  )
