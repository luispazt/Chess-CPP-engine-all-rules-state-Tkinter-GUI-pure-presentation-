#pragma once

#include <string>
#include <string_view>
#include <stdexcept>

namespace chess {

struct Square {
    int index = -1;

    bool valid() const { return index >= 0 && index < 64; }
    int file() const { return index % 8; }
    int rank() const { return index / 8; }

    static Square fromString(std::string_view s);
    std::string toString() const;

    bool operator==(const Square&) const = default;
};

}


