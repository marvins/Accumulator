/**
 * @file    LogFormat.hpp
 * @author  Marvin Smith
 * @date    6/21/2023
*/
#pragma once

namespace acc {

enum class LogFormat : int
{
    PRETTY = 0 /*< Format easy on the eyes when digging through logs*/,
    SHELL  = 1 /*< Format easy to use with grep/awk/bash calls*/,
};

} // End of acc namespace
