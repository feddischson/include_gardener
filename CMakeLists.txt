cmake_minimum_required (VERSION 2.8.11)
project (INCLUDE_GARDENER)

set ( GARDENER_VERSION_MAJOR 0 )
set ( GARDENER_VERSION_MINOR 1 )
set ( GARDENER_VERSION_PATCH 0 )
set ( GARDENER_VERSION
  "${GARDENER_VERSION_MAJOR}.${GARDENER_VERSION_MINOR}.${GARDENER_VERSION_PATCH}" )

find_package(Boost REQUIRED COMPONENTS log system filesystem program_options )
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

find_package (Threads REQUIRED )


find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc
     ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
     COMMENT "Generating API documentation with Doxygen" VERBATIM )
endif(DOXYGEN_FOUND)


add_definitions( -DBOOST_LOG_DYN_LINK=1 )
add_definitions( -D_GARDENER_VERSION_MAJOR=${GARDENER_VERSION_MAJOR})
add_definitions( -D_GARDENER_VERSION_MINOR=${GARDENER_VERSION_MINOR})
add_definitions( -D_GARDENER_VERSION_PATCH=${GARDENER_VERSION_PATCH})
add_definitions( -D_GARDENER_VERSION="${GARDENER_VERSION}")

add_executable ( include_gardener src/main.cpp src/include_entry.cpp src/parser.cpp )
target_link_libraries( include_gardener ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries( include_gardener ${Boost_LIBRARIES} )
install( TARGETS include_gardener DESTINATION bin )
