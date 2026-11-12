#include "chess/Knight.h"
#include "chess/Board.h"

namespace chess {

char Knight::fenSymbol() const noexcept {
    return color_ == Color::White ? 'N' : 'n';
}

std::vector<Move> Knight::pseudoLegalMoves(const Board& board, Square from) const {
    std::vector<Move> moves;
    static constexpr std::pair<int,int> offsets[] = {
        {1,2},{2,1},{2,-1},{1,-2},{-1,-2},{-2,-1},{-2,1},{-1,2}
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

std::unique_ptr<Piece> Knight::clone() const {
    return std::make_unique<Knight>(color_);
}

}


