#include "grayscale_filter.h"



namespace ip {

Color GrayscaleFilter::Transform(const Color& color) const {
    float gray = 0.299f * color.r + 0.587f * color.g + 0.114f * color.b;
    return Color(gray, gray, gray);
}

}