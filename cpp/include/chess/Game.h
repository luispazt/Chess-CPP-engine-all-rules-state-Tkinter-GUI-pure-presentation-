#pragma once

#include "chess/Board.h"
#include "chess/Color.h"
#include "chess/GameStatus.h"
#include "chess/Move.h"
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace chess {

class Game {
public:
    Game();

    Color sideToMove() const;
    GameStatus status() const;
    bool isInCheck(Color c) const;
    std::vector<Move> legalMovesFrom(Square s) const;
    std::vector<Move> allLegalMoves() const;
    const Board& board() const;
    const std::vector<Move>& history() const;

    bool tryMove(const Move& m);
    bool tryMoveFromUci(std::string_view uci);
    void undo();
    void reset();

    std::string toJson() const;
    std::vector<std::string> legalMovesFromUci(std::string_view square) const;

private:
    std::vector<Move> generatePseudoLegal(Color side) const;
    bool isLegal(const Move& m) const;
    void apply(const Move& m);
    void revert(const Move& m);
    struct UndoInfo {
        Move move;
        std::unique_ptr<Piece> captured;
        std::optional<Square> epCaptured;  // square the en-passant pawn was taken from
        std::array<bool,2> castleKingside{};
        std::array<bool,2> castleQueenside{};
        std::optional<Square> enPassantTarget;
        int halfmoveClock = 0;
        int fullmoveNumber = 1;
    };

    Board board_;
    Color turn_ = Color::White;
    std::array<bool,2> castleKingside_{true, true};
    std::array<bool,2> castleQueenside_{true, true};
    std::optional<Square> enPassantTarget_;
    int halfmoveClock_ = 0;
    int fullmoveNumber_ = 1;
    std::vector<Move> history_;
    std::vector<UndoInfo> undoStack_;
};

}


