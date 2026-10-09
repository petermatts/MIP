/**
 * @file image.tpp
 * @brief Template implementations for the mip image container.
 */

#pragma once

#include <limits>
#include <stdexcept>
#include <vector>

namespace mip {
namespace detail {

/**
 * @brief Computes the number of channel elements required for an image.
 *
 * Validates the dimensions, channel count, and storage size before
 * allocating the image buffer.
 *
 * @tparam PixelT Channel storage type.
 * @param width Image width in pixels.
 * @param height Image height in pixels.
 * @param channels Number of channels per pixel.
 *
 * @return Number of channel elements required.
 *
 * @throws std::invalid_argument If dimensions or channel count are invalid.
 * @throws std::length_error If the requested allocation size is too large.
 */
template <typename PixelT>
[[nodiscard]] std::size_t checked_element_count(
    std::size_t width,
    std::size_t height,
    std::size_t channels
) {
    if (width == 0 || height == 0) {
        throw std::invalid_argument(
            "Image width and height must be greater than zero."
        );
    }

    if (channels != 1 && channels != 3 && channels != 4) {
        throw std::invalid_argument(
            "Image channel count must be 1, 3, or 4."
        );
    }

    constexpr std::size_t max_size =
        std::numeric_limits<std::size_t>::max();

    // Check width * height for overflow.
    if (width > max_size / height) {
        throw std::length_error("Image dimensions are too large.");
    }

    const std::size_t pixels = width * height;

    // Check pixels * channels for overflow.
    if (pixels > max_size / channels) {
        throw std::length_error("Image element count is too large.");
    }

    const std::size_t elements = pixels * channels;

    // Also check against the underlying vector's maximum element count.
    if (elements > std::vector<PixelT>{}.max_size()) {
        throw std::length_error(
            "Image exceeds the maximum supported storage size."
        );
    }

    return elements;
}

} // namespace detail

template <typename PixelT>
Image<PixelT>::Image(
    std::size_t width,
    std::size_t height,
    std::size_t channels
)
    : width_(width),
      height_(height),
      channels_(channels),
      data_(
          detail::checked_element_count<PixelT>(
              width,
              height,
              channels
          ),
          PixelT{}
      ) {
}

template <typename PixelT>
std::size_t Image<PixelT>::width() const noexcept {
    return width_;
}

template <typename PixelT>
std::size_t Image<PixelT>::height() const noexcept {
    return height_;
}

template <typename PixelT>
std::size_t Image<PixelT>::channels() const noexcept {
    return channels_;
}

template <typename PixelT>
std::size_t Image<PixelT>::num_pixels() const noexcept {
    return width_ * height_;
}

template <typename PixelT>
std::size_t Image<PixelT>::num_elements() const noexcept {
    return data_.size();
}

template <typename PixelT>
std::size_t Image<PixelT>::size_bytes() const noexcept {
    return data_.size() * sizeof(PixelT);
}

template <typename PixelT>
std::size_t Image<PixelT>::row_stride() const noexcept {
    return width_ * channels_ * sizeof(PixelT);
}

template <typename PixelT>
PixelT* Image<PixelT>::data() noexcept {
    return data_.empty() ? nullptr : data_.data();
}

template <typename PixelT>
const PixelT* Image<PixelT>::data() const noexcept {
    return data_.empty() ? nullptr : data_.data();
}

template <typename PixelT>
PixelT& Image<PixelT>::at(
    std::size_t x,
    std::size_t y,
    std::size_t channel
) {
    if (x >= width_ || y >= height_ || channel >= channels_) {
        throw std::out_of_range("Image channel coordinates are out of range.");
    }

    return data_[offset(x, y, channel)];
}

template <typename PixelT>
const PixelT& Image<PixelT>::at(
    std::size_t x,
    std::size_t y,
    std::size_t channel
) const {
    if (x >= width_ || y >= height_ || channel >= channels_) {
        throw std::out_of_range("Image channel coordinates are out of range.");
    }

    return data_[offset(x, y, channel)];
}

template <typename PixelT>
PixelT* Image<PixelT>::pixel(
    std::size_t x,
    std::size_t y
) {
    if (x >= width_ || y >= height_) {
        throw std::out_of_range("Image pixel coordinates are out of range.");
    }

    return data_.data() + offset(x, y, 0);
}

template <typename PixelT>
const PixelT* Image<PixelT>::pixel(
    std::size_t x,
    std::size_t y
) const {
    if (x >= width_ || y >= height_) {
        throw std::out_of_range("Image pixel coordinates are out of range.");
    }

    return data_.data() + offset(x, y, 0);
}

template <typename PixelT>
bool Image<PixelT>::empty() const noexcept {
    return data_.empty();
}

template <typename PixelT>
void Image<PixelT>::clear() noexcept {
    data_.clear();

    width_ = 0;
    height_ = 0;
    channels_ = 0;
}

template <typename PixelT>
std::size_t Image<PixelT>::offset(
    std::size_t x,
    std::size_t y,
    std::size_t channel
) const noexcept {
    return ((y * width_) + x) * channels_ + channel;
}

} // namespace mip