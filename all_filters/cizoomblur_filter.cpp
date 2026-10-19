#include "cizoomblur_filter.h"

#include <algorithm>
#include <charconv>
#include <cmath>
#include <system_error>
#include <utility>

namespace ip {

namespace {

constexpr double kWindowSigmas = 3.0;
constexpr size_t kMinHalf = 6;
constexpr size_t kMaxHalf = 256;
constexpr double kTinyWindow = 1e-3;

Color Sample(const Image& image, double x, double y) {
    x = std::max(0.0, std::min(x, static_cast<double>(image.Width() - 1u)));
    y = std::max(0.0, std::min(y, static_cast<double>(image.Height() - 1u)));

    size_t x0 = static_cast<size_t>(x);
    size_t y0 = static_cast<size_t>(y);
    size_t x1 = std::min(x0 + 1, image.Width() - 1);
    size_t y1 = std::min(y0 + 1, image.Height() - 1);
    double fx = x - static_cast<double>(x0);
    double fy = y - static_cast<double>(y0);

    Color c00 = image.At(x0, y0);
    Color c10 = image.At(x1, y0);
    Color c01 = image.At(x0, y1);
    Color c11 = image.At(x1, y1);

    double w00 = (1.0 - fx) * (1.0 - fy);
    double w10 = fx * (1.0 - fy);
    double w01 = (1.0 - fx) * fy;
    double w11 = fx * fy;

    return Color(static_cast<float>(c00.r * w00 + c10.r * w10 + c01.r * w01 + c11.r * w11),
               static_cast<float>(c00.g * w00 + c10.g * w10 + c01.g * w01 + c11.g * w11),
               static_cast<float>(c00.b * w00 + c10.b * w10 + c01.b * w01 + c11.b * w11));
}

std::vector<double> MakeWeights(size_t radius) {
    std::vector<double> weights(2 * radius + 1);
    double sum = 0.0;
    for (size_t i = 0; i < weights.size(); ++i) {
        double g = kWindowSigmas * (static_cast<double>(i) - static_cast<double>(radius)) / static_cast<double>(radius);
        weights[i] = std::exp(-g * g / 2.0f);
        sum += weights[i];
    }
    for (double& weight : weights) {
        weight /= sum;
    }
    return weights;
}

}

CIZoomBlurFilter::CIZoomBlurFilter(double amount) : amount_(amount) {}

CIZoomBlurFilter::CIZoomBlurFilter(double amount, double center_x, double center_y)
    : amount_(amount), has_center_(true), center_(Center(center_x, center_y)) {}

void CIZoomBlurFilter::Apply(Image& image) const {
    if (image.Empty() || amount_ == 0.0f) {
        return;
    }

    size_t width = image.Width();
    size_t height = image.Height();
    double center_x = has_center_ ? static_cast<double>(center_.x) : static_cast<double>(width - 1) / 2.0;
    double center_y = has_center_ ? static_cast<double>(center_.y) : static_cast<double>(height - 1) / 2.0;

    double zoom = static_cast<double>(amount_) / 100.0;
    double mean = zoom / 2.0;
    double sigma = zoom / 2.0;

    std::vector<std::vector<double>> weights_by_half(kMaxHalf + 1);

    Image result(width, height);
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            double dx = static_cast<double>(x) - center_x;
            double dy = static_cast<double>(y) - center_y;
            double sigma_pixels = sigma * std::hypot(dx, dy);
            if (sigma_pixels < kTinyWindow) {
                result.At(x, y) = image.At(x, y);
                continue;
            }

            size_t half = static_cast<size_t>(std::max(static_cast<double>(kMinHalf), std::min(std::ceil(kWindowSigmas * sigma_pixels), static_cast<double>(kMaxHalf))));
            std::vector<double> weights = weights_by_half[half];
            if (weights.empty()) {
                weights = MakeWeights(half);
            }

            double sum_r = 0;
            double sum_g = 0;
            double sum_b = 0;
            for (size_t i = 0; i < weights.size(); ++i) {
                double g = kWindowSigmas * (static_cast<double>(i) - static_cast<double>(half)) / static_cast<double>(half);
                double z = 1.0 - mean - sigma * g;
                Color s = Sample(image, center_x + dx * z, center_y + dy * z);
                sum_r += weights[i] * s.r;
                sum_g += weights[i] * s.g;
                sum_b += weights[i] * s.b;
            }
            result.At(x, y) = Color(static_cast<float>(sum_r), static_cast<float>(sum_g), static_cast<float>(sum_b));
        }
    }

    image = std::move(result);
}

}