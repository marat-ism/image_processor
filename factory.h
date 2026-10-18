#pragma once

#include <string_view>
#include <vector>
#include <memory>
#include <iostream>
#include <charconv>
#include <type_traits>
#include <system_error>
#include <cmath>

#include "filter.h"
#include "errors.h"

namespace ip::factory {

std::unique_ptr<Filter> CreateFilter(const std::string_view& type, const std::vector<std::string_view>& params);

std::vector<std::string> KnownFilters();

}