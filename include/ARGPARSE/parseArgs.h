#pragma once

//We do not have boost
#ifndef strtk_no_tr1_or_boost
#define strtk_no_tr1_or_boost
#endif

//STD libraries
#include <string>
#include <vector>
#include <regex>
#include "plog/Log.h"
#include "stringFormatter.h"
#include "strtk/strtk.hpp"

namespace CTL {
namespace util {
std::vector<int> processFramesSpecification(std::string frameSpecification, int dimz);
}}//namespace CTL::util
