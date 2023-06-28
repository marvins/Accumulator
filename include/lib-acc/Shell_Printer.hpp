/**
 * @file    Shell_Printer.hpp
 * @author  Marvin Smith
 * @date    6/21/2023
*/
#pragma once

// C++ Libraries
#include <iomanip>
#include <sstream>
#include <string>

namespace acc::print::shell {


/**
 * Create "pretty" log outputs for various fields
*/
struct Printer
{
    /**
     * @brief Print generic field
    */
    template <typename SAMPLE_TYPE,
              size_t offset = 6>
    static std::string to_log_string( const std::string& key,
                                      const SAMPLE_TYPE& value,
                                      const std::string& units,
                                      int                precision )
    {
        std::stringstream sout;
        sout << std::fixed
             << std::setprecision( precision );

        sout << key << " " << value << " " << units << " ";
        return sout.str();
    }

}; // End of Printer class

} // end of acc::print::pretty namespace