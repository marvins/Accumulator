/**
 * @file    Accumulator.hpp
 * @author  Marvin Smith
 * @date    6/13/2023
*/
#pragma once

// Boost Libraries
#include <boost/accumulators/accumulators.hpp>
#include <boost/utility.hpp>

// C++ Libraries
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <type_traits>

// Project Libraries
#include "Feature_Utilities.hpp"
#include "LogFormat.hpp"
#include "Shell_Printer.hpp"
#include "Pretty_Printer.hpp"

namespace acc {

/**
 * @class Accumulator
 *
 * Stores statistical information about events as events are added.
*/
template <typename FEATURE_SET = FULL_FEATURE_SET,
          typename SAMPLE_TP = double >
class Accumulator final
{
    public:

        typedef FEATURE_SET FEATURE_SET_TP;

        // Small helper function for enabling a function
        template< bool cond, typename U >
        using resolvedType  = typename std::enable_if< cond, U >::type;

        /**
         * @brief Create an accumulator
         * @note Rolling sum/count/mean accumulators require a window size, so you can't use this method.
        */
        template <typename = std::enable_if<std::negation<std::is_same<FEATURE_SET,ROLLING_FEATURE_SET>>::value>>
        static Accumulator<FEATURE_SET,SAMPLE_TP> create( const std::string& units )
        {
            return Accumulator<FEATURE_SET,SAMPLE_TP>( units, 0, false );
        }

        /**
         * @brief Build a rolling accumulator
        */
        template <typename = std::enable_if<std::is_same<FEATURE_SET,ROLLING_FEATURE_SET>::value>>
        static Accumulator<FEATURE_SET,SAMPLE_TP> create_rolling( const std::string& units,
                                                                  size_t             window_size )
        {
            return Accumulator<FEATURE_SET,SAMPLE_TP>( units, window_size );
        }

        /**
         * @brief Add new value to the accumulator
        */
        void insert( SAMPLE_TP new_value )
        {
            std::unique_lock<std::mutex> lck(m_acc_mtx);
            m_accumulator( new_value );
            m_last_entry_entered = new_value;
            m_insert_counter++;
            m_rolling_count = std::min( (int64_t)m_rolling_count + 1, (int64_t)m_insert_counter );
        }

        template<typename REP_TYPE,
                 typename RATIO_TYPE>
        void insert( const std::chrono::duration<REP_TYPE,RATIO_TYPE>& duration )
        {
            insert( (SAMPLE_TP)duration.count() );
        }

        /**
         * @brief Get a copy of the underlying accumulator
        */
        boost::accumulators::accumulator_set<SAMPLE_TP,FEATURE_SET> get_accumulator() const
        {
            return m_accumulator;
        }

        /**
         * @brief Get a reference to the underlying accumulator
        */
        boost::accumulators::accumulator_set<SAMPLE_TP,FEATURE_SET>& get_accumulator_ref()
        {
            return m_accumulator;
        }

        /**
         * @brief Get the number of items inserted into the accumulator
        */
        std::optional<int64_t> get_count() const
        {
            std::unique_lock<std::mutex> lck(m_acc_mtx);
            return stats::count( m_accumulator );
        }

        /**
         * @brief Check if the count is supported for this accumulator
        */
        bool has_count() const
        {
            std::unique_lock<std::mutex> lck(m_acc_mtx);
            return acc::stats::has_feature<FEATURE_SET,acc::count_stat>::result::value;
        }

        /**
         * @brief Get the number of items inserted into the accumulator,
         *        capped to the rolling window size.
        */
        std::optional<int64_t> get_rolling_count() const
        {
            std::unique_lock<std::mutex> lck(m_acc_mtx);
            return m_rolling_count;
        }

        /**
         * @brief Get the number of items inserted into the accumulator
        */
        int64_t number_items_inserted() const
        {
            std::unique_lock<std::mutex> lck(m_acc_mtx);
            return m_insert_counter;
        }

        /**
         * @brief Get the last entry inserted
        */
        SAMPLE_TP last_entry() const
        {
            std::unique_lock<std::mutex> lck(m_acc_mtx);
            return m_last_entry_entered;
        }

        /**
         * @brief Get the mean, if enabled
        */
        std::optional<SAMPLE_TP> get_mean() const
        {
            std::unique_lock<std::mutex> lck(m_acc_mtx);
            return stats::mean( m_accumulator );
        }

        /**
         * @brief Check if the mean is supported for this accumulator
        */
        bool has_mean() const
        {
            std::unique_lock<std::mutex> lck(m_acc_mtx);
            return acc::stats::has_feature<FEATURE_SET,acc::mean_stat>::result::value;
        }

        /**
         * @brief Get the rolling mean, if enabled
        */
        std::optional<SAMPLE_TP> get_rolling_mean() const
        {
            std::unique_lock<std::mutex> lck(m_acc_mtx);
            return stats::rolling_mean( m_accumulator );
        }

        /**
         * @brief Check if the rolling mean is supported for this accumulator
        */
        bool has_rolling_mean() const
        {
            std::unique_lock<std::mutex> lck(m_acc_mtx);
            return acc::stats::has_feature<FEATURE_SET,acc::rolling_mean_stat>::result::value;
        }

        /**
         * @brief Get the min value
        */
        std::optional<SAMPLE_TP> get_min() const
        {
            std::unique_lock<std::mutex> lck(m_acc_mtx);
            return stats::min( m_accumulator );
        }

        /**
         * @brief Check if the min is supported for this accumulator
        */
        bool has_min() const
        {
            std::unique_lock<std::mutex> lck(m_acc_mtx);
            return acc::stats::has_feature<FEATURE_SET,acc::min_stat>::result::value;
        }

        /**
         * @brief Get the max value
        */
        std::optional<SAMPLE_TP> get_max() const
        {
            std::unique_lock<std::mutex> lck(m_acc_mtx);
            return stats::max( m_accumulator );
        }

        /**
         * @brief Check if the max is supported for this accumulator
        */
        bool has_max() const
        {
            std::unique_lock<std::mutex> lck(m_acc_mtx);
            return acc::stats::has_feature<FEATURE_SET,acc::max_stat>::result::value;
        }

        /**
         * @brief Get the variance value
        */
        std::optional<SAMPLE_TP> get_variance() const
        {
            std::unique_lock<std::mutex> lck(m_acc_mtx);
            return stats::variance( m_accumulator );
        }

        /**
         * @brief Check if the variance is supported for this accumulator
        */
        bool has_variance() const
        {
            std::unique_lock<std::mutex> lck(m_acc_mtx);
            return acc::stats::has_feature<FEATURE_SET,acc::variance_stat>::result::value;
        }

        /**
         * @brief Get the rolling variance value
        */
        std::optional<SAMPLE_TP> get_rolling_variance() const
        {
            std::unique_lock<std::mutex> lck(m_acc_mtx);
            return stats::rolling_variance( m_accumulator );
        }

        /**
         * @brief Check if the rolling variance is supported for this accumulator
        */
        bool has_rolling_variance() const
        {
            std::unique_lock<std::mutex> lck(m_acc_mtx);
            return acc::stats::has_feature<FEATURE_SET,acc::rolling_variance_stat>::result::value;
        }

        /**
         * @brief Get the sum value
        */
        std::optional<SAMPLE_TP> get_sum() const
        {
            std::unique_lock<std::mutex> lck(m_acc_mtx);
            return stats::sum( m_accumulator );
        }

        /**
         * @brief Check if the sum is supported for this accumulator
        */
        bool has_sum() const
        {
            std::unique_lock<std::mutex> lck(m_acc_mtx);
            return acc::stats::has_feature<FEATURE_SET,acc::sum_stat>::result::value;
        }

        /**
         * @brief Get the rolling sum value
        */
        std::optional<SAMPLE_TP> get_rolling_sum() const
        {
            std::unique_lock<std::mutex> lck(m_acc_mtx);
            return stats::rolling_sum( m_accumulator );
        }

        /**
         * @brief Check if the rolling_sum is supported for this accumulator
        */
        bool has_rolling_sum() const
        {
            std::unique_lock<std::mutex> lck(m_acc_mtx);
            return acc::stats::has_feature<FEATURE_SET,acc::rolling_sum_stat>::result::value;
        }

        /**
         * @brief Print to pretty-string
        */
        template <typename PRINTER = acc::print::pretty::Printer>
        std::string toLogString( int precision = 6 ) const
        {
            std::stringstream sin;

            // Check insert counter
            if( m_insert_counter <= 0 )
            {
                std::cerr << "No Entries Accumulated." << std::endl;
                return sin.str();
            }

            if( has_count() )
            {
                sin << PRINTER::to_log_string( "Count",
                                               get_count().value(),
                                               m_units,
                                               precision );
            }
            if( has_mean() )
            {
                sin << PRINTER::to_log_string( "Mean",
                                               get_mean().value(),
                                               m_units,
                                               precision );
            }
            if( has_rolling_mean() )
            {
                sin << PRINTER::to_log_string( "Rolling Mean",
                                               get_rolling_mean().value(),
                                               m_units,
                                               precision );
            }
            if( has_min() )
            {
                sin << PRINTER::to_log_string( "Min",
                                               get_min().value(),
                                               m_units,
                                               precision );
            }
            if( has_max() )
            {
                sin << PRINTER::to_log_string( "Max",
                                               get_max().value(),
                                               m_units,
                                               precision );
            }
            if( has_rolling_variance() )
            {
                sin << PRINTER::to_log_string( "Rolling StdDev",
                                               std::sqrt(get_rolling_variance().value()),
                                               m_units,
                                               precision );
            }
            if( has_variance() )
            {
                sin << PRINTER::to_log_string( "StdDev",
                                               std::sqrt(get_variance().value()),
                                               m_units,
                                               precision );
            }
            if( has_rolling_variance() )
            {
                sin << PRINTER::to_log_string( "Rolling Variance",
                                               get_rolling_variance().value(),
                                               m_units,
                                               precision );
            }
            if( has_variance() )
            {
                sin << PRINTER::to_log_string( "Variance",
                                               get_variance().value(),
                                               m_units,
                                               precision );
            }
            if( has_rolling_sum() )
            {
                sin << PRINTER::to_log_string( "Rolling Sum",
                                               get_rolling_sum().value(),
                                               m_units,
                                               precision );
            }
            if( has_sum() )
            {
                sin << PRINTER::to_log_string( "Sum",
                                               get_sum().value(),
                                               m_units,
                                               precision );
            }

            sin << PRINTER::to_log_string( "Last Entry",
                                           m_last_entry_entered.load(),
                                           m_units,
                                           precision );

            // Return string
            return sin.str();
        }

    private:

        template <typename FEATURE_TP>
        using find_feature_pos = typename boost::mpl::find<FEATURE_SET,FEATURE_TP>::type::pos;

        //template <typename FEATURE_TP>
        //using has_feature = boost::mpl::equal_to<find_feature_pos<FEATURE_TP>::value,boost::mpl::size<FEATURE_SET>::value>;

        /**
         * Constructor (Only for rolling sum features)
        */
        template < typename = typename std::enable_if<std::is_same<FEATURE_SET,ROLLING_FEATURE_SET>::value,bool> >
        explicit Accumulator( const std::string& units,
                              size_t             window_size )
          : m_window_size( window_size ),
            m_accumulator( boost::accumulators::tag::rolling_window::window_size = window_size ),
            m_units( units )
        {
        }

        /**
         * Constructor (For everything else)
        */
        template < typename = typename std::enable_if<std::negation<std::is_same<FEATURE_SET,ROLLING_FEATURE_SET>>::value,bool> >
        explicit Accumulator( const std::string& units,
                              size_t             window_size,
                              bool               dummy_value )
          : m_window_size( window_size ),
            m_accumulator(),
            m_units( units )
        {
        }

        /// Number of items inserted into accumulator
        std::atomic<int64_t> m_insert_counter { 0 };

        /// Size of the window for counting the rolling window
        std::optional<int> m_window_size;

        /// Rolling Count
        std::atomic<int64_t> m_rolling_count { 0 };

        /// Last Entry Pushed
        std::atomic<SAMPLE_TP> m_last_entry_entered;

        /// Accumulator Object
        boost::accumulators::accumulator_set<SAMPLE_TP,FEATURE_SET> m_accumulator;

        /// Unit of measure
        std::string m_units;

        /// Access Mutex
        mutable std::mutex m_acc_mtx;

}; // End of Accumulator Class

} // End of acc namespace
