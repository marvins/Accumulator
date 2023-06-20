/**
 * @file    TEST_Accumulator.cpp
 * @author  Marvin Smith
 * @date    6/14/2023
*/
#include <gtest/gtest.h>

// C++ Libraries
#include <chrono>
#include <thread>

// Project Libraries
#include <lib-acc/Accumulator.hpp>
#include <lib-acc/Stopwatch.hpp>

/********************************************/
/*          Test the full feature set       */
/********************************************/
TEST( Accumulator, Full_Test_01 )
{
    // Create the accumulator
    auto acc = acc::Accumulator<acc::FULL_FEATURE_SET,double>::create( "ms" );

    // run a large number of iterations
    const int AVG_SLEEP_TIME_MS   = 50;
    const int SLEEP_TIME_RANGE_MS = 5;
    const size_t NUM_ITERATIONS = 250;
    double expected_size = 0;
    for( size_t iteration = 0; iteration < NUM_ITERATIONS; iteration++ )
    {
        // Sleep
        const int EXP_SLEEP = rand() % SLEEP_TIME_RANGE_MS + (AVG_SLEEP_TIME_MS - (SLEEP_TIME_RANGE_MS/2));
        acc::Stopwatch<> sleep_timer;
        std::this_thread::sleep_for( std::chrono::milliseconds( EXP_SLEEP ) );
        acc.insert( sleep_timer.stop().count() );
        expected_size += sleep_timer.stop().count();
    }

    // Log results
    std::cout << acc.toLogString() << std::endl;
    ASSERT_EQ( acc.get_count().value(), NUM_ITERATIONS );
    ASSERT_TRUE( acc.get_mean() );
    ASSERT_LE( std::fabs( acc.get_mean().value() - AVG_SLEEP_TIME_MS ), SLEEP_TIME_RANGE_MS );
    ASSERT_TRUE( acc.get_min() );
    ASSERT_TRUE( acc.get_max() );
    ASSERT_LT( acc.get_min().value(), acc.get_mean().value() );
    ASSERT_NEAR( acc.get_sum().value(), expected_size, 10 );
}

/***********************************************/
/*          Test the rolling feature set       */
/***********************************************/
TEST( Accumulator, Rolling_Test_01 )
{
    // Create the accumulator
    const int WINDOW_SIZE = 100;
    auto acc = acc::Accumulator<acc::ROLLING_FEATURE_SET,double>::create_rolling("ms", 100);

    // run a large number of iterations
    const int AVG_SLEEP_TIME_MS   = 50;
    const int SLEEP_TIME_RANGE_MS = 5;
    const size_t NUM_ITERATIONS = 250;
    for( size_t iteration = 0; iteration < NUM_ITERATIONS; iteration++ )
    {
        // Sleep
        const int EXP_SLEEP = rand() % SLEEP_TIME_RANGE_MS + (AVG_SLEEP_TIME_MS - (SLEEP_TIME_RANGE_MS/2));
        acc::Stopwatch<> sleep_timer;
        std::this_thread::sleep_for( std::chrono::milliseconds( EXP_SLEEP ) );
        acc.insert( sleep_timer.stop().count() );
    }

    // Log results
    std::cout << acc.toLogString() << std::endl;
    ASSERT_EQ( acc.get_count().value(), NUM_ITERATIONS );
    //ASSERT_LE( std::fabs( acc.mean() - AVG_SLEEP_TIME_MS ), SLEEP_TIME_RANGE_MS );
}