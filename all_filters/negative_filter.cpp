#include "negative_filter.h"



namespace ip {

Color NegativeFilter::Transform(const Color& color) const {
    return Color(1.0f - color.r, 1.0f - color.g, 1.0f - color.b);
}

}