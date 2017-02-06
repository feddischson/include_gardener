cmake_minimum_required (VERSION 2.8.11)
project (INCLUDE_GARDENER)

find_package(Boost REQUIRED COMPONENTS system filesystem program_options)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
else()
  message( FATAL_ERROR "Boost not found." )
endif()
include_directories( inc )
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(  CMAKE_COMPILER_IS_GNUCXX )
  set(CMAKE_CXX_FLAGS "-O0 -fno-elide-constructors -pedantic-errors -ansi -Wextra -Wall -Winit-self -Wold-style-cast -Woverloaded-virtual -Wuninitialized -Wmissing-declarations -Winit-self -Werror" )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "-O0 -fno-elide-constructors -pedantic-errors -Wextra -Wall -Winit-self -Wold-style-cast -Woverloaded-virtual -Wuninitialized -Wmissing-declarations -Winit-self -Werror" )
endif()


find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc
     ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
     COMMENT "Generating API documentation with Doxygen" VERBATIM )
endif(DOXYGEN_FOUND)


add_executable ( include_gardener src/main.cpp src/include_entry.cpp src/parser.cpp )
TARGET_LINK_LIBRARIES( include_gardener ${Boost_LIBRARIES} )
