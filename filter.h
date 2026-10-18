#pragma once

#include <string_view>
#include <vector>
#include <array>

#include "image.h"

namespace ip {

class Filter {
public:
    virtual ~Filter() = default;
    virtual void Apply(Image& image) const = 0;
    virtual std::string_view Name() const noexcept = 0;   
};

}