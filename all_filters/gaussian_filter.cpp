#include "gaussian_filter.h"

#include <cmath>
#include <cstdint>
#include <system_error>
#include <iomanip>
#include <cassert>
#include <utility>

namespace ip {

namespace {

std::vector<double> MakeKernel(double sigma, size_t size) {
    size_t max_radius = size - 1;
    double wanted_radius = std::ceil(3.0f * static_cast<double>(sigma));
    size_t radius = 0;
    if (wanted_radius >= static_cast<double>(max_radius)) {
        radius = max_radius;
    } else {
        radius = static_cast<size_t>(wanted_radius);
    }
    double two_sigma_squared = 2.0f * static_cast<double>(sigma) * static_cast<double>(sigma);
    std::vector<double> kernel(2u * radius + 1u);
    double sum = 0.0;
    for (size_t i = 0; i < kernel.size(); ++i) {
        double dx = static_cast<double>(i) - static_cast<double>(radius);
        kernel[i] = std::exp(-(dx * dx) / two_sigma_squared);
        sum += kernel[i];
    }
    for (double& weight : kernel) {
        weight /= sum;
    }
    return kernel;
}

void BlurPass(const Image& image_from, Image& image_to, const std::vector<double>& kernel, bool horizontal) {
    int32_t radius = static_cast<int32_t>(kernel.size() / 2);
    for (size_t y = 0; y < image_from.Height(); ++y) {
        for (size_t x = 0; x < image_from.Width(); ++x) {
            int32_t cx = static_cast<int32_t>(x);
            int32_t cy = static_cast<int32_t>(y);
            double red = 0.0f;
            double green = 0.0f;
            double blue = 0.0f;
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
            image_to.At(x, y) = Color(static_cast<float>(red), static_cast<float>(green), static_cast<float>(blue));
        }
    }
}

}

GaussianFilter::GaussianFilter(double sigma) : sigma_(sigma) {}

void GaussianFilter::Apply(Image& image) const {
    if (image.Empty()) {
        return;
    }

    std::vector<double> horizontal_kernel = MakeKernel(sigma_, image.Width());
    std::vector<double> vertical_kernel = MakeKernel(sigma_, image.Height());

    Image horizontal(image.Width(), image.Height());
    Image result(image.Width(), image.Height());
    BlurPass(image, horizontal, horizontal_kernel, true);
    BlurPass(horizontal, result, vertical_kernel, false);

    image = std::move(result);
}

}