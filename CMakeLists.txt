#  Name:    CMakeLists.txt
#  Author:  Marvin Smith
#  Date:    4/3/2023

cmake_minimum_required( VERSION 3.0 )

#  Project
project( Accumulator )

#  Set build parameters
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
set(CMAKE_CXX_STANDARD 20)

#  Import boost
find_package( Boost
                COMPONENTS
                    log
                REQUIRED )

include_directories( ${Boost_INCLUDE_DIR} )

find_package( OpenCV REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} )

include_directories( include )

add_subdirectory( test )

# Add source code
add_executable( acc-demo-01
                src/demo1.cpp
                include/lib-acc/Accumulator.hpp
                include/lib-acc/Features.hpp
                include/lib-acc/Stats_Aggregator.hpp
                include/lib-acc/Stopwatch.hpp )

                # Add source code
add_executable( acc-demo-02
                src/demo2.cpp
                include/lib-acc/Accumulator.hpp
                include/lib-acc/Features.hpp
                include/lib-acc/Stats_Aggregator.hpp
                include/lib-acc/Stopwatch.hpp )

target_link_libraries( acc-demo-02
                       ${OpenCV_LIBRARIES} )
