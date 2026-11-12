#pragma once

#include "chess/SlidingPiece.h"

namespace chess {

class Rook : public SlidingPiece {
public:
    using SlidingPiece::SlidingPiece;

    PieceType type() const noexcept override { return PieceType::Rook; }
    char fenSymbol() const noexcept override;
    int value() const noexcept override { return 5; }
    std::string name() const override { return "rook"; }

    std::vector<Move> pseudoLegalMoves(const Board& board, Square from) const override;
    std::unique_ptr<Piece> clone() const override;
};

}


