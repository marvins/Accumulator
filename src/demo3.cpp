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
#include <algorithm>
#include <chrono>
#include <deque>
#include <filesystem>
#include <functional>
#include <iostream>
#include <map>
#include <thread>

// Project Libraries
#include <lib-acc/Accumulator.hpp>
#include <lib-acc/Stopwatch.hpp>

// Boost Libraries
#include <boost/log/trivial.hpp>

// Demo Libraries
#include "Thread_Pool.hpp"

class Contact
{
    public:
        Contact( const std::string& number,
                 const std::string& name )
        : m_number( number ),
          m_name( name )
        {
        }

        std::string number() const
        {
            return m_number;
        }

        std::string name() const
        {
            return m_name;
        }

        bool operator < ( const Contact& rhs ) const
        {
            return (m_number < rhs.m_number);
        }

        bool operator == ( const Contact& rhs ) const
        {
            return (m_number == rhs.m_number);
        }

        std::string print() const
        {
            return "Number: " + m_number + ", Name: " + m_name;
        }

    private:
        std::string m_number;
        std::string m_name;
};

class ReallyComicallyBadAddressBook
{
    public:

        void insert( const std::string& phone_number,
                     const std::string& name )
        {
            Contact ref_contact( phone_number, name );
            auto it = std::find( m_phone_numbers.begin(),
                                 m_phone_numbers.end(),
                                 ref_contact );

            if( it != m_phone_numbers.end() )
            {
                return;
            }
            m_phone_numbers.emplace_back( phone_number, name );
            std::sort( m_phone_numbers.begin(),
                       m_phone_numbers.end() );
        }

        size_t size() const
        {
            return m_phone_numbers.size();
        }

        std::string print() const
        {
            std::stringstream sout;
            sout << "Address Book" << std::endl;
            sout << "------------" << std::endl;
            for( const auto& contact : m_phone_numbers )
            {
                sout << contact.print() << std::endl;
            }
            return sout.str();
        }
    private:

        std::vector<Contact> m_phone_numbers;
        //std::deque<Contact> m_phone_numbers;
};

/**
 * @brief Method to randomly generate a US 10-digit phone number
*/
std::string Generate_Phone_Number()
{
    std::string output;
    for( size_t i=0; i < 10; i++ )
    {
        output += (char)(rand() % 10 + 48);
    }
    return output;
}

std::string Generate_Name()
{
    std::string output;

    size_t name_len = rand() % 15;
    for( size_t i=0; i < name_len; i++ )
    {
        output += (char)( rand() % 26 + 97 );
    }
    return output;
}

int main( int argc, char* argv[] )
{
    const size_t max_entries = 100000;

    // Step 1:  Show increasingly bad performance
    ReallyComicallyBadAddressBook address_book;

    auto timing_acc = acc::Accumulator<acc::ALL_FEATURE_SET, double>::create_rolling( "ms", 100 );

    size_t loops = 0;
    while( address_book.size() < max_entries )
    {
        // Create a randomly generated phone number
        auto phone_number = Generate_Phone_Number();
        auto contact      = Generate_Name();

        auto stopwatch = acc::Stopwatch<>();
        address_book.insert( phone_number,
                             contact );
        timing_acc.insert( stopwatch.stop().count() );

        if( loops++ % 1000 == 0 )
        {
            BOOST_LOG_TRIVIAL(info) << "Timing Results. Loop: " << loops << std::endl
                                    << timing_acc.toLogString<acc::print::pretty::Printer>();
            BOOST_LOG_TRIVIAL(info) << " Adding Entry " << phone_number << " " << contact
                                    << timing_acc.toLogString<acc::print::shell::Printer>();
        }
    }

    BOOST_LOG_TRIVIAL(info) << "End of Program";
    return 0;
}