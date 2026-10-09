/**
 * @file benchmark_image.cpp
 * @brief Baseline performance benchmarks for the mip image container.
 *
 * Measures:
 * - Image construction and zero-initialization.
 * - Sequential traversal through the raw data buffer.
 * - Checked channel access through Image::at().
 * - Pixel access through Image::pixel().
 *
 * Build in Release mode for meaningful performance measurements.
 */

#include "mip/image.hpp"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

namespace {

using Clock = std::chrono::steady_clock;

// Prevent the compiler from discarding benchmark results.
volatile double benchmark_sink = 0.0;

/**
 * @brief Runs a benchmark repeatedly and reports timing statistics.
 *
 * @tparam Operation Callable returning a numeric result.
 * @param name Name of the benchmark.
 * @param repetitions Number of measured iterations.
 * @param operation Callable performing the measured work.
 * @param pixels Number of pixels processed per iteration.
 * @param bytes Number of bytes read or initialized per iteration.
 * @param report_throughput Whether to report estimated throughput.
 */
template <typename Operation>
void run_benchmark(
    const std::string& name,
    std::size_t repetitions,
    Operation operation,
    std::size_t pixels = 0,
    std::size_t bytes = 0,
    bool report_throughput = true
) {
    using Duration = std::chrono::duration<double, std::nano>;

    // Warm up the code and allocator before collecting measurements.
    benchmark_sink = operation();

    std::vector<double> timings_ns;
    timings_ns.reserve(repetitions);

    for (std::size_t i = 0; i < repetitions; ++i) {
        const auto start = Clock::now();

        benchmark_sink = operation();

        const auto end = Clock::now();

        const auto elapsed =
            std::chrono::duration_cast<Duration>(end - start);

        timings_ns.push_back(elapsed.count());
    }

    std::sort(timings_ns.begin(), timings_ns.end());

    double sum_ns = 0.0;

    for (const double timing : timings_ns) {
        sum_ns += timing;
    }

    const double mean_ns =
        sum_ns / static_cast<double>(timings_ns.size());

    const double median_ns = timings_ns[timings_ns.size() / 2];
    const double min_ns = timings_ns.front();

    std::cout << std::left << std::setw(38) << name
              << std::right
              << " median: " << std::setw(12) << std::fixed
              << std::setprecision(2) << median_ns / 1.0e6 << " ms"
              << " | mean: " << std::setw(10) << mean_ns / 1.0e6
              << " ms"
              << " | min: " << std::setw(10) << min_ns / 1.0e6
              << " ms";

    if (report_throughput && median_ns > 0.0) {
        const double seconds = median_ns / 1.0e9;

        const double megapixels_per_second =
            static_cast<double>(pixels) / seconds / 1.0e6;

        const double gigabytes_per_second =
            static_cast<double>(bytes) / seconds / 1.0e9;

        std::cout << " | " << std::setprecision(2)
                  << megapixels_per_second << " MPix/s"
                  << " | " << gigabytes_per_second << " GB/s";
    }

    std::cout << '\n';
}

/**
 * @brief Benchmarks construction and zero-initialization.
 *
 * @tparam PixelT Channel storage type.
 * @param width Image width.
 * @param height Image height.
 * @param channels Number of channels.
 */
template <typename PixelT>
void benchmark_construction(
    std::size_t width,
    std::size_t height,
    std::size_t channels
) {
    const std::string name =
        "Construction " +
        std::to_string(width) + "x" +
        std::to_string(height) + "x" +
        std::to_string(channels) + " (" +
        std::to_string(sizeof(PixelT) * 8) + "-bit)";

    run_benchmark(
        name,
        7,
        [=]() -> double {
            const mip::Image<PixelT> image(width, height, channels);

            return static_cast<double>(image.size_bytes());
        },
        width * height,
        width * height * channels * sizeof(PixelT),
        false
    );
}

/**
 * @brief Benchmarks sequential traversal of the raw image buffer.
 *
 * @tparam PixelT Channel storage type.
 * @param image Image to traverse.
 */
template <typename PixelT>
void benchmark_raw_traversal(const mip::Image<PixelT>& image) {
    const auto* data = image.data();
    const std::size_t elements = image.num_elements();

    const std::string name =
        "Raw traversal (" +
        std::to_string(sizeof(PixelT) * 8) + "-bit, " +
        std::to_string(image.width()) + "x" +
        std::to_string(image.height()) + ")";

    run_benchmark(
        name,
        7,
        [&]() -> double {
            double sum = 0.0;

            for (std::size_t i = 0; i < elements; ++i) {
                sum += static_cast<double>(data[i]);
            }

            return sum;
        },
        image.num_pixels(),
        image.size_bytes()
    );
}

/**
 * @brief Benchmarks checked access to every image channel.
 *
 * @tparam PixelT Channel storage type.
 * @param image Image to traverse.
 */
template <typename PixelT>
void benchmark_checked_access(const mip::Image<PixelT>& image) {
    const std::string name =
        "Checked at() access (" +
        std::to_string(sizeof(PixelT) * 8) + "-bit, " +
        std::to_string(image.width()) + "x" +
        std::to_string(image.height()) + ")";

    run_benchmark(
        name,
        7,
        [&]() -> double {
            double sum = 0.0;

            for (std::size_t y = 0; y < image.height(); ++y) {
                for (std::size_t x = 0; x < image.width(); ++x) {
                    for (std::size_t c = 0; c < image.channels(); ++c) {
                        sum += static_cast<double>(image.at(x, y, c));
                    }
                }
            }

            return sum;
        },
        image.num_pixels(),
        image.size_bytes()
    );
}

/**
 * @brief Benchmarks pixel-pointer access to every pixel.
 *
 * @tparam PixelT Channel storage type.
 * @param image Image to traverse.
 */
template <typename PixelT>
void benchmark_pixel_access(const mip::Image<PixelT>& image) {
    const std::string name =
        "Pixel access (" +
        std::to_string(sizeof(PixelT) * 8) + "-bit, " +
        std::to_string(image.width()) + "x" +
        std::to_string(image.height()) + ")";

    run_benchmark(
        name,
        7,
        [&]() -> double {
            double sum = 0.0;

            for (std::size_t y = 0; y < image.height(); ++y) {
                for (std::size_t x = 0; x < image.width(); ++x) {
                    const PixelT* pixel = image.pixel(x, y);

                    for (std::size_t c = 0; c < image.channels(); ++c) {
                        sum += static_cast<double>(pixel[c]);
                    }
                }
            }

            return sum;
        },
        image.num_pixels(),
        image.size_bytes()
    );
}

/**
 * @brief Runs all benchmarks for one image type and resolution.
 *
 * @tparam PixelT Channel storage type.
 * @param width Image width.
 * @param height Image height.
 */
template <typename PixelT>
void benchmark_image_size(
    std::size_t width,
    std::size_t height
) {
    constexpr std::size_t channels = 3;

    mip::Image<PixelT> image(width, height, channels);

    // Initialize the image with deterministic nonzero values.
    for (std::size_t i = 0; i < image.num_elements(); ++i) {
        image.data()[i] = static_cast<PixelT>((i % 251U) + 1U);
    }

    benchmark_construction<PixelT>(width, height, channels);
    benchmark_raw_traversal(image);
    benchmark_checked_access(image);
    benchmark_pixel_access(image);
}

} // namespace

int main() {
    std::cout << "mip image container benchmarks\n";
    std::cout << "==============================\n\n";

    std::cout << "Timing statistics are reported per operation.\n";
    std::cout << "Throughput is estimated from image bytes and pixels.\n\n";

    std::cout << "--- 640x480 images ---\n";

    benchmark_image_size<std::uint8_t>(640, 480);
    benchmark_image_size<float>(640, 480);

    std::cout << "\n--- 1920x1080 images ---\n";

    benchmark_image_size<std::uint8_t>(1920, 1080);
    benchmark_image_size<float>(1920, 1080);

    std::cout << "\nBenchmark sink: " << benchmark_sink << '\n';

    return 0;
}