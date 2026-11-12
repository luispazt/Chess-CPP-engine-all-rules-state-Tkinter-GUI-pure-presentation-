#include "chess/Board.h"
#include "chess/Pawn.h"
#include "chess/Knight.h"
#include "chess/Bishop.h"
#include "chess/Rook.h"
#include "chess/Queen.h"
#include "chess/King.h"
#include "chess/Exceptions.h"

namespace chess {

namespace {

std::unique_ptr<Piece> pieceFromChar(char c, Color color) {
    switch (c) {
        case 'P': return std::make_unique<Pawn>(color);
        case 'N': return std::make_unique<Knight>(color);
        case 'B': return std::make_unique<Bishop>(color);
        case 'R': return std::make_unique<Rook>(color);
        case 'Q': return std::make_unique<Queen>(color);
        case 'K': return std::make_unique<King>(color);
        default: return nullptr;
    }
}

}

Board::Board() {
    // Standard starting position
    const char* backRank[] = {"R","N","B","Q","K","B","N","R"};
    for (int f = 0; f < 8; ++f) {
        set(Square::fromString(std::string(1, 'a'+f) + "8"),
            pieceFromChar(backRank[f][0], Color::Black));
        set(Square::fromString(std::string(1, 'a'+f) + "7"),
            std::make_unique<Pawn>(Color::Black));
        set(Square::fromString(std::string(1, 'a'+f) + "2"),
            std::make_unique<Pawn>(Color::White));
        set(Square::fromString(std::string(1, 'a'+f) + "1"),
            pieceFromChar(backRank[f][0], Color::White));
    }
}

Board Board::fromFen(std::string_view fen) {
    Board b;
    for (auto& sq : b.squares_) sq = nullptr;  // clear (default constructed)

    int rank = 7;
    int file = 0;
    for (char c : fen) {
        if (c == '/') {
            --rank;
            file = 0;
        } else if (c >= '1' && c <= '8') {
            file += c - '0';
        } else {
            Color color = (c >= 'A' && c <= 'Z') ? Color::White : Color::Black;
            char upper = (c >= 'a' && c <= 'z') ? static_cast<char>(c - 'a' + 'A') : c;
            Square sq;
            sq.index = rank * 8 + file;
            b.set(sq, pieceFromChar(upper, color));
            ++file;
        }
    }
    return b;
}

Piece* Board::at(Square s) const {
    if (!s.valid()) return nullptr;
    return squares_[s.index].get();
}

bool Board::occupiedBy(Square s, Color c) const {
    Piece* p = at(s);
    return p != nullptr && p->color() == c;
}

void Board::set(Square s, std::unique_ptr<Piece> p) {
    if (!s.valid()) throw InvalidSquare("invalid square");
    squares_[s.index] = std::move(p);
}

std::unique_ptr<Piece> Board::take(Square s) {
    if (!s.valid()) throw InvalidSquare("invalid square");
    return std::move(squares_[s.index]);
}

Square Board::findKing(Color c) const {
    for (int i = 0; i < 64; ++i) {
        if (squares_[i] && squares_[i]->color() == c && squares_[i]->type() == PieceType::King)
            return Square{i};
    }
    throw InvalidSquare("king not found");
}

bool Board::isAttackedBy(Square s, Color attacker) const {
    for (int i = 0; i < 64; ++i) {
        if (!squares_[i] || squares_[i]->color() != attacker) continue;
        Square from{i};
        for (const Move& m : squares_[i]->pseudoLegalMoves(*this, from)) {
            if (m.to == s) return true;
        }
    }
    return false;
}

std::unique_ptr<Board> Board::clone() const {
    auto b = std::make_unique<Board>();
    for (auto& sq : b->squares_) sq = nullptr;
    for (int i = 0; i < 64; ++i) {
        if (squares_[i]) b->squares_[i] = squares_[i]->clone();
    }
    return b;
}

}


