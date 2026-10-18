#pragma once

#include "../filter.h"

namespace ip {
    
class GaussianFilter : public Filter {
public:
    GaussianFilter(float sigma);
    
    void Apply(Image& image) const override;

    std::string_view Name() const noexcept override {
        return "blur";
    }

    float Sigma() const {
        return sigma_;
    }

private:
    float sigma_ = 0.0f;
};

}