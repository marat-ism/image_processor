#pragma once

#include <array>

#include "../filter.h"



namespace ip {

using Matrix = std::array<std::array<float, 3>, 3>;


class MatrixFilter : virtual public Filter {
public:

    MatrixFilter(const Matrix& matrix) : matrix_(matrix) {}

    void Apply(Image& image) const override;

    float& At(size_t x, size_t y);

    const float& At(size_t x, size_t y) const;

private:
    Matrix matrix_;
};

}