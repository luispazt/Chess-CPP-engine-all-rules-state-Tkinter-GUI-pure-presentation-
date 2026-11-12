#include "chess/Move.h"
#include "chess/Exceptions.h"

namespace chess {

std::string Move::toUci() const {
    std::string s = from.toString() + to.toString();
    if (promotion.has_value()) {
        switch (*promotion) {
            case PieceType::Queen: s += 'q'; break;
            case PieceType::Rook:  s += 'r'; break;
            case PieceType::Bishop:s += 'b'; break;
            case PieceType::Knight:s += 'n'; break;
            default: break;
        }
    }
    return s;
}

Move Move::fromUci(std::string_view uci) {
    Move m;
    if (uci.size() < 4 || uci.size() > 5)
        throw InvalidSquare("invalid UCI: " + std::string(uci));
    m.from = Square::fromString(uci.substr(0, 2));
    m.to = Square::fromString(uci.substr(2, 2));
    if (uci.size() == 5) {
        switch (uci[4]) {
            case 'q': m.promotion = PieceType::Queen; break;
            case 'r': m.promotion = PieceType::Rook; break;
            case 'b': m.promotion = PieceType::Bishop; break;
            case 'n': m.promotion = PieceType::Knight; break;
            default: throw InvalidSquare("invalid promotion: " + std::string(uci));
        }
    }
    return m;
}

}


