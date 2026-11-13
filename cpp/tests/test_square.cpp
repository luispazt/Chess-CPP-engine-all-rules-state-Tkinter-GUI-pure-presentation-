#include "test.h"
#include "chess/Square.h"
#include "chess/Move.h"
#include "chess/Exceptions.h"

using namespace chess;

static void test_square_round_trip() {
    REQUIRE(Square::fromString("e2").index == 12);
    REQUIRE(Square::fromString("a1").index == 0);
    REQUIRE(Square::fromString("h8").index == 63);
    REQUIRE(Square::fromString("e2").toString() == "e2");
    REQUIRE(Square::fromString("a1").file() == 0);
    REQUIRE(Square::fromString("a1").rank() == 0);
}

static void test_square_invalid_throws() {
    REQUIRE_THROWS_AS(Square::fromString("i9"), InvalidSquare);
    REQUIRE_THROWS_AS(Square::fromString("e"), InvalidSquare);
}

static void test_move_uci_parsing() {
    Move m = Move::fromUci("e2e4");
    REQUIRE(m.from == Square::fromString("e2"));
    REQUIRE(m.to == Square::fromString("e4"));
    REQUIRE(!m.promotion.has_value());
    REQUIRE(m.toUci() == "e2e4");

    Move p = Move::fromUci("e7e8q");
    REQUIRE(p.promotion == PieceType::Queen);
    REQUIRE(p.toUci() == "e7e8q");

    Move r = Move::fromUci("e7e8r");
    REQUIRE(r.promotion == PieceType::Rook);
}

static void test_move_invalid_uci_throws() {
    REQUIRE_THROWS_AS(Move::fromUci("e2e4x"), InvalidSquare);
}
