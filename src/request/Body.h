#ifndef BODY_H
#define BODY_H

#include <iostream>

class Body {
    char *data;
    size_t length;
    size_t reserved;

  public:
    explicit Body();

    Body(const Body &other);

    Body &operator=(const Body &other);

    void reserve(size_t new_reserved);

    void append(const char *content, size_t n);

    [[nodiscard]] char *c_str() const { return data; }

    [[nodiscard]] std::string str() const { return std::string{data, length}; }

    [[nodiscard]] size_t size() const { return length; }

    [[nodiscard]] size_t get_reserved() const { return reserved; }

    ~Body();
};

#endif
