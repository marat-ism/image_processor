#pragma once

#include "matrix_filter.h"

namespace ip {

class SharpeningFilter : public MatrixFilter {
public:
    SharpeningFilter(const std::vector<std::string_view>& params);

    SharpeningFilter();

    std::string_view Name() const noexcept override {
        return "sharp";
    }
};

}