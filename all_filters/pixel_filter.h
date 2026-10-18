#pragma once



#include "../filter.h"

namespace ip {

class PixelFilter : virtual public Filter {
public:

    void Apply(Image& image) const override;

    virtual Color Transform(const Color& color) const = 0;
};

}