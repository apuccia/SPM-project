cmake_minimum_required( VERSION 3.16 )
project( SPM-project )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin" )
add_compile_options( -O3 -std=c++14 )
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( sequential.out ./src/sequential.cpp )
target_link_libraries( sequential.out ${OpenCV_LIBS} )

add_executable( farm.out ./src/farm.cpp )
target_include_directories( farm.out PRIVATE ./lib/fastflow-master )
target_link_libraries( farm.out PUBLIC -pthread ${OpenCV_LIBS})

add_executable( pipe.out ./src/pipe.cpp )
target_include_directories( pipe.out PRIVATE ./lib/fastflow-master )
target_link_libraries( pipe.out PUBLIC -pthread ${OpenCV_LIBS})