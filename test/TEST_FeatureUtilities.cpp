/**
 * @file    TEST_FeatureUtilities.cpp
 * @author  Marvin Smith
 * @date    6/14/2023
*/
#include <gtest/gtest.h>

// Project Libraries
#include <lib-acc/Feature_Utilities.hpp>

template< typename T >
class Foo
{

    template< bool cond, typename U >
    using resolvedType  = typename std::enable_if< cond, U >::type;

    public:
        template< typename U = T >
        resolvedType< std::is_same<T,double>::value, U > bar() {
            return 11;
        }
        template< typename U = T >
        resolvedType< std::is_same<T,int>::value, U > bar() {
            return 12;
        }

};

TEST( enable_if, demo01 )
{
    Foo<double> foo1;
    Foo<int> foo2;
    std::cout << foo1.bar() << std::endl;
    std::cout << foo2.bar() << std::endl;
}

TEST( Features, has_feature )
{
    // Test the has_feature method
    ASSERT_EQ( (int)(acc::stats::has_feature<acc::ROLLING_FEATURE_SET,acc::rolling_mean_stat>::position_tp::value), 0 );
    ASSERT_EQ( (int)(acc::stats::has_feature<acc::ROLLING_FEATURE_SET,acc::rolling_sum_stat>::position_tp::value), 1 );
    ASSERT_EQ( (int)(acc::stats::has_feature<acc::ROLLING_FEATURE_SET,acc::sum_stat>::position_tp::value), 6 );

    ASSERT_FALSE( (bool)(acc::stats::has_feature<acc::ROLLING_FEATURE_SET,acc::sum_stat>::result::value) );
    ASSERT_TRUE(  (bool)(acc::stats::has_feature<acc::FULL_FEATURE_SET,acc::sum_stat>::result::value) );
}

TEST( FeatureUtilities, mean )
{
    boost::accumulators::accumulator_set<double,acc::TEST_FEATURE_SET> acc;
    auto result = acc::stats::mean( acc );
}