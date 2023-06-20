/**
 * @file    TEST_boost.cpp
 * @author  Marvin Smith
 * @date    6/19/2023
 *
 * @note Test Boost Accumulator API calls directly
*/
#include <gtest/gtest.h>

// Boost API
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/density.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/utility.hpp>

// C++ API
#include <vector>

typedef boost::accumulators::stats<boost::accumulators::tag::density> DENSITY_FEATURE;

typedef boost::iterator_range<std::vector<std::pair<double, double> >::iterator > histogram_type;

/************************************************************/
/*          Test the Density Accumulator Functionality      */
/************************************************************/
TEST( boost_accumulators, density )
{

    const int mean_val = 100;
    const int radius   = 1;

    // Create Accumulator Set
    boost::accumulators::accumulator_set<double,DENSITY_FEATURE> acc( boost::accumulators::tag::density::cache_size = 10,
                                                                      boost::accumulators::tag::density::num_bins = 20 );

    for( int i=0; i<500; i++ )
    {
        double new_value = rand() % (radius * 2) + (mean_val - radius);
        acc( new_value );
    }
    // add a few noisy entries
    acc( 5 );
    acc( 0 );
    acc( 5000 );

    // Check the results
    histogram_type density_results = boost::accumulators::density( acc );

    double total = 0.0;

    for( int i = 0; i < density_results.size(); i++ )
    {
        std::cout << "Bin lower bound: " << density_results[i].first << ", Value: " << density_results[i].second << std::endl;
        total += density_results[i].second;
    }

    std::cout << "Total: " << total << std::endl; //should be 1 (and it is)

}