#pragma once

#include "chess/Square.h"
#include "chess/PieceType.h"
#include <optional>
#include <string>
#include <string_view>

namespace chess {

struct Move {
    Square from, to;
    std::optional<PieceType> promotion;
    bool castleKingside = false;
    bool castleQueenside = false;
    bool enPassant = false;

    std::string toUci() const;
    static Move fromUci(std::string_view uci);
};

}


