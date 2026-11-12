#include "chess/Bishop.h"
#include "chess/Board.h"

namespace chess {

char Bishop::fenSymbol() const noexcept {
    return color_ == Color::White ? 'B' : 'b';
}

std::vector<Move> Bishop::pseudoLegalMoves(const Board& board, Square from) const {
    static constexpr std::pair<int,int> dirs[] = {{1,1},{1,-1},{-1,1},{-1,-1}};
    return slide(board, from, dirs);
}

std::unique_ptr<Piece> Bishop::clone() const {
    return std::make_unique<Bishop>(color_);
}

}


