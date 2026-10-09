#include "mip/image.hpp"

#include <cstdint>
#include <iostream>

int main() {
    // Create a 1920x1080 RGB image with 8-bit channels.
    mip::Image<std::uint8_t> image8(1920, 1080, 3);

    // Create a 16-bit, single-channel image.
    mip::Image<std::uint16_t> image16(1024, 1024, 1);

    // Create a floating-point RGBA image.
    mip::Image<float> image_float(512, 512, 4);

    // Set the RGB channels of pixel (10, 20).
    image8.at(10, 20, 0) = 255;
    image8.at(10, 20, 1) = 128;
    image8.at(10, 20, 2) = 64;

    // Alternatively, access the contiguous channels of a pixel.
    auto* pixel = image8.pixel(10, 20);

    std::cout << "R: " << static_cast<int>(pixel[0]) << '\n';
    std::cout << "G: " << static_cast<int>(pixel[1]) << '\n';
    std::cout << "B: " << static_cast<int>(pixel[2]) << '\n';

    std::cout << "Image width: " << image8.width() << '\n';
    std::cout << "Image height: " << image8.height() << '\n';
    std::cout << "Channels: " << image8.channels() << '\n';
    std::cout << "Row stride (bytes): " << image8.row_stride() << '\n';
    std::cout << "Storage size (bytes): " << image8.size_bytes() << '\n';

    // Floating-point images can store values in the range your algorithms
    // define; the container does not enforce a particular normalization.
    image_float.at(0, 0, 0) = 1.0f;
    image_float.at(0, 0, 3) = 1.0f;

    return 0;
}