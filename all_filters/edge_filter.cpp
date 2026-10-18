#include "edge_filter.h"

#include <charconv>
#include <string_view>
#include <iostream>
#include <system_error>
#include <utility>

namespace {

constexpr ip::Matrix edge_matrix = {{{0.0f, -1.0f, 0.0f}, {-1.0f, 4.0f, -1.0f}, {0.0f, -1.0f, 0.0f}}};

}

namespace ip {
    
EdgeFilter::EdgeFilter(float threshold) : MatrixFilter(edge_matrix), GrayscaleFilter(), threshold_(threshold) {}

void EdgeFilter::Apply(Image& image) const {
    if (image.Empty()) {
        return;
    }
    for (Color& pixel : image.Pixels()) {
        pixel = GrayscaleFilter::Transform(pixel);
    }

    MatrixFilter::Apply(image);

    for (Color& pixel : image.Pixels()) {
        if (pixel.r > threshold_) {
            pixel = Color(1.0f, 1.0f, 1.0f);
        } else {
            pixel = Color(0.0f, 0.0f, 0.0f);
        }
    }
}

}