#pragma once

#include "chess/Piece.h"

namespace chess {

class Pawn : public Piece {
public:
    using Piece::Piece;

    PieceType type() const noexcept override { return PieceType::Pawn; }
    char fenSymbol() const noexcept override;
    int value() const noexcept override { return 1; }
    std::string name() const override { return "pawn"; }

    std::vector<Move> pseudoLegalMoves(const Board& board, Square from) const override;
    std::unique_ptr<Piece> clone() const override;
};

}


