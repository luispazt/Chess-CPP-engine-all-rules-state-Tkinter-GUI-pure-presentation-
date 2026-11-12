#pragma once

#include "chess/Square.h"
#include "chess/Piece.h"
#include <array>
#include <memory>
#include <string>
#include <string_view>

namespace chess {

class Board {
    friend class Game;
public:
    Board();
    static Board fromFen(std::string_view fen);

    Piece* at(Square s) const;
    bool empty(Square s) const { return at(s) == nullptr; }
    bool occupiedBy(Square s, Color c) const;
    void set(Square s, std::unique_ptr<Piece> p);
    std::unique_ptr<Piece> take(Square s);

    Square findKing(Color c) const;
    bool isAttackedBy(Square s, Color attacker) const;

    std::unique_ptr<Board> clone() const;

    const std::array<std::unique_ptr<Piece>, 64>& squares() const { return squares_; }

private:
    std::array<std::unique_ptr<Piece>, 64> squares_;
};

}


