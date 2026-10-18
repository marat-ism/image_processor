#pragma once

#include "../filter.h"

namespace ip {

class CropFilter : public Filter {
public:
    size_t Width() const {
        return width_;
    }
    size_t Height() const {
        return height_;
    }
    CropFilter(size_t width, size_t height);
    void Apply(Image& image) const override;
    std::string_view Name() const noexcept override {
        return "crop";
    };
private:
    size_t width_;
    size_t height_;
};

}