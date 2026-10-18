#include "matrix_filter.h"



namespace ip {

float& MatrixFilter::At(size_t x, size_t y) {
    if (x >= 3u || y >= 3u) {
        throw InvalidParameterError();
    }
    return matrix_[y][x];
}

const float& MatrixFilter::At(size_t x, size_t y) const {
    if (x >= 3u || y >= 3u) {
        throw InvalidParameterError();
    }
    return matrix_[y][x];
}

void MatrixFilter::Apply(Image& image) const {
    if (image.Empty()) {
        return;
    }

    Image result(image.Width(), image.Height());

    for (size_t y = 0; y < image.Height(); ++y) {
        for (size_t x = 0; x < image.Width(); ++x) {
            Color new_color(0.0f, 0.0f, 0.0f);
            for (int32_t dy = -1; dy <= 1; ++dy) {
                for (int32_t dx = -1; dx <= 1; ++dx) {
                    int32_t nx = static_cast<int32_t>(x) + dx;
                    int32_t ny = static_cast<int32_t>(y) + dy;
                    const Color& neighbor_color = image.AtNear(nx, ny);
                    float weight = At(static_cast<size_t>(dx) + 1, static_cast<size_t>(dy) + 1);
                    new_color.r += neighbor_color.r * weight;
                    new_color.g += neighbor_color.g * weight;
                    new_color.b += neighbor_color.b * weight;
                }
            }
            result.At(x, y).r = std::min(1.0f, std::max(0.0f, new_color.r));
            result.At(x, y).g = std::min(1.0f, std::max(0.0f, new_color.g));
            result.At(x, y).b = std::min(1.0f, std::max(0.0f, new_color.b));
        }
    }

    image = std::move(result);
}

}