

#include "bmp.h"
#include "errors.h"

namespace ip::bmp {

namespace {
    constexpr uint16_t kSignature = 0x4D42; // "BM"
}

Image Read(std::string_view path) {
    std::ifstream in((std::string(path)), std::ios::binary);
    if (!in) {
        throw BMPError("Failed to open file");
    }
    if (ReadTo<uint16_t>(in) != kSignature) {
        throw BMPError("Invalid BMP signature");
    }
    ReadTo<uint32_t>(in); // The size of the BMP file in bytes
    ReadTo<uint32_t>(in); // Reserved; must be zero
    uint32_t bfOffBits = ReadTo<uint32_t>(in); // The offset
    if (bfOffBits < 54u) {
        throw BMPError("Invalid BMP pixel-data offset");
    }
    uint32_t biIsize = ReadTo<uint32_t>(in); // The size of this header (must be 40 bytes)
    if (biIsize != 40u) {
        throw BMPError("Unsupported BMP header size, must be 40 bytes");
    }
    int32_t biWidth = ReadTo<int32_t>(in); // The bitmap width in pixels
    if (biWidth <= 0) {
        throw BMPError("Invalid BMP width");
    }
    int32_t biHeight = ReadTo<int32_t>(in); // The bitmap height in pixels
    if (biHeight == 0 || biHeight == INT32_MIN) {
        throw BMPError("Invalid BMP height");
    }
    uint16_t biPlanes = ReadTo<uint16_t>(in); // The number of color planes
    if (biPlanes != 1u) {
        throw BMPError("Invalid BMP color planes, must be 1");
    }
    uint16_t biBitCount = ReadTo<uint16_t>(in); // The number of bits per pixel
    if (biBitCount != 24u) {
        throw BMPError("Unsupported BMP bit count, must be 24");
    }
    uint32_t biCompression = ReadTo<uint32_t>(in); // The compression method, must be 0
    if (biCompression != 0u) {
        throw BMPError("Unsupported BMP compression, must be 0 (BI_RGB)");
    }
    ReadTo<uint32_t>(in); // The image size. This is the size of the raw bitmap data
    ReadTo<uint32_t>(in); // pixel per meter, signed integer
    ReadTo<uint32_t>(in); // pixel per meter, signed integer
    ReadTo<uint32_t>(in); // The number of colors in the color palette
    ReadTo<uint32_t>(in); // The number of important colors used
    if (bfOffBits != 54u) {
        throw BMPError("Invalid BMP pixel-data offset");
    }
    for (uint32_t i = 54; i < bfOffBits; ++i) {
        ReadTo<uint8_t>(in); // Skip any extra header bytes
    }
    size_t width = static_cast<size_t>(biWidth);
    size_t height = static_cast<size_t>(biHeight > 0 ? biHeight : -biHeight);
    Image image(width, height);
    size_t row_size = (width * 3 + 3) / 4 * 4; // Each row is padded to a multiple of 4 bytes
    size_t padding = row_size - width * 3;
    for (size_t file_y = 0; file_y < height; ++file_y) {
        size_t y = biHeight > 0 ? height - 1 - file_y : file_y; // BMP stores pixels bottom-to-top if height is positive
        for (size_t x = 0; x < width; ++x) {
            uint8_t b = ReadTo<uint8_t>(in);
            uint8_t g = ReadTo<uint8_t>(in);
            uint8_t r = ReadTo<uint8_t>(in);
            image.At(x, y) = Color(static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f, static_cast<float>(b) / 255.0f);
        }
        for (size_t p = 0; p < padding; ++p) {
            ReadTo<uint8_t>(in); // Skip padding bytes
        }
    }
    return image;
}

void Write(const Image& image, std::string_view path) {
    std::ofstream out((std::string(path)), std::ios::binary | std::ios::trunc);
    if (!out) {
        throw BMPError("Failed to open file for writing");
    }
    size_t width = image.Width();
    size_t height = image.Height();
    if (width == 0 || height == 0) {
        throw BMPError("Cannot write an empty image");
    }
    if (width > INT32_MAX || height > INT32_MAX) {
        throw BMPError("Image dimensions are too large to write as BMP");
    }
    size_t row_size = (width * 3 + 3) / 4 * 4; // Each row is padded to a multiple of 4 bytes
    if (row_size > (UINT32_MAX - 54) / height || height > (UINT32_MAX - 54) / row_size) {
        throw BMPError("Bmp file is too large");
    }
    WriteTo<uint16_t>(out, kSignature); // "BM" signature
    WriteTo<uint32_t>(out, static_cast<uint32_t>(54) + row_size * height); // Placeholder for file size
    WriteTo<uint32_t>(out, 0); // Reserved
    WriteTo<uint32_t>(out, 54); // Offset to pixel data
    WriteTo<uint32_t>(out, 40); // DIB header size
    WriteTo<int32_t>(out, static_cast<int32_t>(width)); // Width
    WriteTo<int32_t>(out, static_cast<int32_t>(height)); // Height
    WriteTo<uint16_t>(out, 1); // Color planes
    WriteTo<uint16_t>(out, 24); // Bits per pixel
    WriteTo<uint32_t>(out, 0); // Compression (none)
    WriteTo<uint32_t>(out, 0);
    WriteTo<uint32_t>(out, 0);
    WriteTo<uint32_t>(out, 0);
    WriteTo<uint32_t>(out, 0);
    WriteTo<uint32_t>(out, 0);
    size_t padding = row_size - width * 3;
    for (size_t file_y = 0; file_y < height; ++file_y) {
        size_t y = height - 1 - file_y;
        for (size_t x = 0; x < width; ++x) {
            WriteTo<uint8_t>(out, static_cast<uint8_t>(image.At(x, y).b * 255.0f));
            WriteTo<uint8_t>(out, static_cast<uint8_t>(image.At(x, y).g * 255.0f));
            WriteTo<uint8_t>(out, static_cast<uint8_t>(image.At(x, y).r * 255.0f));
        }
        for (size_t p = 0; p < padding; ++p) {
            WriteTo<uint8_t>(out, 0); // Write padding bytes
        }
    }
    return;
}

}