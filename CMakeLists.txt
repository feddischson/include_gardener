cmake_minimum_required (VERSION 3.6)
project (INCLUDE_GARDENER)

set ( GARDENER_VERSION_MAJOR X )
set ( GARDENER_VERSION_MINOR X )
set ( GARDENER_VERSION_PATCH 0 )
set ( GARDENER_VERSION
  "${GARDENER_VERSION_MAJOR}.${GARDENER_VERSION_MINOR}.${GARDENER_VERSION_PATCH}" )

set ( CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost REQUIRED COMPONENTS log system filesystem program_options )
if(Boost_FOUND)
  include_directories( SYSTEM ${Boost_INCLUDE_DIRS})
else()
  message( FATAL_ERROR "Boost not found." )
endif()
include_directories( inc )
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(  CMAKE_COMPILER_IS_GNUCXX )
  set(CMAKE_CXX_FLAGS "-O0 -fno-elide-constructors -pedantic-errors -ansi -Wextra -Wall -Winit-self -Wold-style-cast -Woverloaded-virtual -Wuninitialized -Winit-self -Werror" )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "-O0 -fno-elide-constructors -pedantic-errors -Wextra -Wall -Winit-self -Wold-style-cast -Woverloaded-virtual -Wuninitialized -Winit-self -Werror" )
endif()

find_package (Threads REQUIRED )

#
# Doxygen Documentation Generation
#

find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc
     ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
     COMMENT "Generating API documentation with Doxygen" VERBATIM )
endif(DOXYGEN_FOUND)

##
## Main Target Generation
##

add_definitions( -DBOOST_LOG_DYN_LINK=1 )
add_definitions( -D_GARDENER_VERSION_MAJOR=${GARDENER_VERSION_MAJOR})
add_definitions( -D_GARDENER_VERSION_MINOR=${GARDENER_VERSION_MINOR})
add_definitions( -D_GARDENER_VERSION_PATCH=${GARDENER_VERSION_PATCH})
add_definitions( -D_GARDENER_VERSION="${GARDENER_VERSION}")

set (SOURCE_FILES
  ${CMAKE_SOURCE_DIR}/src/helper.cpp
  ${CMAKE_SOURCE_DIR}/src/statement_detector.cpp
  ${CMAKE_SOURCE_DIR}/src/file_detector.cpp
  ${CMAKE_SOURCE_DIR}/src/input_files.cpp
  ${CMAKE_SOURCE_DIR}/src/vertex.cpp
  ${CMAKE_SOURCE_DIR}/src/solver.cpp
  ${CMAKE_SOURCE_DIR}/src/solver_c.cpp )

set (EXEC_SOURCE_FILES
  ${SOURCE_FILES}
  ${CMAKE_SOURCE_DIR}/src/main.cpp )

set (UNIT_TEST_SOURCE_FILES
  ${CMAKE_SOURCE_DIR}/test/unit_test/main.cpp
  ${CMAKE_SOURCE_DIR}/test/unit_test/test_file_detector.cpp
  ${CMAKE_SOURCE_DIR}/test/unit_test/test_statement_detector.cpp 
  ${CMAKE_SOURCE_DIR}/test/unit_test/test_helper.cpp
  ${CMAKE_SOURCE_DIR}/test/unit_test/test_input_files.cpp
  ${CMAKE_SOURCE_DIR}/test/unit_test/test_solver.cpp)

add_executable ( include_gardener ${EXEC_SOURCE_FILES})

target_link_libraries( include_gardener ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries( include_gardener ${Boost_LIBRARIES} )

install( TARGETS include_gardener DESTINATION bin )

enable_testing()

#########
#
# Download and unpack googletest at configure time
#      configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
#
#
#
# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in
              googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()


execute_process(COMMAND ${CMAKE_COMMAND} --build .
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This adds
# the following targets: gtest, gtest_main, gmock
# and gmock_main
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                ${CMAKE_BINARY_DIR}/googletest-build)

# The gtest/gmock targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include"
                      "${gmock_SOURCE_DIR}/include")
endif()
#
########

##
## Unit-Test Executable Generation
##

add_executable( unit_test
  ${SOURCE_FILES}
  ${UNIT_TEST_SOURCE_FILES}
  )

add_dependencies( unit_test gmock )
add_dependencies( unit_test gtest )
add_dependencies( unit_test gtest_main )
link_directories( ${CMAKE_BINARY_DIR}/googletest-build/googlemock)
link_directories( ${CMAKE_BINARY_DIR}/googletest-build/googlemock/gtest)

target_link_libraries( unit_test  ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries( unit_test  ${Boost_LIBRARIES} )
target_link_libraries( unit_test  gtest_main )
target_link_libraries( unit_test  gmock_main )

set ( TEST_DIR ${CMAKE_SOURCE_DIR}/test )

##
## Unit-Test (gtest) execution
##
add_test( unit_test unit_test --gtest_output=xml:test-reports/unit_test_results.xml )

##
## cmdline test execution (via python)
##
add_test( c_cmdline_test python3 ${TEST_DIR}/cmd_line/test.py
  test-reports
  ${CMAKE_BINARY_DIR}/include_gardener ${TEST_DIR}/test_files/c )

##
## CPPCHECK
##
find_program(CPPCHECK "cppcheck")
add_custom_target(
        cppcheck
        COMMAND cppcheck
        --enable=warning,performance,portability,missingInclude,style
        --std=c++11
        --suppress=missingIncludeSystem
        --template="[{severity}][{id}][{file}:{line}] {message} {callstack}"
        --project=compile_commands.json
        --output-file=cpp_check.log
)

##
## CLANG-TIDY
##
find_program(CLANG_TIDY "clang-tidy")
add_custom_target(
        clang-tidy
        COMMAND clang-tidy
        -p=./
        -checks=*,-fuchsia-default-arguments,-llvm-header-guard
        -format-style=google
        -header-filter=.*
        ${EXEC_SOURCE_FILES}
        ${UNIT_TEST_SOURCE_FILES}
        -extra-arg=-std=c++17 > clang-tidy.log
)

