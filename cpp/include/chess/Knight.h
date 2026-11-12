#pragma once

#include "chess/Piece.h"

namespace chess {

class Knight : public Piece {
public:
    using Piece::Piece;

    PieceType type() const noexcept override { return PieceType::Knight; }
    char fenSymbol() const noexcept override;
    int value() const noexcept override { return 3; }
    std::string name() const override { return "knight"; }

    std::vector<Move> pseudoLegalMoves(const Board& board, Square from) const override;
    std::unique_ptr<Piece> clone() const override;
};

}


