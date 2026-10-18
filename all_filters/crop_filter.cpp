#include "crop_filter.h"

#include <algorithm>

namespace ip {

CropFilter::CropFilter(size_t width, size_t height) : width_(width), height_(height) {}

void CropFilter::Apply(Image& image) const {
    size_t cropped_width = std::min(width_, image.Width());
    size_t cropped_height = std::min(height_, image.Height());
    Image cropped_image(cropped_width, cropped_height);
    for (size_t y = 0; y < cropped_height; ++y) {
        for (size_t x = 0; x < cropped_width; ++x) {
            cropped_image.At(x, y) = image.At(x, y);
        }
    }
    image = std::move(cropped_image);
}

}