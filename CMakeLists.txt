cmake_minimum_required (VERSION 2.6)
project (HVBench)

file(GLOB_RECURSE sources src/*.cpp src/*.h)

find_package( Boost REQUIRED COMPONENTS system filesystem chrono thread )
include_directories( ${Boost_INCLUDE_DIRS} )
include_directories( src/libs/ )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(hvbench ${sources})

target_link_libraries(hvbench curl fluid_msg fluid_base ${Boost_LIBRARIES} event_pthreads pthread)

# Activate C++11:
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
	message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

