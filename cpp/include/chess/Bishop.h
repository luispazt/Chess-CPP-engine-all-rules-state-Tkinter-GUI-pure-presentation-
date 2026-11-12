#pragma once

#include "chess/SlidingPiece.h"

namespace chess {

class Bishop : public SlidingPiece {
public:
    using SlidingPiece::SlidingPiece;

    PieceType type() const noexcept override { return PieceType::Bishop; }
    char fenSymbol() const noexcept override;
    int value() const noexcept override { return 3; }
    std::string name() const override { return "bishop"; }

    std::vector<Move> pseudoLegalMoves(const Board& board, Square from) const override;
    std::unique_ptr<Piece> clone() const override;
};

}


