#include "test.h"
#include "chess/Game.h"

using namespace chess;

static void test_game_initial_state() {
    Game g;
    REQUIRE(g.sideToMove() == Color::White);
    REQUIRE(g.status() == GameStatus::InProgress);
    REQUIRE_FALSE(g.isInCheck(Color::White));
}

static void test_game_try_move() {
    Game g;
    REQUIRE(g.tryMoveFromUci("e2e4"));
    REQUIRE(g.sideToMove() == Color::Black);
    REQUIRE_FALSE(g.tryMoveFromUci("e2e4"));
    REQUIRE(g.status() == GameStatus::InProgress);
}

static void test_game_undo() {
    Game g;
    g.tryMoveFromUci("e2e4");
    g.undo();
    REQUIRE(g.sideToMove() == Color::White);
    REQUIRE(g.status() == GameStatus::InProgress);
}

static void test_game_legal_moves_from() {
    Game g;
    auto moves = g.legalMovesFrom(Square::fromString("e2"));
    bool hasE4 = false;
    for (const Move& m : moves) {
        if (m.to == Square::fromString("e4")) hasE4 = true;
    }
    REQUIRE(hasE4);
}

static void test_game_fools_mate_checkmate() {
    Game g;
    REQUIRE(g.tryMoveFromUci("f2f3"));
    REQUIRE(g.tryMoveFromUci("e7e5"));
    REQUIRE(g.tryMoveFromUci("g2g4"));
    REQUIRE(g.tryMoveFromUci("d8h4"));
    REQUIRE(g.status() == GameStatus::Checkmate);
}

static void test_game_enpassant() {
    Game g;
    REQUIRE(g.tryMoveFromUci("e2e4"));
    REQUIRE(g.tryMoveFromUci("e7e5"));
    REQUIRE(g.tryMoveFromUci("e4e5"));
    REQUIRE(g.tryMoveFromUci("d7d5"));
    auto moves = g.legalMovesFrom(Square::fromString("e5"));
    bool hasD6 = false;
    for (const Move& m : moves) {
        if (m.to == Square::fromString("d6") && m.enPassant) hasD6 = true;
    }
    REQUIRE(hasD6);
    REQUIRE(g.tryMoveFromUci("e5d6"));
    REQUIRE(g.board().empty(Square::fromString("d5")));
    REQUIRE(!g.board().empty(Square::fromString("d6")));
}

static void test_game_en_passant_undo() {
    Game g;
    REQUIRE(g.tryMoveFromUci("e2e4"));
    REQUIRE(g.tryMoveFromUci("e7e5"));
    REQUIRE(g.tryMoveFromUci("e4e5"));
    REQUIRE(g.tryMoveFromUci("d7d5"));
    REQUIRE(g.tryMoveFromUci("e5d6"));
    g.undo();
    REQUIRE(!g.board().empty(Square::fromString("d5")));
    REQUIRE(g.board().empty(Square::fromString("d6")));
    REQUIRE(g.board().at(Square::fromString("e5")) != nullptr);
    REQUIRE(g.sideToMove() == Color::White);
}
