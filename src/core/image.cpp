/**
 * @file image.cpp
 * @brief Explicit template instantiations for the mip image container.
 */

#include "mip/image.hpp"

namespace mip {

template class Image<std::uint8_t>;
template class Image<std::uint16_t>;
template class Image<std::uint32_t>;
template class Image<float>;
template class Image<double>;

} // namespace mip