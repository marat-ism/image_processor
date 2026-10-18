#include "image.h"
#include "errors.h"

namespace ip {

Image::Image(size_t width, size_t height) {
    if (width == 0 || height == 0) {
        throw ImageProcessorError("Image dimensions must be positive");
    }
    if (width > std::numeric_limits<size_t>::max() / height || height > std::numeric_limits<size_t>::max() / width) {
        throw ImageProcessorError("Image dimensions are too large");
    }
    width_ = width;
    height_ = height;
    pixels_.resize(static_cast<size_t>(width) * height);
}

Color& Image::At(size_t x, size_t y) {
    if (x >= width_ || y >= height_) {
        throw ImageProcessorError("Pixel coordinates out of range");
    }
    return pixels_[y * width_ + x];
}

const Color& Image::At(size_t x, size_t y) const {
    if (x >= width_ || y >= height_) {
        throw ImageProcessorError("Pixel coordinates out of range");
    }
    return pixels_[y * width_ + x];
}

const Color& Image::AtNear(int32_t x, int32_t y) const {
    if (x < 0) {
        x = 0;
    }
    if (y < 0) {
        y = 0;
    }
    if (x >= static_cast<int32_t>(width_)) {
        x = static_cast<int32_t>(width_) - 1;
    }
    if (y >= static_cast<int32_t>(height_)) {
        y = static_cast<int32_t>(height_) - 1;
    }

    return pixels_[y * width_ + x];
}

Color& Image::AtNear(int32_t x, int32_t y) {
    if (x < 0) {
        x = 0;
    }
    if (y < 0) {
        y = 0;
    }
    if (x >= static_cast<int32_t>(width_)) {
        x = static_cast<int32_t>(width_) - 1;
    }
    if (y >= static_cast<int32_t>(height_)) {
        y = static_cast<int32_t>(height_) - 1;
    }

    return pixels_[y * width_ + x];
}

}