/**
 * @file    demo3.cpp
 * @author  Marvin Smith
 * @date    6/19/2023
 *
 *  Use the Accumulator with a Stats-Aggregator for
 *  tracking lots of stuff.
 *
 * This application simulates 3 sequential stages.
 * - The first runs clean, consistent, and without noticeable issues except for periodic
 *   failures.  The failures manifest themselves as immediate returns.
 *      - This will show how
 *   min/max can find failures when mean/std cannot.
 *
 * - The second set of methods has inconsistent timing due to an algorithm which takes
 *    longer to complete the longer it runs.
 *      - This will show how you can use accumulators and the stats aggregator to show degredation.
 *
 * - The third performs okay, but periodic degredation and is difficult to troubleshoot due to
 *    noisy data.
 *      - This shows how to utilize a rolling window so that you can spot bad behavior for a period
 *        of time without getting bogged down in noisy data.
 *
 *
*/

// C++ Includes
#include <chrono>
#include <deque>
#include <filesystem>
#include <functional>
#include <iostream>
#include <thread>

// Project Libraries
#include <lib-acc/Accumulator.hpp>
#include <lib-acc/Stopwatch.hpp>

// OpenCV Libraries
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

// Demo Libraries
#include "Thread_Pool.hpp"

/**
 * Really bad implementation of a prime sieve that gets comically bad performance
 * very quickly.
*/
class ReallyBadPrimeSieve
{
    public:

        ReallyBadPrimeSieve( size_t max_value )
         : m_is_prime( max_value, true )
        {
        }

        size_t getNextPrime()
        {
            // find the next index that's true
            m_current_pos++;
            while( m_current_pos < m_is_prime.size() )
            {
                if( m_is_prime[m_current_pos] )
                {
                    break;
                }
            }

            // Update all factors
            for( size_t idx = m_current_pos*2; idx < m_is_prime.size(); idx += m_current_pos )
            {
                m_is_prime[idx] = false;
            }
            return m_current_pos;
        }

    private:

        std::vector<bool> m_is_prime;
        size_t m_current_pos { 1 };
};

int main( int argc, char* argv[] )
{
    const size_t max_primes = 100000000;

    // Step 1:  Show increasingly bad performance
    ReallyBadPrimeSieve sieve( max_primes );
    

    std::cout << "End of Program" << std::endl;
    return 0;
}