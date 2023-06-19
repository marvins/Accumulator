/**
 * @file    demo1.cpp
 * @author  Marvin Smith
 * @date    6/13/2023
 *
 *  Demonstrate usage of boost::accumulators API directly.
*/

// C++ Includes
#include <chrono>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>

// Boost Includes
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/count.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/sum.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/utility.hpp>

/**
 * @brief Simple function which sleeps for a bit to pretend like it's doing a complex task
*/
void Do_Complex_Task( const size_t avg_task_time_ms,
                      const size_t task_time_range_ms,
                      const double prob_of_random_event = 0,
                      const size_t max_random_task_time_deviation_ms = 100,
                      const double pct_of_zero_time_entries = 0 )
{
    // Compute the expected time to sleep
    size_t time_to_sleep_ms = rand() % task_time_range_ms + (avg_task_time_ms - task_time_range_ms/2);

    // Check if we need to add random noise
    size_t random_sleep_time = max_random_task_time_deviation_ms * ( (rand() % 100 )/100.0);
    if( ( rand() % 1000 ) < ( prob_of_random_event * 1000 ) )
    {
        time_to_sleep_ms += random_sleep_time;
    }

    std::this_thread::sleep_for( std::chrono::milliseconds( time_to_sleep_ms ) );
}

// Typedef to a "general-purpose" feature set
typedef boost::accumulators::stats<boost::accumulators::tag::mean,
                                   boost::accumulators::tag::min,
                                   boost::accumulators::tag::max,
                                   boost::accumulators::tag::sum,
                                   boost::accumulators::tag::count,
                                   boost::accumulators::tag::variance> FEATURE_SET;

/**
 * @brief Simple function to print contents of an accumulator.
*/
std::string print_accumulator( const boost::accumulators::accumulator_set<double,FEATURE_SET>& acc,
                               const std::string&                                              units )
{
    std::string gap( 4, ' ' );
    std::stringstream sout;

    // Recommend setting precision for when you print doubles.
    sout << std::setprecision(4);
    sout << std::fixed;

    sout << gap << "Count : " << boost::accumulators::count( acc ) << " entries" << std::endl;
    sout << gap << "Mean  : " << boost::accumulators::mean( acc ) << " " << units << std::endl;
    sout << gap << "Min   : " << boost::accumulators::min( acc ) << " " << units << std::endl;
    sout << gap << "Max   : " << boost::accumulators::max( acc ) << " " << units << std::endl;
    sout << gap << "StdDev: " << std::sqrt( boost::accumulators::variance( acc ) ) << " " << units << std::endl;
    sout << gap << "Sum   : " << boost::accumulators::sum( acc ) << " " << units << std::endl;

    return sout.str();
}

// Global mutexes
std::mutex acc_worker_mtx;

int main( int argc, char* argv[] )
{
    // Some handy const variables
    const size_t average_sleep_time_ms = 100;
    const size_t sleep_time_range_ms = 20;
    const size_t number_iterations = 1000;
    const double noise_pct = 0; // No random noise for this demo
    const size_t max_random_time_ms = 0;
    const double pct_of_zero_time_entries = 0.05;
    const size_t print_frequency = 100; // Every N loops

    // Create accumulators for tracking events
    boost::accumulators::accumulator_set<double,FEATURE_SET> acc_worker;

    // Do a big loop
    for( size_t i=0; i < number_iterations; i++ )
    {
        // Get the time
        auto start_time = std::chrono::steady_clock::now();

        // Perform the operation
        Do_Complex_Task( average_sleep_time_ms,
                         sleep_time_range_ms,
                         noise_pct,
                         max_random_time_ms,
                         pct_of_zero_time_entries );

        // Compute task time
        auto operation_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time);
        {
            std::unique_lock<std::mutex> lck( acc_worker_mtx );
            acc_worker( operation_time.count() );
        }

        // Print if needed
        if( ( i > 0 ) && ( ( i % print_frequency ) == 0 ) )
        {
            std::unique_lock<std::mutex> lck( acc_worker_mtx );
            std::cout << "Iteration " << i << " Accumulator Results\n" << print_accumulator( acc_worker, "ms" ) << std::endl;
        }

    }

    // Print final results
    std::cout << "Final Results After " << number_iterations << " Iterations" << std::endl;
    std::cout << print_accumulator( acc_worker, "ms" ) << std::endl;

    return 0;
}