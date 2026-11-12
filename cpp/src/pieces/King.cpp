#include "chess/King.h"
#include "chess/Board.h"

namespace chess {

char King::fenSymbol() const noexcept {
    return color_ == Color::White ? 'K' : 'k';
}

std::vector<Move> King::pseudoLegalMoves(const Board& board, Square from) const {
    std::vector<Move> moves;
    static constexpr std::pair<int,int> offsets[] = {
        {1,0},{-1,0},{0,1},{0,-1},{1,1},{1,-1},{-1,1},{-1,-1}
    };
    for (auto [df, dr] : offsets) {
        int nf = from.file() + df;
        int nr = from.rank() + dr;
        if (nf < 0 || nf > 7 || nr < 0 || nr > 7) continue;
        Square to;
        to.index = nr * 8 + nf;
        if (board.empty(to) || board.occupiedBy(to, opponent(color_)))
            moves.push_back({from, to});
    }
    return moves;
}

std::unique_ptr<Piece> King::clone() const {
    return std::make_unique<King>(color_);
}

}


