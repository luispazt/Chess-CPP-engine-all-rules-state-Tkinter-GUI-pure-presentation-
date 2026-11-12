#include "chess/Square.h"
#include "chess/Exceptions.h"

namespace chess {

Square Square::fromString(std::string_view s) {
    if (s.size() != 2) throw InvalidSquare("invalid square: " + std::string(s));
    int file = s[0] - 'a';
    int rank = s[1] - '1';
    if (file < 0 || file > 7 || rank < 0 || rank > 7)
        throw InvalidSquare("invalid square: " + std::string(s));
    Square sq;
    sq.index = rank * 8 + file;
    return sq;
}

std::string Square::toString() const {
    if (!valid()) return "";
    std::string s(2, ' ');
    s[0] = static_cast<char>('a' + file());
    s[1] = static_cast<char>('1' + rank());
    return s;
}

}


