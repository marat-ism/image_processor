#pragma once

#include <atomic>
#include <cstddef>
#include <stdexcept>
#include <vector>
#include <fstream>
#include <string>
#include <string_view>

#include "image.h"

namespace ip::bmp {

Image Read(std::string_view path);

void Write(const Image& image, std::string_view path);

template <typename T>
T ReadTo(std::istream& in) {
    unsigned char bytes[sizeof(T)];
    if (!in.read(reinterpret_cast<char*>(bytes), sizeof(T))) {
        throw BMPError("Failed to read from file");
    }
    uint64_t value = 0;
    for (size_t i = 0; i < sizeof(T); ++i) {
        value |= static_cast<uint64_t>(bytes[i]) << (8 * i);
    }
    return static_cast<T>(value);
}

template <typename T>
void WriteTo(std::ostream& out, T value) {
    unsigned char bytes[sizeof(T)];
    for (size_t i = 0; i < sizeof(T); ++i) {
        bytes[i] = (value >> (8 * i)) & 0xFF;
    }
    if (!out.write(reinterpret_cast<char*>(bytes), sizeof(T))) {
        throw BMPError("Failed to write to file");
    }
    return;
}

}