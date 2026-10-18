#include "sharpening_filter.h"

namespace {

constexpr ip::Matrix sharpening_matrix = ip::Matrix{{
    {0.0f, -1.0f, 0.0f},
    {-1.0f, 5.0f, -1.0f},
    {0.0f, -1.0f, 0.0f}
}};

}

namespace ip {

SharpeningFilter::SharpeningFilter(const std::vector<std::string_view>& params) : MatrixFilter(sharpening_matrix) {
    if (!params.empty()) {
        throw InvalidParameterError();
    }
}

SharpeningFilter::SharpeningFilter() : MatrixFilter(sharpening_matrix) {}

}