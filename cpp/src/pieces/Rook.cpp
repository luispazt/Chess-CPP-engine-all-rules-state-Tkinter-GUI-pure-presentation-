#include "chess/Rook.h"
#include "chess/Board.h"

namespace chess {

char Rook::fenSymbol() const noexcept {
    return color_ == Color::White ? 'R' : 'r';
}

std::vector<Move> Rook::pseudoLegalMoves(const Board& board, Square from) const {
    static constexpr std::pair<int,int> dirs[] = {{1,0},{-1,0},{0,1},{0,-1}};
    return slide(board, from, dirs);
}

std::unique_ptr<Piece> Rook::clone() const {
    return std::make_unique<Rook>(color_);
}

}


