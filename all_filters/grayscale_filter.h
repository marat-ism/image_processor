#pragma once



#include "pixel_filter.h"

namespace ip {

class GrayscaleFilter : public PixelFilter {
public:
    std::string_view Name() const noexcept override {
        return "gs";
    }

    Color Transform(const Color& color) const override;
};

}