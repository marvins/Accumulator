#    File:    CMakeLists.txt
#    Author:  Marvin Smith
#    Date:    6/14/2023

find_package( GTest REQUIRED )

enable_testing()

add_executable( acc_test
                TEST_Accumulator.cpp
                TEST_boost.cpp
                TEST_Features.cpp
                TEST_FeatureUtilities.cpp
)
target_link_libraries( acc_test
  GTest::gtest_main
)


gtest_discover_tests( acc_test )
