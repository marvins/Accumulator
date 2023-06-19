/**
 * @class   TEST_Features.cpp
 * @author  Marvin Smith
 * @date    6/14/2023
*/
#include <gtest/gtest.h>

// Project Libraries
#include <lib-acc/Features.hpp>

TEST( Features, FULL_FEATURE_SET_Validation )
{


    // Check features for FULL
    ASSERT_EQ( boost::mpl::size<acc::FULL_FEATURE_SET>::value, 6 );

    // Check that specific items exist in this instance
    ASSERT_EQ( (size_t)(boost::mpl::find<acc::FULL_FEATURE_SET,boost::accumulators::tag::rolling_sum>::type::pos::value), 6 );

}

TEST( Features, ROLLING_FEATURE_SET_Validation )
{
    // Check the size
    ASSERT_EQ( boost::mpl::size<acc::ROLLING_FEATURE_SET>::value, 6 );

    // Check that specific items exist in this instance
    ASSERT_EQ( (size_t)(boost::mpl::find<acc::ROLLING_FEATURE_SET,boost::accumulators::tag::rolling_sum>::type::pos::value), 1 );

    // Check that specific items do not exist in this instance
    ASSERT_EQ( (size_t)(boost::mpl::find<acc::ROLLING_FEATURE_SET,boost::accumulators::tag::rolling_sum>::type::pos::value), 1 );
}
