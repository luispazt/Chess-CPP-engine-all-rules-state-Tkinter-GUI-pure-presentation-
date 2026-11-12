#include "chess/SlidingPiece.h"
#include "chess/Board.h"

namespace chess {

std::vector<Move> SlidingPiece::slide(const Board& board, Square from,
                                      std::span<const std::pair<int,int>> directions) const {
    std::vector<Move> moves;
    for (auto [df, dr] : directions) {
        int f = from.file() + df;
        int r = from.rank() + dr;
        while (f >= 0 && f <= 7 && r >= 0 && r <= 7) {
            Square to;
            to.index = r * 8 + f;
            if (board.empty(to)) {
                moves.push_back({from, to});
            } else {
                if (board.occupiedBy(to, opponent(color_)))
                    moves.push_back({from, to});
                break;
            }
            f += df;
            r += dr;
        }
    }
    return moves;
}

}


