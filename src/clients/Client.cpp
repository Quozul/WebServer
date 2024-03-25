#include "Client.h"

#include <algorithm>

int get_buffer_size(const size_t remaining) {
    constexpr size_t default_buffer_size = BUFFER_SIZE;
    const size_t buffer_size = std::min(default_buffer_size, remaining);
    const auto res = static_cast<int>(buffer_size);
    return res;
}
