#include "chess/Queen.h"
#include "chess/Board.h"

namespace chess {

char Queen::fenSymbol() const noexcept {
    return color_ == Color::White ? 'Q' : 'q';
}

std::vector<Move> Queen::pseudoLegalMoves(const Board& board, Square from) const {
    static constexpr std::pair<int,int> dirs[] = {
        {1,0},{-1,0},{0,1},{0,-1},{1,1},{1,-1},{-1,1},{-1,-1}
    };
    return slide(board, from, dirs);
}

std::unique_ptr<Piece> Queen::clone() const {
    return std::make_unique<Queen>(color_);
}

}


