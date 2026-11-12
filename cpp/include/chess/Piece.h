#pragma once

#include "chess/Color.h"
#include "chess/PieceType.h"
#include "chess/Move.h"
#include <memory>
#include <string>
#include <vector>

namespace chess {

class Board;

class Piece {
public:
    virtual ~Piece() = default;
    Piece(const Piece&) = delete;
    Piece& operator=(const Piece&) = delete;

    Color color() const noexcept { return color_; }

    virtual PieceType type() const noexcept = 0;
    virtual char fenSymbol() const noexcept = 0;
    virtual int value() const noexcept = 0;
    virtual std::string name() const = 0;

    virtual std::vector<Move> pseudoLegalMoves(const Board& board, Square from) const = 0;

    virtual std::unique_ptr<Piece> clone() const = 0;

    Piece(Color c) : color_(c) {}
    Color color_;
};

}


