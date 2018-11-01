#pragma once

//STD libraries
#include <string>
#include <vector>
#include "strtk/strtk.hpp"

namespace CTL {
namespace util {
std::vector<int> processFramesSpecification(std::string frameSpecification, int dimz);
}}//namespace CTL::util
