#pragma once

#include "chess/Piece.h"

namespace chess {

class King : public Piece {
public:
    using Piece::Piece;

    PieceType type() const noexcept override { return PieceType::King; }
    char fenSymbol() const noexcept override;
    int value() const noexcept override { return 0; }
    std::string name() const override { return "king"; }

    std::vector<Move> pseudoLegalMoves(const Board& board, Square from) const override;
    std::unique_ptr<Piece> clone() const override;
};

}


