#include "test.h"
#include "chess/Board.h"
#include "chess/Pawn.h"
#include "chess/Queen.h"

using namespace chess;

static void test_board_clone() {
    Board b;
    auto c = b.clone();
    REQUIRE(c->at(Square::fromString("e1")) != nullptr);
    REQUIRE(c->at(Square::fromString("e1"))->type() == PieceType::King);
    REQUIRE(c->at(Square::fromString("e1"))->color() == Color::White);
}

static void test_board_find_king() {
    Board b;
    REQUIRE(b.findKing(Color::White) == Square::fromString("e1"));
    REQUIRE(b.findKing(Color::Black) == Square::fromString("e8"));
}

static void test_board_set_take() {
    Board b;
    b.set(Square::fromString("e4"), std::make_unique<Pawn>(Color::White));
    REQUIRE(!b.empty(Square::fromString("e4")));
    auto p = b.take(Square::fromString("e4"));
    REQUIRE(p != nullptr);
    REQUIRE(b.empty(Square::fromString("e4")));
}

static void test_board_is_attacked_by() {
    Board b = Board::fromFen("8/8/8/8/8/8/8/8");
    b.set(Square::fromString("e4"), std::make_unique<Queen>(Color::White));
    REQUIRE(b.isAttackedBy(Square::fromString("a4"), Color::White));
    REQUIRE(!b.isAttackedBy(Square::fromString("a4"), Color::Black));
}

static void test_board_from_fen() {
    Board b = Board::fromFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    REQUIRE(b.at(Square::fromString("a1"))->type() == PieceType::Rook);
    REQUIRE(b.at(Square::fromString("a8"))->color() == Color::Black);
}
