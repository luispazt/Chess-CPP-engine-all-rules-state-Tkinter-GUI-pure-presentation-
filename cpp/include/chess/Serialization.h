#pragma once

#include "chess/Board.h"
#include <string>

namespace chess {

std::string fenSymbolOf(PieceType type, Color c);
std::string boardToFen(const Board& b);

}
