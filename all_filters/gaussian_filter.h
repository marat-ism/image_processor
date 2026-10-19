#pragma once

#include "../filter.h"

namespace ip {
    
class GaussianFilter : public Filter {
public:
    GaussianFilter(double sigma);
    
    void Apply(Image& image) const override;

    std::string_view Name() const noexcept override {
        return "blur";
    }

    double Sigma() const {
        return sigma_;
    }

private:
    double sigma_ = 0.0f;
};

}