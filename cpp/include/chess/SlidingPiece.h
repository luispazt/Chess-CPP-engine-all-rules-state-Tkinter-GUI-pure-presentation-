#pragma once

#include "chess/Piece.h"
#include <span>
#include <utility>

namespace chess {

class SlidingPiece : public Piece {
public:
    explicit SlidingPiece(Color c) : Piece(c) {}

    std::vector<Move> slide(const Board& board, Square from,
                            std::span<const std::pair<int,int>> directions) const;
};

}


