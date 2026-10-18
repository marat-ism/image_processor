#pragma once

#include <iostream>
#include <vector>
#include <limits>

#include "errors.h"


namespace ip {


struct Color {
    float r = 0;
    float g = 0;
    float b = 0;
    Color(float r_, float g_, float b_) : r(r_), g(g_), b(b_) {}
    Color() = default;
};

class Image {
public:
    Image(size_t width, size_t height);

    size_t Width() const {
        return width_;
    }

    size_t Height() const {
        return height_;
    }

    Color& At(size_t x, size_t y);

    const Color& At(size_t x, size_t y) const;

    const Color& AtNear(int32_t x, int32_t y) const;

    Color& AtNear(int32_t x, int32_t y);

    std::vector<Color>& Pixels() {
        return pixels_;
    }

    const std::vector<Color>& Pixels() const {
        return pixels_;
    }

    bool Empty() const {
        return pixels_.empty();
    }

private:
    size_t width_;
    size_t height_;
    std::vector<Color> pixels_;
};

}