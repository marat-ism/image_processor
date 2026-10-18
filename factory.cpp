#include "factory.h"

#include <vector>
#include <map>

#include "all_filters/crop_filter.h"
#include "all_filters/negative_filter.h"
#include "all_filters/grayscale_filter.h"
#include "all_filters/sharpening_filter.h"
#include "all_filters/edge_filter.h"
#include "all_filters/gaussian_filter.h"
#include "all_filters/cizoomblur_filter.h"


namespace ip::factory {

namespace {

using Params = std::vector<std::string_view>;
using Creator = std::unique_ptr<Filter> (*)(const Params& params);

float ParsePositiveSize(std::string_view text) {
    if (text.empty()) {
        throw InvalidParameterError();
    }
    size_t answer = 0.0f;
    auto [result, ec] = std::from_chars(text.data(), text.data() + text.size(), answer);
    if (ec != std::errc() || result != text.data() + text.size() || answer == 0) {
        throw InvalidParameterError();
    }
    return answer;
}

float ParseFiniteFloat(std::string_view text) {
    if (text.empty()) {
        throw InvalidParameterError();
    }
    float answer = 0.0f;
    auto [result, ec] = std::from_chars(text.data(), text.data() + text.size(), answer);
    if (ec != std::errc() || result != text.data() + text.size() || !std::isfinite(answer)) {
        throw InvalidParameterError();
    }
    return answer;
}

template <typename T>
std::unique_ptr<Filter> Make(const Params& params) {
    if constexpr (std::is_same_v<T, CropFilter>) {
        if (params.size() != 2u) {
            throw InvalidParameterError();
        }
        size_t width = ParsePositiveSize(params[0]);
        size_t height = ParsePositiveSize(params[1]);
        return std::make_unique<CropFilter>(width, height);
    } else if constexpr (std::is_same_v<T, EdgeFilter>) {
        if (params.size() != 1u) {
            throw InvalidParameterError();
        }
        float threshold = ParseFiniteFloat(params[0]);
        if (threshold < 0.0f || threshold > 1.0f) {
            throw InvalidParameterError();
        }
        return std::make_unique<EdgeFilter>(threshold);
    } else if constexpr (std::is_same_v<T, GaussianFilter>) {
        if (params.size() != 1u) {
            throw InvalidParameterError();
        }
        float sigma = ParseFiniteFloat(params[0]);
        if (sigma <= 0.0f) {
            throw InvalidParameterError();
        }
        return std::make_unique<GaussianFilter>(sigma);
    } else if constexpr (std::is_same_v<T, CIZoomBlurFilter>) {
        if (params.empty()) {
            return std::make_unique<CIZoomBlurFilter>();
        }
        if (params.size() != 1u && params.size() != 3u) {
            throw InvalidParameterError();
        }

        float amount = ParseFiniteFloat(params[0]);
        if (amount < 0.0f) {
            throw InvalidParameterError();
        }
        if (params.size() == 1u) {
            return std::make_unique<CIZoomBlurFilter>(amount);
        }

        float center_x = ParseFiniteFloat(params[1]);
        float center_y = ParseFiniteFloat(params[2]);
        return std::make_unique<CIZoomBlurFilter>(amount, center_x, center_y);
    } else {
        if (!params.empty()) {
            throw InvalidParameterError();
        }
        return std::make_unique<T>();
    }
}

const std::map<std::string, Creator> kCreators = {
    {"crop", Make<CropFilter>},
    {"neg", Make<NegativeFilter>},
    {"gs", Make<GrayscaleFilter>},
    {"sharp", Make<SharpeningFilter>},
    {"edge", Make<EdgeFilter>},
    {"blur", Make<GaussianFilter>},
    {"cizoomblur", Make<CIZoomBlurFilter>}
};

}

std::unique_ptr<Filter> CreateFilter(const std::string_view& type, const Params& params) {
    const auto it = kCreators.find(std::string(type));
    if (it == kCreators.end()) {
        throw UnknownFilterError(std::string(type));
    }
    return it->second(params);
}

std::vector<std::string> KnownFilters() {
    std::vector<std::string> types;
    for (const auto& [type, creator] : kCreators) {
        types.push_back(type);
    }
    return types;
}

}