#include "pixel_filter.h"

namespace ip {

void PixelFilter::Apply(Image& image) const {
    for (Color& color : image.Pixels()) {
        color = Transform(color);
    }
}

}