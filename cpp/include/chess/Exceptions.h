#pragma once

#include <stdexcept>
#include <string>

namespace chess {

class InvalidMove : public std::runtime_error {
public:
    explicit InvalidMove(const std::string& msg) : std::runtime_error(msg) {}
};

class InvalidSquare : public std::runtime_error {
public:
    explicit InvalidSquare(const std::string& msg) : std::runtime_error(msg) {}
};

}


