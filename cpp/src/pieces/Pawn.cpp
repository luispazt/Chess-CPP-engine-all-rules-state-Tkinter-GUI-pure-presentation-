#include "chess/Pawn.h"
#include "chess/Board.h"
#include "chess/Game.h"

namespace chess {

char Pawn::fenSymbol() const noexcept {
    return color_ == Color::White ? 'P' : 'p';
}

std::vector<Move> Pawn::pseudoLegalMoves(const Board& board, Square from) const {
    std::vector<Move> moves;
    int dir = (color_ == Color::White) ? 1 : -1;
    int startRank = (color_ == Color::White) ? 1 : 6;
    int promotionRank = (color_ == Color::White) ? 6 : 1;

    Square oneStep;
    oneStep.index = from.index + dir * 8;
    if (oneStep.valid() && board.empty(oneStep)) {
        if (from.rank() == promotionRank) {
            moves.push_back({from, oneStep, PieceType::Queen});
            moves.push_back({from, oneStep, PieceType::Rook});
            moves.push_back({from, oneStep, PieceType::Bishop});
            moves.push_back({from, oneStep, PieceType::Knight});
        } else {
            moves.push_back({from, oneStep});
        }

        Square twoStep;
        twoStep.index = from.index + dir * 16;
        if (from.rank() == startRank && board.empty(twoStep)) {
            moves.push_back({from, twoStep, std::nullopt, false, false, true});
        }
    }

    for (int df : {-1, 1}) {
        Square cap;
        cap.index = from.index + dir * 8 + df;
        if (!cap.valid() || cap.file() < 0 || cap.file() > 7) continue;
        if (cap.file() == from.file() + df && board.occupiedBy(cap, opponent(color_))) {
            if (from.rank() == promotionRank) {
                moves.push_back({from, cap, PieceType::Queen});
                moves.push_back({from, cap, PieceType::Rook});
                moves.push_back({from, cap, PieceType::Bishop});
                moves.push_back({from, cap, PieceType::Knight});
            } else {
                moves.push_back({from, cap});
            }
        }
    }
    return moves;
}

std::unique_ptr<Piece> Pawn::clone() const {
    return std::make_unique<Pawn>(color_);
}

}


