#pragma once

namespace chess {

enum class Color { White, Black };

inline Color opponent(Color c) { return c == Color::White ? Color::Black : Color::White; }

inline const char* colorName(Color c) {
    return c == Color::White ? "white" : "black";
}

}


