#pragma once
//#define strtk_no_tr1_or_boost

// We do not have boost
#ifndef strtk_no_tr1_or_boost
#define strtk_no_tr1_or_boost
#endif

// STD libraries
#include "plog/Log.h"
#include "stringFormatter.h"
#include "strtk/strtk.hpp"
#include <algorithm> //std::remove_if
#include <cctype> //std::isspace
#include <regex>
#include <string>
#include <vector>

namespace CTL {
namespace util {
    std::vector<int> processFramesSpecification(std::string frameSpecification, int dimz);
}
} // namespace CTL
