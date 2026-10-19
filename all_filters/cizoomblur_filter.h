#pragma once

#include "../filter.h"

namespace ip {

class CIZoomBlurFilter : public Filter {
public:
    struct Center {
        double x = 0.0f;
        double y = 0.0f;
        Center(double x_, double y_) : x(x_), y(y_) {}
        Center() : x(150.0), y(150.0) {}
    };

    static constexpr double kDefaultAmount = static_cast<double>(20.0);

    explicit CIZoomBlurFilter(double amount = kDefaultAmount);
    CIZoomBlurFilter(double amount, double center_x, double center_y);

    void Apply(Image& image) const override;

    std::string_view Name() const noexcept override {
        return "zoomblur";
    }

    double Amount() const {
        return amount_;
    }

    bool HasCenter() const {
        return has_center_;
    }

    Center GetCenter() const {
        return center_;
    }

private:
    double amount_ = kDefaultAmount;
    bool has_center_ = false;
    Center center_ = Center();
};

}