#include "chess/Game.h"
#include "chess/Serialization.h"
#include <algorithm>
#include <sstream>

namespace chess {

Game::Game() : board_(Board()) {}

Color Game::sideToMove() const { return turn_; }

GameStatus Game::status() const {
    std::vector<Move> legal = allLegalMoves();
    if (isInCheck(turn_)) {
        if (legal.empty()) return GameStatus::Checkmate;
        return GameStatus::Check;
    }
    if (legal.empty()) return GameStatus::Stalemate;
    if (halfmoveClock_ >= 100) return GameStatus::DrawFiftyMove;
    return GameStatus::InProgress;
}

bool Game::isInCheck(Color c) const {
    Square king = board_.findKing(c);
    return board_.isAttackedBy(king, opponent(c));
}

std::vector<Move> Game::legalMovesFrom(Square s) const {
    std::vector<Move> result;
    for (const Move& m : allLegalMoves()) {
        if (m.from == s) result.push_back(m);
    }
    return result;
}

std::vector<Move> Game::allLegalMoves() const {
    std::vector<Move> pseudo = generatePseudoLegal(turn_);
    std::vector<Move> legal;
    legal.reserve(pseudo.size());
    for (const Move& m : pseudo) {
        if (isLegal(m)) legal.push_back(m);
    }
    return legal;
}

const Board& Game::board() const { return board_; }

const std::vector<Move>& Game::history() const { return history_; }

bool Game::tryMove(const Move& m) {
    if (!isLegal(m)) return false;

    std::optional<Square> newTarget;
    if (board_.at(m.from) && board_.at(m.from)->type() == PieceType::Pawn) {
        int dr = m.to.rank() - m.from.rank();
        if (dr == 2 || dr == -2) {
            int midRank = (m.from.rank() + m.to.rank()) / 2;
            newTarget = Square{};
            newTarget->index = midRank * 8 + m.from.file();
        }
    }
    apply(m);
    enPassantTarget_ = newTarget;
    return true;
}

bool Game::tryMoveFromUci(std::string_view uci) {
    Move m = Move::fromUci(uci);
    return tryMove(m);
}

void Game::undo() {
    if (undoStack_.empty()) return;
    UndoInfo info = std::move(undoStack_.back());
    undoStack_.pop_back();
    history_.pop_back();

    // Reverse the move: move piece from to back to from
    board_.set(info.move.from, board_.take(info.move.to));
    // Restore captured piece
    if (info.epCaptured.has_value()) {
        board_.set(*info.epCaptured, std::move(info.captured));
    } else {
        board_.set(info.move.to, std::move(info.captured));
    }

    // Undo castling rook
    if (info.move.castleKingside || info.move.castleQueenside) {
        Color side = opponent(turn_);
        Square rookFrom, rookTo;
        if (info.move.castleKingside) {
            rookFrom = (side == Color::White) ? Square{5} : Square{61};
            rookTo = (side == Color::White) ? Square{7} : Square{63};
        } else {
            rookFrom = (side == Color::White) ? Square{3} : Square{59};
            rookTo = (side == Color::White) ? Square{0} : Square{56};
        }
        board_.set(rookTo, board_.take(rookFrom));
    }

    turn_ = opponent(turn_);
    castleKingside_ = info.castleKingside;
    castleQueenside_ = info.castleQueenside;
    enPassantTarget_ = info.enPassantTarget;
    halfmoveClock_ = info.halfmoveClock;
    fullmoveNumber_ = info.fullmoveNumber;
}

void Game::reset() {
    board_ = Board();
    turn_ = Color::White;
    castleKingside_ = {true, true};
    castleQueenside_ = {true, true};
    enPassantTarget_ = std::nullopt;
    halfmoveClock_ = 0;
    fullmoveNumber_ = 1;
    history_.clear();
    undoStack_.clear();
}

std::string Game::toJson() const {
    GameStatus st = status();
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"turn\": \"" << colorName(turn_) << "\",\n";
    std::string statusStr;
    switch (st) {
        case GameStatus::InProgress: statusStr = "in_progress"; break;
        case GameStatus::Check: statusStr = "check"; break;
        case GameStatus::Checkmate: statusStr = "checkmate"; break;
        case GameStatus::Stalemate: statusStr = "stalemate"; break;
        case GameStatus::DrawFiftyMove: statusStr = "draw_fifty"; break;
        case GameStatus::DrawInsufficientMaterial: statusStr = "draw_material"; break;
        case GameStatus::DrawThreefold: statusStr = "draw_threefold"; break;
    }
    oss << "  \"status\": \"" << statusStr << "\",\n";
    oss << "  \"inCheck\": " << (isInCheck(turn_) ? "true" : "false") << ",\n";
    oss << "  \"board\": \"" << boardToFen(board_) << "\",\n";
    oss << "  \"history\": [";
    for (size_t i = 0; i < history_.size(); ++i) {
        if (i) oss << ",";
        oss << "\"" << history_[i].toUci() << "\"";
    }
    oss << "],\n";
    if (history_.empty()) {
        oss << "  \"lastMove\": null\n";
    } else {
        const Move& lm = history_.back();
        oss << "  \"lastMove\": {\"from\": \"" << lm.from.toString() << "\", \"to\": \"" << lm.to.toString() << "\"}\n";
    }
    oss << "}";
    return oss.str();
}

std::vector<std::string> Game::legalMovesFromUci(std::string_view square) const {
    std::vector<std::string> result;
    try {
        Square s = Square::fromString(square);
        for (const Move& m : legalMovesFrom(s)) {
            result.push_back(m.toUci());
        }
    } catch (...) {}
    return result;
}

std::vector<Move> Game::generatePseudoLegal(Color side) const {
    std::vector<Move> moves;
    for (int i = 0; i < 64; ++i) {
        if (!board_.squares_[i] || board_.squares_[i]->color() != side) continue;
        Square from{static_cast<int>(i)};
        for (const Move& m : board_.squares_[i]->pseudoLegalMoves(board_, from))
            moves.push_back(m);
    }

    // En passant captures
    if (enPassantTarget_.has_value()) {
        Square ep = *enPassantTarget_;
        int dir = (side == Color::White) ? 1 : -1;
        int fromRank = ep.rank() - dir;
        if (fromRank >= 0 && fromRank <= 7) {
            for (int df : {-1, 1}) {
                int f = ep.file() + df;
                if (f < 0 || f > 7) continue;
                Square from{fromRank * 8 + f};
                Piece* p = board_.at(from);
                if (p && p->color() == side && p->type() == PieceType::Pawn)
                    moves.push_back({from, ep, std::nullopt, false, false, true});
            }
        }
    }
    // Castling
    {
        int idx = (side == Color::White) ? 0 : 1;
        Square kingSq = (side == Color::White) ? Square{4} : Square{60};
        Piece* king = board_.at(kingSq);
        if (king && king->color() == side && king->type() == PieceType::King && !isInCheck(side)) {
            // Kingside
            if ((side == Color::White ? castleKingside_[0] : castleKingside_[1])) {
                Square f = (side == Color::White) ? Square{5} : Square{61};
                Square g = (side == Color::White) ? Square{6} : Square{62};
                Square h = (side == Color::White) ? Square{7} : Square{63};
                Piece* rook = board_.at(h);
                if (rook && rook->color() == side && rook->type() == PieceType::Rook &&
                    board_.empty(f) && board_.empty(g) &&
                    !board_.isAttackedBy(f, opponent(side)) &&
                    !board_.isAttackedBy(g, opponent(side))) {
                    Move cm; cm.from = kingSq; cm.to = g; cm.castleKingside = true;
                    moves.push_back(cm);
                }
            }
            // Queenside
            if ((side == Color::White ? castleQueenside_[0] : castleQueenside_[1])) {
                Square d = (side == Color::White) ? Square{3} : Square{59};
                Square c = (side == Color::White) ? Square{2} : Square{58};
                Square b = (side == Color::White) ? Square{1} : Square{57};
                Square a = (side == Color::White) ? Square{0} : Square{56};
                Piece* rook = board_.at(a);
                if (rook && rook->color() == side && rook->type() == PieceType::Rook &&
                    board_.empty(b) && board_.empty(c) && board_.empty(d) &&
                    !board_.isAttackedBy(d, opponent(side)) &&
                    !board_.isAttackedBy(c, opponent(side))) {
                    Move cm; cm.from = kingSq; cm.to = c; cm.castleQueenside = true;
                    moves.push_back(cm);
                }
            }
        }
    }
    return moves;
}
bool Game::isLegal(const Move& m) const {
    // Castling validation
    bool castleKS = m.castleKingside;
    bool castleQS = m.castleQueenside;
    Piece* kp = board_.at(m.from);
    if (!castleKS && !castleQS && kp && kp->type() == PieceType::King &&
        (m.from.file() - m.to.file() > 1 || m.to.file() - m.from.file() > 1)) {
        if (m.to.file() == 6) castleKS = true;
        else if (m.to.file() == 2) castleQS = true;
    }
    if (castleKS || castleQS) {
        Color side = turn_;
        int idx = (side == Color::White) ? 0 : 1;
        if (castleKS && !(side == Color::White ? castleKingside_[0] : castleKingside_[1])) return false;
        if (castleQS && !(side == Color::White ? castleQueenside_[0] : castleQueenside_[1])) return false;
        Square kingStart = (side == Color::White) ? Square{4} : Square{60};
        Square kingDest = (side == Color::White)
            ? (castleKS ? Square{6} : Square{2})
            : (castleKS ? Square{62} : Square{58});
        if (m.from != kingStart || m.to != kingDest) return false;
        Square rookStart = (side == Color::White)
            ? (castleKS ? Square{7} : Square{0})
            : (castleKS ? Square{63} : Square{56});
        Piece* rook = board_.at(rookStart);
        if (!rook || rook->color() != side || rook->type() != PieceType::Rook) return false;
        if (isInCheck(side)) return false;
        if (castleKS) {
            Square f = (side == Color::White) ? Square{5} : Square{61};
            Square g = (side == Color::White) ? Square{6} : Square{62};
            if (!board_.empty(f) || !board_.empty(g)) return false;
            if (board_.isAttackedBy(f, opponent(side))) return false;
            if (board_.isAttackedBy(g, opponent(side))) return false;
        } else {
            Square b = (side == Color::White) ? Square{1} : Square{57};
            Square c = (side == Color::White) ? Square{2} : Square{58};
            Square d = (side == Color::White) ? Square{3} : Square{59};
            if (!board_.empty(b) || !board_.empty(c) || !board_.empty(d)) return false;
            if (board_.isAttackedBy(d, opponent(side))) return false;
            if (board_.isAttackedBy(c, opponent(side))) return false;
        }
        return true;
    }

    Board b = std::move(*board_.clone());
    std::unique_ptr<Piece> moved = b.take(m.from);
    if (!moved) return false;
    std::unique_ptr<Piece> captured = b.take(m.to);
    b.set(m.to, std::move(moved));
    Square king = b.findKing(turn_);
    return !b.isAttackedBy(king, opponent(turn_));
}

void Game::apply(const Move& m) {
    UndoInfo info;
    info.castleKingside = castleKingside_;
    info.castleQueenside = castleQueenside_;
    info.enPassantTarget = enPassantTarget_;
    info.halfmoveClock = halfmoveClock_;
    info.fullmoveNumber = fullmoveNumber_;
    info.move = m;

    Color side = turn_;

    std::unique_ptr<Piece> moved = board_.take(m.from);
    bool isEp = false;
    Square epSquare;
    if (moved && moved->type() == PieceType::Pawn &&
        enPassantTarget_.has_value() && m.to == *enPassantTarget_ &&
        (m.from.file() == m.to.file() + 1 || m.from.file() == m.to.file() - 1)) {
        isEp = true;
        int dir = (turn_ == Color::White) ? 1 : -1;
        epSquare.index = (m.to.rank() - dir) * 8 + m.to.file();
        info.epCaptured = epSquare;
        info.captured = board_.take(epSquare);
    } else {
        info.captured = board_.take(m.to);
    }

    bool castleKS = m.castleKingside;
    bool castleQS = m.castleQueenside;
    Piece* kp = moved.get();
    if (!castleKS && !castleQS && kp && kp->type() == PieceType::King &&
        (m.from.file() > m.to.file() + 1 || m.to.file() > m.from.file() + 1)) {
        if (m.to.file() == 6) castleKS = true;
        else if (m.to.file() == 2) castleQS = true;
    }
    if (castleKS || castleQS) {
        Square rookFrom, rookTo;
        if (castleKS) {
            rookFrom = (side == Color::White) ? Square{7} : Square{63};
            rookTo = (side == Color::White) ? Square{5} : Square{61};
        } else {
            rookFrom = (side == Color::White) ? Square{0} : Square{56};
            rookTo = (side == Color::White) ? Square{3} : Square{59};
        }
        std::unique_ptr<Piece> rook = board_.take(rookFrom);
        board_.set(rookTo, std::move(rook));
    }

    int sideIdx = (side == Color::White) ? 0 : 1;
    if (moved && moved->type() == PieceType::King) {
        castleKingside_[sideIdx] = false;
        castleQueenside_[sideIdx] = false;
    }
    if (moved && moved->type() == PieceType::Rook) {
        if (m.from == Square{(side == Color::White) ? 7 : 63}) castleKingside_[sideIdx] = false;
        if (m.from == Square{(side == Color::White) ? 0 : 56}) castleQueenside_[sideIdx] = false;
    }
    if (m.to == Square{7}) castleKingside_[0] = false;
    if (m.to == Square{0}) castleQueenside_[0] = false;
    if (m.to == Square{63}) castleKingside_[1] = false;
    if (m.to == Square{56}) castleQueenside_[1] = false;

    info.move.castleKingside = castleKS;
    info.move.castleQueenside = castleQS;

    undoStack_.push_back(std::move(info));
    history_.push_back(m);

    board_.set(m.to, std::move(moved));
    if (isEp) {
        board_.set(epSquare, nullptr);
    }
    turn_ = opponent(turn_);
    if (turn_ == Color::White) ++fullmoveNumber_;
}

}


