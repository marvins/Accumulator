/**
 * @file    Stats_Aggregator.hpp
 * @author  Marvin Smith
 * @date    6/13/2023
*/
#pragma once

namespace acc {

/**
 * @class Stats_Aggregator
 *
 * Stores accumulators, allowing a place to consolidate storing
 * random stats information.
*/
class Stats_Aggregator
{
    public:

        Stats_Aggregator() = default;

        void initialize()
        {
        }

        void finalize()
        {
        }

        template <typename VALUE_TP>
        void insert( const std::string& key,
                     const VALUE_TP&    value )
        {

        }

    private:

        /// List of accumulators


}; // End of class Stats_Aggregator

} // End of acc namespace