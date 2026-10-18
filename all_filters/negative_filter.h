#pragma once

#include "pixel_filter.h"



namespace ip {

class NegativeFilter : public PixelFilter {
public:
    std::string_view Name() const noexcept override {
        return "neg";
    }

    Color Transform(const Color& color) const override;
};

}