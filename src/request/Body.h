#ifndef BODY_H
#define BODY_H

#include <algorithm>
#include <cstring>
#include <iostream>

class Body {
    char *data;
    size_t length;
    size_t reserved;

  public:
    explicit Body() : data(nullptr), length(0), reserved(0) { data = new char[reserved]; }

    Body(const Body &other) : data(new char[other.reserved]), length(other.length), reserved(other.reserved) {
        std::memcpy(data, other.data, reserved);
    }

    Body &operator=(const Body &other) {
        if (this != &other) {
            delete[] data;
            length = other.length;
            reserved = other.reserved;
            data = new char[reserved];
            std::memcpy(data, other.data, reserved);
        }
        return *this;
    }

    void reserve(const size_t new_reserved) {
        const auto new_data = new char[new_reserved + 1];

        reserved = new_reserved + 1;
        length = std::min(length, new_reserved + 1);
        std::memcpy(new_data, data, length);

        delete[] data;
        data = new_data;
    }

    void append(const char *content, const size_t n) {
        if (length + n > reserved) {
            reserve(length + n);
        }

        std::memcpy(data + length, content, n);
        length += n;
        data[length] = '\0';
    }

    [[nodiscard]] char *c_str() const { return data; }

    [[nodiscard]] std::string str() const { return std::string{data, length}; }

    [[nodiscard]] size_t size() const { return length; }

    [[nodiscard]] size_t get_reserved() const { return reserved; }

    ~Body() {
        delete[] data;
        data = nullptr;
        length = reserved = 0;
    }
};

#endif
