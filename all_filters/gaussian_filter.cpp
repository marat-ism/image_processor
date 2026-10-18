#include "gaussian_filter.h"

#include <cmath>
#include <cstdint>
#include <system_error>
#include <utility>

namespace ip {

namespace {

std::vector<float> MakeKernel(float sigma, size_t size) {
    size_t max_radius = size - 1;
    float wanted_radius = std::ceil(3.0f * static_cast<float>(sigma));
    size_t radius = 0;
    if (wanted_radius >= static_cast<float>(max_radius)) {
        radius = max_radius;
    } else {
        radius = static_cast<size_t>(wanted_radius);
    }
    float two_sigma_squared = 2.0f * static_cast<double>(sigma) * static_cast<float>(sigma);
    std::vector<float> kernel(2u * radius + 1u);
    float sum = 0.0;
    for (size_t i = 0; i < kernel.size(); ++i) {
        float dx = static_cast<float>(i) - static_cast<float>(radius);
        kernel[i] = std::exp(-(dx * dx) / two_sigma_squared);
        sum += kernel[i];
    }
    for (float& weight : kernel) {
        weight /= sum;
    }
    return kernel;
}

void BlurPass(const Image& image_from, Image& image_to, const std::vector<float>& kernel, bool horizontal) {
    int32_t radius = static_cast<int32_t>(kernel.size() / 2);
    for (size_t y = 0; y < image_from.Height(); ++y) {
        for (size_t x = 0; x < image_from.Width(); ++x) {
            int32_t cx = static_cast<int32_t>(x);
            int32_t cy = static_cast<int32_t>(y);
            float red = 0.0f;
            float green = 0.0f;
            float blue = 0.0f;
            for (size_t i = 0; i < kernel.size(); ++i) {
                int32_t dxy = static_cast<int32_t>(i) - radius;
                Color pixel = Color(0.0f, 0.0f, 0.0f);
                if (horizontal) {
                    pixel = image_from.AtNear(cx + dxy, cy);
                } else {
                    pixel = image_from.AtNear(cx, cy + dxy);
                }
                red += kernel[i] * pixel.r;
                green += kernel[i] * pixel.g;
                blue += kernel[i] * pixel.b;
            }
            image_to.At(x, y) = Color(red, green, blue);
        }
    }
}

}

GaussianFilter::GaussianFilter(float sigma) : sigma_(sigma) {}

void GaussianFilter::Apply(Image& image) const {
    if (image.Empty()) {
        return;
    }

    std::vector<float> horizontal_kernel = MakeKernel(sigma_, image.Width());
    std::vector<float> vertical_kernel = MakeKernel(sigma_, image.Height());

    Image horizontal(image.Width(), image.Height());
    Image result(image.Width(), image.Height());
    BlurPass(image, horizontal, horizontal_kernel, true);
    BlurPass(horizontal, result, vertical_kernel, false);

    image = std::move(result);
}

}