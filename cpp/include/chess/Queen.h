#pragma once

#include "chess/SlidingPiece.h"

namespace chess {

class Queen : public SlidingPiece {
public:
    using SlidingPiece::SlidingPiece;

    PieceType type() const noexcept override { return PieceType::Queen; }
    char fenSymbol() const noexcept override;
    int value() const noexcept override { return 9; }
    std::string name() const override { return "queen"; }

    std::vector<Move> pseudoLegalMoves(const Board& board, Square from) const override;
    std::unique_ptr<Piece> clone() const override;
};

}


