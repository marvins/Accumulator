/**
 * @file    Stopwatch.hpp
 * @author  Marvin Smith
 * @date    6/13/2023
*/
#pragma once

// C++ Libraries
#include <chrono>

namespace acc {

template <typename CLOCK_TP = std::chrono::steady_clock>
class Stopwatch
{
    public:

        /**
         * @brief Initializes the starting time
        */
        Stopwatch()
        {
            start();
        }

        /**
         * @brief Reset the start time.
        */
        void start()
        {
            m_start_time = CLOCK_TP::now();
        }

        template <typename DUR_TYPE = std::chrono::milliseconds>
        DUR_TYPE stop()
        {
            return std::chrono::duration_cast<DUR_TYPE>( CLOCK_TP::now() - m_start_time );
        }

    private:

        // Start Time
        typename CLOCK_TP::time_point m_start_time;

}; // End of Stopwatch Class

} // End of acc namespace