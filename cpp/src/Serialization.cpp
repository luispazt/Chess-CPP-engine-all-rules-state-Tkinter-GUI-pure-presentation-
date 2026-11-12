#include "chess/Serialization.h"
#include <sstream>

namespace chess {

std::string fenSymbolOf(PieceType type, Color c) {
    char base;
    switch (type) {
        case PieceType::Pawn:   base = 'P'; break;
        case PieceType::Knight: base = 'N'; break;
        case PieceType::Bishop: base = 'B'; break;
        case PieceType::Rook:   base = 'R'; break;
        case PieceType::Queen:  base = 'Q'; break;
        case PieceType::King:   base = 'K'; break;
    }
    return std::string(1, c == Color::White ? base : static_cast<char>(base + 32));
}

std::string boardToFen(const Board& b) {
    std::ostringstream oss;
    for (int rank = 7; rank >= 0; --rank) {
        int empty = 0;
        for (int file = 0; file < 8; ++file) {
            Square sq{rank * 8 + file};
            Piece* p = b.at(sq);
            if (p) {
                if (empty > 0) { oss << empty; empty = 0; }
                oss << fenSymbolOf(p->type(), p->color());
            } else {
                ++empty;
            }
        }
        if (empty > 0) oss << empty;
        if (rank > 0) oss << '/';
    }
    return oss.str();
}

}
