/**
 * @file image.hpp
 * @brief Defines the core image container for the mip library.
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <vector>

namespace mip {

/**
 * @brief A two-dimensional image with a fixed number of channels per pixel.
 *
 * @tparam PixelT The scalar type used to store each channel.
 *
 * Supported storage types are:
 * - std::uint8_t  (8-bit unsigned integer)
 * - std::uint16_t (16-bit unsigned integer)
 * - std::uint32_t (32-bit unsigned integer)
 * - float         (32-bit floating point, when float is 32 bits)
 * - double        (typically 64-bit floating point)
 *
 * Pixels are stored in row-major order, with channels interleaved.
 * For example, an RGB image stores the channels in the order:
 *
 * R0, G0, B0, R1, G1, B1, ...
 *
 * The image owns its pixel storage and does not depend on any external
 * image library or GPU runtime.
 *
 * Supported channel counts:
 * - 1: single-channel image
 * - 3: three-channel image, conventionally RGB
 * - 4: four-channel image, conventionally RGBA
 *
 * The container does not impose a numeric range or normalization policy
 * on pixel values. Those semantics are determined by the application
 * and the image-processing operations using the container.
 *
 * @tparam PixelT must be one of the supported storage types listed above.
 */
template <typename PixelT>
class Image {
    static_assert(
        std::is_same_v<PixelT, std::uint8_t>  ||
        std::is_same_v<PixelT, std::uint16_t> ||
        std::is_same_v<PixelT, std::uint32_t> ||
        std::is_same_v<PixelT, float>         ||
        std::is_same_v<PixelT, double>,
        "Image<PixelT>: PixelT must be uint8_t, uint16_t, uint32_t, "
        "float, or double."
    );

public:
    /**
     * @brief Constructs an empty image.
     *
     * The resulting image has zero width, height, and channels, and
     * contains no pixel data.
     */
    Image() noexcept = default;

    /**
     * @brief Constructs an image with the specified dimensions.
     *
     * Pixel storage is initialized to zero.
     *
     * @param width Image width in pixels.
     * @param height Image height in pixels.
     * @param channels Number of channels per pixel; must be 1, 3, or 4.
     *
     * @throws std::invalid_argument If width or height is zero, or if
     *         channels is not 1, 3, or 4.
     * @throws std::length_error If the requested image size cannot be
     *         represented or exceeds the storage container's maximum size.
     * @throws std::bad_alloc If memory allocation fails.
     */
    Image(std::size_t width,
          std::size_t height,
          std::size_t channels = 3);

    /**
     * @brief Returns the image width.
     *
     * @return Width in pixels.
     */
    [[nodiscard]] std::size_t width() const noexcept;

    /**
     * @brief Returns the image height.
     *
     * @return Height in pixels.
     */
    [[nodiscard]] std::size_t height() const noexcept;

    /**
     * @brief Returns the number of channels per pixel.
     *
     * @return Channel count, typically 1, 3, or 4.
     */
    [[nodiscard]] std::size_t channels() const noexcept;

    /**
     * @brief Returns the total number of pixels.
     *
     * This excludes the number of channels.
     *
     * @return Width multiplied by height.
     */
    [[nodiscard]] std::size_t num_pixels() const noexcept;

    /**
     * @brief Returns the total number of stored channel elements.
     *
     * This is the number of elements in the underlying pixel buffer,
     * rather than the number of bytes.
     *
     * @return Width multiplied by height multiplied by channels.
     */
    [[nodiscard]] std::size_t num_elements() const noexcept;

    /**
     * @brief Returns the image storage size in bytes.
     *
     * @return Number of stored channel elements multiplied by sizeof(PixelT).
     */
    [[nodiscard]] std::size_t size_bytes() const noexcept;

    /**
     * @brief Returns the distance between consecutive rows in bytes.
     *
     * The image uses tightly packed rows, without row padding.
     *
     * @return Number of bytes in one row.
     */
    [[nodiscard]] std::size_t row_stride() const noexcept;

    /**
     * @brief Returns a pointer to the underlying pixel buffer.
     *
     * The buffer is contiguous and uses row-major, interleaved-channel
     * storage. The returned pointer is invalidated when the image's
     * storage is destroyed or otherwise reallocated.
     *
     * @return Pointer to the first channel element, or nullptr if empty.
     */
    [[nodiscard]] PixelT* data() noexcept;

    /**
     * @brief Returns a read-only pointer to the underlying pixel buffer.
     *
     * @return Pointer to the first channel element, or nullptr if empty.
     */
    [[nodiscard]] const PixelT* data() const noexcept;

    /**
     * @brief Returns a reference to a channel value.
     *
     * @param x Horizontal pixel coordinate, starting at zero.
     * @param y Vertical pixel coordinate, starting at zero.
     * @param channel Channel index, starting at zero.
     *
     * @return Reference to the requested channel.
     *
     * @throws std::out_of_range If any coordinate or channel index is
     *         outside the image bounds.
     */
    [[nodiscard]] PixelT& at(
        std::size_t x,
        std::size_t y,
        std::size_t channel
    );

    /**
     * @brief Returns a read-only reference to a channel value.
     *
     * @param x Horizontal pixel coordinate, starting at zero.
     * @param y Vertical pixel coordinate, starting at zero.
     * @param channel Channel index, starting at zero.
     *
     * @return Read-only reference to the requested channel.
     *
     * @throws std::out_of_range If any coordinate or channel index is
     *         outside the image bounds.
     */
    [[nodiscard]] const PixelT& at(
        std::size_t x,
        std::size_t y,
        std::size_t channel
    ) const;

    /**
     * @brief Returns a pointer to the first channel of a pixel.
     *
     * The channels for the pixel are contiguous. For an RGB image,
     * pixel(x, y)[0], pixel(x, y)[1], and pixel(x, y)[2] refer to its
     * red, green, and blue channel values, respectively.
     *
     * @param x Horizontal pixel coordinate, starting at zero.
     * @param y Vertical pixel coordinate, starting at zero.
     *
     * @return Pointer to the first channel of the requested pixel.
     *
     * @throws std::out_of_range If the pixel coordinates are outside
     *         the image bounds.
     */
    [[nodiscard]] PixelT* pixel(
        std::size_t x,
        std::size_t y
    );

    /**
     * @brief Returns a read-only pointer to the first channel of a pixel.
     *
     * @param x Horizontal pixel coordinate, starting at zero.
     * @param y Vertical pixel coordinate, starting at zero.
     *
     * @return Read-only pointer to the first channel of the requested pixel.
     *
     * @throws std::out_of_range If the pixel coordinates are outside
     *         the image bounds.
     */
    [[nodiscard]] const PixelT* pixel(
        std::size_t x,
        std::size_t y
    ) const;

    /**
     * @brief Indicates whether the image contains pixel data.
     *
     * @return True if the image has no pixel elements; otherwise false.
     */
    [[nodiscard]] bool empty() const noexcept;

    /**
     * @brief Removes the image's pixel elements and resets its dimensions.
     *
     * After this operation, width, height, channels, and the number of
     * elements are zero. The vector's allocated capacity is not
     * necessarily released.
     */
    void clear() noexcept;

private:
    /**
     * @brief Computes the linear storage index for a channel.
     *
     * @param x Horizontal pixel coordinate.
     * @param y Vertical pixel coordinate.
     * @param channel Channel index.
     * @return Index into the underlying channel buffer.
     */
    [[nodiscard]] std::size_t offset(
        std::size_t x,
        std::size_t y,
        std::size_t channel
    ) const noexcept;

    std::size_t width_ = 0;
    std::size_t height_ = 0;
    std::size_t channels_ = 0;

    std::vector<PixelT> data_;
};

/**
 * @brief Explicitly instantiated image types supported by the mip library.
 *
 * These declarations indicate that the corresponding template
 * instantiations are provided by the compiled library.
 */
extern template class Image<std::uint8_t>;
extern template class Image<std::uint16_t>;
extern template class Image<std::uint32_t>;
extern template class Image<float>;
extern template class Image<double>;

} // namespace mip

#include "mip/image.tpp"