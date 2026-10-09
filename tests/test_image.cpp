/**
 * @file test_image.cpp
 * @brief Unit tests for the mip image container.
 */

#include "mip/image.hpp"

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>

namespace {

using SupportedTypes = ::testing::Types<
    std::uint8_t,
    std::uint16_t,
    std::uint32_t,
    float,
    double
>;

template <typename T>
class ImageTest : public ::testing::Test {};

TYPED_TEST_SUITE(ImageTest, SupportedTypes);

// --------------------------------------------------
// Construction and metadata
// --------------------------------------------------

TYPED_TEST(ImageTest, DefaultConstructsEmptyImage) {
    using PixelT = TypeParam;

    const mip::Image<PixelT> image;

    EXPECT_TRUE(image.empty());
    EXPECT_EQ(image.width(), 0U);
    EXPECT_EQ(image.height(), 0U);
    EXPECT_EQ(image.channels(), 0U);
    EXPECT_EQ(image.num_pixels(), 0U);
    EXPECT_EQ(image.num_elements(), 0U);
    EXPECT_EQ(image.size_bytes(), 0U);
    EXPECT_EQ(image.row_stride(), 0U);
}

TYPED_TEST(ImageTest, ConstructsImageWithExpectedDimensions) {
    using PixelT = TypeParam;

    const mip::Image<PixelT> image(640, 480, 3);

    EXPECT_FALSE(image.empty());
    EXPECT_EQ(image.width(), 640U);
    EXPECT_EQ(image.height(), 480U);
    EXPECT_EQ(image.channels(), 3U);

    EXPECT_EQ(image.num_pixels(), 640U * 480U);
    EXPECT_EQ(image.num_elements(), 640U * 480U * 3U);
    EXPECT_EQ(
        image.size_bytes(),
        640U * 480U * 3U * sizeof(PixelT)
    );
    EXPECT_EQ(
        image.row_stride(),
        640U * 3U * sizeof(PixelT)
    );
}

TYPED_TEST(ImageTest, SupportsSingleChannelImages) {
    using PixelT = TypeParam;

    const mip::Image<PixelT> image(100, 50, 1);

    EXPECT_EQ(image.channels(), 1U);
    EXPECT_EQ(image.num_elements(), 100U * 50U);
}

TYPED_TEST(ImageTest, SupportsFourChannelImages) {
    using PixelT = TypeParam;

    const mip::Image<PixelT> image(100, 50, 4);

    EXPECT_EQ(image.channels(), 4U);
    EXPECT_EQ(image.num_elements(), 100U * 50U * 4U);
}

// --------------------------------------------------
// Initialization and memory layout
// --------------------------------------------------

TYPED_TEST(ImageTest, InitializesAllChannelsToZero) {
    using PixelT = TypeParam;

    const mip::Image<PixelT> image(8, 6, 3);

    for (std::size_t i = 0; i < image.num_elements(); ++i) {
        EXPECT_EQ(image.data()[i], PixelT{});
    }
}

TYPED_TEST(ImageTest, StoresChannelsContiguously) {
    using PixelT = TypeParam;

    mip::Image<PixelT> image(2, 1, 3);

    image.at(0, 0, 0) = static_cast<PixelT>(1);
    image.at(0, 0, 1) = static_cast<PixelT>(2);
    image.at(0, 0, 2) = static_cast<PixelT>(3);

    image.at(1, 0, 0) = static_cast<PixelT>(4);
    image.at(1, 0, 1) = static_cast<PixelT>(5);
    image.at(1, 0, 2) = static_cast<PixelT>(6);

    const PixelT expected[] = {
        static_cast<PixelT>(1),
        static_cast<PixelT>(2),
        static_cast<PixelT>(3),
        static_cast<PixelT>(4),
        static_cast<PixelT>(5),
        static_cast<PixelT>(6)
    };

    for (std::size_t i = 0; i < 6; ++i) {
        EXPECT_EQ(image.data()[i], expected[i]);
    }
}

// --------------------------------------------------
// Pixel and channel access
// --------------------------------------------------

TYPED_TEST(ImageTest, AtReadsAndWritesChannelValues) {
    using PixelT = TypeParam;

    mip::Image<PixelT> image(4, 3, 3);

    image.at(2, 1, 0) = static_cast<PixelT>(10);
    image.at(2, 1, 1) = static_cast<PixelT>(20);
    image.at(2, 1, 2) = static_cast<PixelT>(30);

    EXPECT_EQ(image.at(2, 1, 0), static_cast<PixelT>(10));
    EXPECT_EQ(image.at(2, 1, 1), static_cast<PixelT>(20));
    EXPECT_EQ(image.at(2, 1, 2), static_cast<PixelT>(30));
}

TYPED_TEST(ImageTest, PixelReturnsPointerToFirstChannel) {
    using PixelT = TypeParam;

    mip::Image<PixelT> image(3, 2, 3);

    image.at(1, 1, 0) = static_cast<PixelT>(11);
    image.at(1, 1, 1) = static_cast<PixelT>(22);
    image.at(1, 1, 2) = static_cast<PixelT>(33);

    PixelT* pixel = image.pixel(1, 1);

    ASSERT_NE(pixel, nullptr);
    EXPECT_EQ(pixel[0], static_cast<PixelT>(11));
    EXPECT_EQ(pixel[1], static_cast<PixelT>(22));
    EXPECT_EQ(pixel[2], static_cast<PixelT>(33));
}

TYPED_TEST(ImageTest, PixelWritesModifyUnderlyingStorage) {
    using PixelT = TypeParam;

    mip::Image<PixelT> image(2, 2, 3);

    PixelT* pixel = image.pixel(1, 0);

    pixel[0] = static_cast<PixelT>(7);
    pixel[1] = static_cast<PixelT>(8);
    pixel[2] = static_cast<PixelT>(9);

    EXPECT_EQ(image.at(1, 0, 0), static_cast<PixelT>(7));
    EXPECT_EQ(image.at(1, 0, 1), static_cast<PixelT>(8));
    EXPECT_EQ(image.at(1, 0, 2), static_cast<PixelT>(9));
}

// --------------------------------------------------
// Bounds checking
// --------------------------------------------------

TYPED_TEST(ImageTest, AtRejectsOutOfRangeCoordinates) {
    using PixelT = TypeParam;

    mip::Image<PixelT> image(4, 3, 3);

    EXPECT_THROW(static_cast<void>(image.at(4, 0, 0)), std::out_of_range);
    EXPECT_THROW(static_cast<void>(image.at(0, 3, 0)), std::out_of_range);
    EXPECT_THROW(static_cast<void>(image.at(0, 0, 3)), std::out_of_range);
}

TYPED_TEST(ImageTest, PixelRejectsOutOfRangeCoordinates) {
    using PixelT = TypeParam;

    mip::Image<PixelT> image(4, 3, 3);

    EXPECT_THROW(static_cast<void>(image.pixel(4, 0)), std::out_of_range);
    EXPECT_THROW(static_cast<void>(image.pixel(0, 3)), std::out_of_range);
}

// --------------------------------------------------
// Invalid construction
// --------------------------------------------------

TYPED_TEST(ImageTest, RejectsZeroWidth) {
    using PixelT = TypeParam;

    EXPECT_THROW(
        (mip::Image<PixelT>(0, 10, 3)),
        std::invalid_argument
    );
}

TYPED_TEST(ImageTest, RejectsZeroHeight) {
    using PixelT = TypeParam;

    EXPECT_THROW(
        (mip::Image<PixelT>(10, 0, 3)),
        std::invalid_argument
    );
}

TYPED_TEST(ImageTest, RejectsUnsupportedChannelCount) {
    using PixelT = TypeParam;

    EXPECT_THROW(
        (mip::Image<PixelT>(10, 10, 2)),
        std::invalid_argument
    );

    EXPECT_THROW(
        (mip::Image<PixelT>(10, 10, 5)),
        std::invalid_argument
    );
}

TYPED_TEST(ImageTest, RejectsDimensionsThatOverflow) {
    using PixelT = TypeParam;

    constexpr std::size_t max =
        std::numeric_limits<std::size_t>::max();

    EXPECT_THROW(
        (mip::Image<PixelT>(max, 2, 3)),
        std::length_error
    );
}

// --------------------------------------------------
// Clearing and resetting
// --------------------------------------------------

TYPED_TEST(ImageTest, ClearResetsDimensionsAndStorage) {
    using PixelT = TypeParam;

    mip::Image<PixelT> image(10, 20, 3);

    image.clear();

    EXPECT_TRUE(image.empty());
    EXPECT_EQ(image.width(), 0U);
    EXPECT_EQ(image.height(), 0U);
    EXPECT_EQ(image.channels(), 0U);
    EXPECT_EQ(image.num_pixels(), 0U);
    EXPECT_EQ(image.num_elements(), 0U);
    EXPECT_EQ(image.size_bytes(), 0U);
    EXPECT_EQ(image.row_stride(), 0U);
}

TYPED_TEST(ImageTest, CanBeReusedAfterClear) {
    using PixelT = TypeParam;

    mip::Image<PixelT> image(10, 20, 3);

    image.clear();

    // Assign a new image to the cleared object.
    image = mip::Image<PixelT>(5, 4, 1);

    EXPECT_FALSE(image.empty());
    EXPECT_EQ(image.width(), 5U);
    EXPECT_EQ(image.height(), 4U);
    EXPECT_EQ(image.channels(), 1U);
    EXPECT_EQ(image.num_elements(), 20U);
}

TYPED_TEST(ImageTest, EmptyImageReturnsNullDataPointer) {
    using PixelT = TypeParam;

    const mip::Image<PixelT> image;

    EXPECT_EQ(image.data(), nullptr);
}

} // namespace