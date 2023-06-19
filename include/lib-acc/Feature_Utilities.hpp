/**
 * @file    Feature_Utilities.hpp
 * @author  Marvin Smith
 * @date    6/14/2023
*/
#pragma once

// Project Libraries
#include "Features.hpp"

// Boost Libraries
#include <boost/mpl/find.hpp>

// C++ Libraries
#include <functional>

namespace acc::stats {

/**
 * @struct has_feature
 * Simple structure to check if a feature-set has a specific feature
*/
template <typename FEATURE_SET,
          typename FEATURE_TP>
struct has_feature
{
    /// Primary find operation
    typedef boost::mpl::find<FEATURE_SET,FEATURE_TP> find_tp;

    /// position type
    typedef typename find_tp::type::pos::type position_tp;

    /// Number of items in the feature set
    typedef typename boost::mpl::size<FEATURE_SET>::type feature_size;

    typedef typename boost::mpl::equal_to<position_tp,feature_size> comare_res;

    /// Check if the position is at the end
    typedef typename std::negation<comare_res> result;

}; // End of has_feature class

// @todo  Get this working
struct func_mapper
{
    /**
     * @brief Do the main function
     */
    template <typename SAMPLE_TP,
              typename FEATURE_SET,
              typename FEATURE_TP,
              typename RETURN_TP>
              typename std::enable_if< has_feature<FEATURE_SET,
                                                   FEATURE_TP>::result::value,
                                        std::optional<RETURN_TP>>::type
    operation( const boost::accumulators::accumulator_set<SAMPLE_TP,FEATURE_SET>& acc,
               std::function<std::optional<RETURN_TP>( const boost::accumulators::accumulator_set<SAMPLE_TP,FEATURE_SET>& )> work )
    {
        work( acc );
    }

    /**
     * @brief Do the fallback function
     */
    template <typename SAMPLE_TP,
              typename FEATURE_SET,
              typename FEATURE_TP,
              typename RETURN_TP>
              typename std::enable_if<!has_feature<FEATURE_SET,
                                                   FEATURE_TP>::result::value,
                                        std::optional<RETURN_TP>>::type
    operation( const boost::accumulators::accumulator_set<SAMPLE_TP,FEATURE_SET>& acc,
               std::function<std::optional<RETURN_TP>( const boost::accumulators::accumulator_set<SAMPLE_TP,FEATURE_SET>& )> work )
    {
        return {};
    }

};

/**
 * @brief Get the count, if enabled
*/
template <typename SAMPLE_TP,
          typename FEATURE_SET>
typename std::enable_if< has_feature<FEATURE_SET,
                                     count_stat>::result::value,
                         std::optional<SAMPLE_TP>>::type
 count( const boost::accumulators::accumulator_set<SAMPLE_TP,FEATURE_SET>& acc )
{
    return boost::accumulators::count( acc );
}

/**
 * @brief Return a dummy count
*/
template <typename SAMPLE_TP,
          typename FEATURE_SET>
typename std::enable_if<!has_feature<FEATURE_SET,
                                     count_stat>::result::value,
                         std::optional<SAMPLE_TP>>::type
 count( const boost::accumulators::accumulator_set<SAMPLE_TP,FEATURE_SET>& acc )
{
    return {};
}

/**
 * @brief Get the max, if enabled
*/
template <typename SAMPLE_TP,
          typename FEATURE_SET>
typename std::enable_if< has_feature<FEATURE_SET,
                                     max_stat>::result::value,
                         std::optional<SAMPLE_TP>>::type
 max( const boost::accumulators::accumulator_set<SAMPLE_TP,FEATURE_SET>& acc )
{
    return boost::accumulators::max( acc );
}

template <typename SAMPLE_TP,
          typename FEATURE_SET>
typename std::enable_if<!has_feature<FEATURE_SET,
                                     max_stat>::result::value,
                         std::optional<SAMPLE_TP>>::type
 max( const boost::accumulators::accumulator_set<SAMPLE_TP,FEATURE_SET>& acc )
{
    return {};
}

/**
 * @brief Get the mean, if enabled
*/
template <typename SAMPLE_TP,
          typename FEATURE_SET>
typename std::enable_if< has_feature<FEATURE_SET,
                                     mean_stat>::result::value,
                         std::optional<SAMPLE_TP>>::type
 mean( const boost::accumulators::accumulator_set<SAMPLE_TP,FEATURE_SET>& acc )
{
    return boost::accumulators::mean( acc );
}

/**
 * @brief Return a dummy mean.
*/
template <typename SAMPLE_TP,
          typename FEATURE_SET>
typename std::enable_if<!has_feature<FEATURE_SET,
                                     mean_stat>::result::value,
                         std::optional<SAMPLE_TP>>::type
 mean( const boost::accumulators::accumulator_set<SAMPLE_TP,FEATURE_SET>& acc )
{
    return {};
}


/**
 * @brief Get the min, if enabled
*/
template <typename SAMPLE_TP,
          typename FEATURE_SET>
typename std::enable_if< has_feature<FEATURE_SET,
                                     min_stat>::result::value,
                         std::optional<SAMPLE_TP>>::type
 min( const boost::accumulators::accumulator_set<SAMPLE_TP,FEATURE_SET>& acc )
{
    return boost::accumulators::min( acc );
}

/**
 * @brief Get a dummy min
*/
template <typename SAMPLE_TP,
          typename FEATURE_SET>
typename std::enable_if<!has_feature<FEATURE_SET,
                                     min_stat>::result::value,
                         std::optional<SAMPLE_TP>>::type
 min( const boost::accumulators::accumulator_set<SAMPLE_TP,FEATURE_SET>& acc )
{
    return {};
}

/**
 * @brief Get the rolling mean, if enabled
*/
template <typename SAMPLE_TP,
          typename FEATURE_SET>
typename std::enable_if< has_feature<FEATURE_SET,
                                     rolling_mean_stat>::result::value,
                         std::optional<SAMPLE_TP>>::type
 rolling_mean( const boost::accumulators::accumulator_set<SAMPLE_TP,FEATURE_SET>& acc )
{
    return boost::accumulators::rolling_mean( acc );
}

/**
 * @brief Return a dummy rolling mean.
*/
template <typename SAMPLE_TP,
          typename FEATURE_SET>
typename std::enable_if<!has_feature<FEATURE_SET,
                                     rolling_mean_stat>::result::value,
                         std::optional<SAMPLE_TP>>::type
 rolling_mean( const boost::accumulators::accumulator_set<SAMPLE_TP,FEATURE_SET>& acc )
{
    return {};
}

/**
 * @brief Get the rolling sum, if enabled
*/
template <typename SAMPLE_TP,
          typename FEATURE_SET>
typename std::enable_if< has_feature<FEATURE_SET,
                                     rolling_sum_stat>::result::value,
                         std::optional<SAMPLE_TP>>::type
 rolling_sum( const boost::accumulators::accumulator_set<SAMPLE_TP,FEATURE_SET>& acc )
{
    return boost::accumulators::rolling_sum( acc );
}

/**
 * @brief Return a dummy rolling sum.
*/
template <typename SAMPLE_TP,
          typename FEATURE_SET>
typename std::enable_if<!has_feature<FEATURE_SET,
                                     rolling_sum_stat>::result::value,
                         std::optional<SAMPLE_TP>>::type
 rolling_sum( const boost::accumulators::accumulator_set<SAMPLE_TP,FEATURE_SET>& acc )
{
    return {};
}

/**
 * @brief Get the rolling variance, if enabled
*/
template <typename SAMPLE_TP,
          typename FEATURE_SET>
typename std::enable_if< has_feature<FEATURE_SET,
                                     rolling_variance_stat>::result::value,
                         std::optional<SAMPLE_TP>>::type
 rolling_variance( const boost::accumulators::accumulator_set<SAMPLE_TP,FEATURE_SET>& acc )
{
    return boost::accumulators::rolling_variance( acc );
}

/**
 * @brief Return a dummy rolling variance.
*/
template <typename SAMPLE_TP,
          typename FEATURE_SET>
typename std::enable_if<!has_feature<FEATURE_SET,
                                     rolling_variance_stat>::result::value,
                         std::optional<SAMPLE_TP>>::type
 rolling_variance( const boost::accumulators::accumulator_set<SAMPLE_TP,FEATURE_SET>& acc )
{
    return {};
}

/**
 * @brief Get the variance, if enabled
*/
template <typename SAMPLE_TP,
          typename FEATURE_SET>
typename std::enable_if< has_feature<FEATURE_SET,
                                     variance_stat>::result::value,
                         std::optional<SAMPLE_TP>>::type
 variance( const boost::accumulators::accumulator_set<SAMPLE_TP,FEATURE_SET>& acc )
{
    return boost::accumulators::variance( acc );
}

/**
 * @brief Get a dummy rolling variance, if disabled.
*/
template <typename SAMPLE_TP,
          typename FEATURE_SET>
typename std::enable_if<!has_feature<FEATURE_SET,
                                     variance_stat>::result::value,
                         std::optional<SAMPLE_TP>>::type
 variance( const boost::accumulators::accumulator_set<SAMPLE_TP,FEATURE_SET>& acc )
{
    return {};
}

/**
 * @brief Get the sum, if enabled
*/
template <typename SAMPLE_TP,
          typename FEATURE_SET>
typename std::enable_if< has_feature<FEATURE_SET,
                                     sum_stat>::result::value,
                         std::optional<SAMPLE_TP>>::type
 sum( const boost::accumulators::accumulator_set<SAMPLE_TP,FEATURE_SET>& acc )
{
    return boost::accumulators::sum( acc );
}

template <typename SAMPLE_TP,
          typename FEATURE_SET>
typename std::enable_if<!has_feature<FEATURE_SET,
                                     sum_stat>::result::value,
                         std::optional<SAMPLE_TP>>::type
 sum( const boost::accumulators::accumulator_set<SAMPLE_TP,FEATURE_SET>& acc )
{
    return {};
}


} // End of acc::stats namespace