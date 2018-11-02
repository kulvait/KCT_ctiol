#pragma once
//#define strtk_no_tr1_or_boost

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
