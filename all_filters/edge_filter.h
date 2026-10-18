#pragma once

#include "matrix_filter.h"
#include "grayscale_filter.h"

namespace ip {

class EdgeFilter : public MatrixFilter, public GrayscaleFilter {
public:

    explicit EdgeFilter(float threshold);

    std::string_view Name() const noexcept override {
        return "edge";
    }

    void Apply(Image& image) const override;

    float Threshold() const {
        return threshold_;
    }

private:
    float threshold_;
};

}