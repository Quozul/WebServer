#include "Body.h"

#include <algorithm>
#include <cstring>

Body::Body() : data(nullptr), length(0), reserved(0) { data = new char[reserved]; }

Body::Body(const Body &other) : data(new char[other.reserved]), length(other.length), reserved(other.reserved) {
    std::memcpy(data, other.data, reserved);
}

Body &Body::operator=(const Body &other) {
    if (this != &other) {
        delete[] data;
        length = other.length;
        reserved = other.reserved;
        data = new char[reserved];
        std::memcpy(data, other.data, reserved);
    }
    return *this;
}

void Body::reserve(const size_t new_reserved) {
    const auto new_data = new char[new_reserved + 1];

    reserved = new_reserved + 1;
    length = std::min(length, new_reserved + 1);
    std::memcpy(new_data, data, length);

    delete[] data;
    data = new_data;
}

void Body::append(const char *content, const size_t n) {
    if (length + n > reserved) {
        reserve(length + n);
    }

    std::memcpy(data + length, content, n);
    length += n;
    data[length] = '\0';
}

Body::~Body() {
    delete[] data;
    data = nullptr;
    length = reserved = 0;
}
