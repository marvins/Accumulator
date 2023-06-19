/**
 * @file    Features.hpp
 * @author  Marvin Smith
 * @date    6/14/2023
*/
#pragma once

// Boost Libraries
#include <boost/accumulators/statistics/count.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/rolling_mean.hpp>
#include <boost/accumulators/statistics/rolling_sum.hpp>
#include <boost/accumulators/statistics/rolling_variance.hpp>
#include <boost/accumulators/statistics/sum.hpp>
#include <boost/accumulators/statistics/variance.hpp>

namespace acc {

/// Aliases to make for less crazy typing
typedef boost::accumulators::tag::count             count_stat;
typedef boost::accumulators::tag::max               max_stat;
typedef boost::accumulators::tag::mean              mean_stat;
typedef boost::accumulators::tag::min               min_stat;
typedef boost::accumulators::tag::rolling_mean      rolling_mean_stat;
typedef boost::accumulators::tag::rolling_sum       rolling_sum_stat;
typedef boost::accumulators::tag::rolling_variance  rolling_variance_stat;
typedef boost::accumulators::tag::sum               sum_stat;
typedef boost::accumulators::tag::variance          variance_stat;



/// This is a small set of features
typedef boost::accumulators::stats<mean_stat> TEST_FEATURE_SET;

/// Good "all-purpose" feature set
typedef boost::accumulators::stats<mean_stat,
                                   min_stat,
                                   max_stat,
                                   sum_stat,
                                   count_stat,
                                   variance_stat> FULL_FEATURE_SET;

/// This is a similar good "all-purpose" feature-set, but useful if you need a rolling mean.
typedef boost::accumulators::stats<rolling_mean_stat,
                                   rolling_sum_stat,
                                   rolling_variance_stat,
                                   min_stat,
                                   max_stat,
                                   count_stat> ROLLING_FEATURE_SET;

/// This is a good "all-purpose" set of features
typedef boost::accumulators::stats<mean_stat,
                                   min_stat,
                                   max_stat,
                                   sum_stat,
                                   count_stat,
                                   rolling_mean_stat,
                                   rolling_sum_stat,
                                   rolling_variance_stat,
                                   variance_stat> ALL_FEATURE_SET;

} // End of acc namespace
