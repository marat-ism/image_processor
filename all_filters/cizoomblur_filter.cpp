#include "cizoomblur_filter.h"

#include <algorithm>
#include <charconv>
#include <cmath>
#include <system_error>
#include <utility>

namespace ip {

namespace {

constexpr float kWindowSigmas = 3.0f;
constexpr size_t kMinHalf = 6;
constexpr size_t kMaxHalf = 256;
constexpr float kTinyWindow = 1e-3;

Color Sample(const Image& image, float x, float y) {
    x = std::max(0.0f, std::min(x, static_cast<float>(image.Width() - 1u)));
    y = std::max(0.0f, std::min(y, static_cast<float>(image.Height() - 1u)));

    size_t x0 = static_cast<size_t>(x);
    size_t y0 = static_cast<size_t>(y);
    size_t x1 = std::min(x0 + 1, image.Width() - 1);
    size_t y1 = std::min(y0 + 1, image.Height() - 1);
    float fx = x - static_cast<float>(x0);
    float fy = y - static_cast<float>(y0);

    Color c00 = image.At(x0, y0);
    Color c10 = image.At(x1, y0);
    Color c01 = image.At(x0, y1);
    Color c11 = image.At(x1, y1);

    float w00 = (1.0f - fx) * (1.0f - fy);
    float w10 = fx * (1.0f - fy);
    float w01 = (1.0f - fx) * fy;
    float w11 = fx * fy;

    return Color(c00.r * w00 + c10.r * w10 + c01.r * w01 + c11.r * w11,
               c00.g * w00 + c10.g * w10 + c01.g * w01 + c11.g * w11,
               c00.b * w00 + c10.b * w10 + c01.b * w01 + c11.b * w11);
}

std::vector<float> MakeWeights(size_t half) {
    std::vector<float> weights(2 * half + 1);
    float sum = 0.0;
    for (size_t i = 0; i < weights.size(); ++i) {
        float g = kWindowSigmas * (static_cast<float>(i) - static_cast<float>(half)) / static_cast<float>(half);
        weights[i] = std::expf(-g * g / 2.0f);
        sum += weights[i];
    }
    for (float& weight : weights) {
        weight /= sum;
    }
    return weights;
}

}

CIZoomBlurFilter::CIZoomBlurFilter(float amount) : amount_(amount) {}

CIZoomBlurFilter::CIZoomBlurFilter(float amount, float center_x, float center_y)
    : amount_(amount), has_center_(true), center_(Center(center_x, center_y)) {}

void CIZoomBlurFilter::Apply(Image& image) const {
    if (image.Empty() || amount_ == 0.0f) {
        return;
    }

    size_t width = image.Width();
    size_t height = image.Height();
    float center_x = has_center_ ? static_cast<float>(center_.x) : static_cast<float>(width - 1) / 2.0f;
    float center_y = has_center_ ? static_cast<float>(center_.y) : static_cast<float>(height - 1) / 2.0f;

    float zoom = static_cast<float>(amount_) / 100.0f;
    float mean = zoom / 2.0f;
    float sigma = zoom / 2.0f;

    std::vector<std::vector<float>> weights_by_half(kMaxHalf + 1);

    Image result(width, height);
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            float dx = static_cast<float>(x) - center_x;
            float dy = static_cast<float>(y) - center_y;
            float sigma_pixels = sigma * std::hypot(dx, dy);
            if (sigma_pixels < kTinyWindow) {
                result.At(x, y) = image.At(x, y);
                continue;
            }

            size_t half = static_cast<size_t>(std::max(static_cast<float>(kMinHalf), std::min(std::ceil(kWindowSigmas * sigma_pixels), static_cast<float>(kMaxHalf))));
            std::vector<float> weights = weights_by_half[half];
            if (weights.empty()) {
                weights = MakeWeights(half);
            }

            Color sum;
            for (size_t i = 0; i < weights.size(); ++i) {
                float g = kWindowSigmas * (static_cast<float>(i) - static_cast<float>(half)) / static_cast<float>(half);
                float z = 1.0f - mean - sigma * g;
                Color s = Sample(image, center_x + dx * z, center_y + dy * z);
                sum.r += weights[i] * s.r;
                sum.g += weights[i] * s.g;
                sum.b += weights[i] * s.b;
            }
            result.At(x, y) = Color(sum.r, sum.g, sum.b);
        }
    }

    image = std::move(result);
}

}