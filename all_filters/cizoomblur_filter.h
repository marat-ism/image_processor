#pragma once

#include "../filter.h"

namespace ip {

// Zoom blur in the spirit of Core Image's CIZoomBlur: the picture looks as if it was taken
// while zooming in, every bright point is smeared into a streak that points away from the
// zoom center. The center itself stays sharp, the farther from it, the longer the streaks.
//
// A pixel p is the weighted average of the picture sampled along the ray through the center:
//     out(p) = sum_k w_k * in(c + (p - c) * z_k),     z_k = 1 - a/2 - (a/2) * g_k,   a = amount / 100
// g_k runs over [-3, 3] and w_k ~ exp(-g_k^2 / 2) is a Gaussian window, so the streaks fade out
// smoothly instead of ending abruptly. z = 1 is the pixel itself, z < 1 is closer to the center.
// The zoom factors do not depend on the image size: the same amount gives the same look
// on a small and on a big picture.
//
// CLI: -zoomblur [amount [center_x center_y]]
//   amount   - zoom strength (>= 0, default 20, 0 means "do nothing"), amount / 100 is the
//              zoom fraction: 20 smears a point at distance r over about 0.6 * r pixels
//   center_* - zoom center in pixels, origin in the top-left corner
//              (default: the middle of the image)
class CIZoomBlurFilter : public Filter {
public:
    struct Center {
        float x = 0.0f;
        float y = 0.0f;
        Center(float x_, float y_) : x(x_), y(y_) {}
        Center() : x(150.0f), y(150.0f) {}
    };

    static constexpr float kDefaultAmount = 20.0f;

    // The zoom center is the middle of the image.
    explicit CIZoomBlurFilter(float amount = kDefaultAmount);
    // The zoom center is given explicitly.
    CIZoomBlurFilter(float amount, float center_x, float center_y);

    void Apply(Image& image) const override;

    std::string_view Name() const noexcept override {
        return "zoomblur";
    }

    float Amount() const {
        return amount_;
    }

    // true if the center was given explicitly, false if it is the middle of the image.
    bool HasCenter() const {
        return has_center_;
    }

    // Meaningful only if HasCenter() is true.
    Center GetCenter() const {
        return center_;
    }

private:
    float amount_ = kDefaultAmount;
    bool has_center_ = false;
    Center center_ = Center();
};

}