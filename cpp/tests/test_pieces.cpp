#include "test.h"
#include "chess/Board.h"
#include "chess/Piece.h"
#include "chess/Pawn.h"
#include "chess/Knight.h"
#include "chess/Bishop.h"
#include "chess/Rook.h"
#include "chess/Queen.h"
#include "chess/King.h"

using namespace chess;

static void test_pieces_pawn_moves() {
    Board b = Board::fromFen("8/8/8/8/8/8/8/8");
    b.set(Square::fromString("e4"), std::make_unique<Pawn>(Color::White));
    b.set(Square::fromString("d5"), std::make_unique<Pawn>(Color::Black));
    b.set(Square::fromString("f5"), std::make_unique<Pawn>(Color::Black));

    auto moves = b.at(Square::fromString("e4"))->pseudoLegalMoves(b, Square::fromString("e4"));
    bool hasD5 = false, hasF5 = false;
    for (const Move& m : moves) {
        if (m.to == Square::fromString("d5")) hasD5 = true;
        if (m.to == Square::fromString("f5")) hasF5 = true;
    }
    REQUIRE(hasD5);
    REQUIRE(hasF5);
}

static void test_pieces_knight_moves() {
    Board b = Board::fromFen("8/8/8/8/8/8/8/8");
    b.set(Square::fromString("e4"), std::make_unique<Knight>(Color::White));

    auto moves = b.at(Square::fromString("e4"))->pseudoLegalMoves(b, Square::fromString("e4"));
    REQUIRE(moves.size() == 8);
}

static void test_pieces_bishop_moves() {
    Board b = Board::fromFen("8/8/8/8/8/8/8/8");
    b.set(Square::fromString("e4"), std::make_unique<Bishop>(Color::White));

    auto moves = b.at(Square::fromString("e4"))->pseudoLegalMoves(b, Square::fromString("e4"));
    REQUIRE(moves.size() == 13);
}

static void test_pieces_rook_moves() {
    Board b = Board::fromFen("8/8/8/8/8/8/8/8");
    b.set(Square::fromString("e4"), std::make_unique<Rook>(Color::White));

    auto moves = b.at(Square::fromString("e4"))->pseudoLegalMoves(b, Square::fromString("e4"));
    REQUIRE(moves.size() == 14);
}

static void test_pieces_queen_moves() {
    Board b = Board::fromFen("8/8/8/8/8/8/8/8");
    b.set(Square::fromString("e4"), std::make_unique<Queen>(Color::White));

    auto moves = b.at(Square::fromString("e4"))->pseudoLegalMoves(b, Square::fromString("e4"));
    REQUIRE(moves.size() == 27);
}

static void test_pieces_king_moves() {
    Board b = Board::fromFen("8/8/8/8/8/8/8/8");
    b.set(Square::fromString("e4"), std::make_unique<King>(Color::White));

    auto moves = b.at(Square::fromString("e4"))->pseudoLegalMoves(b, Square::fromString("e4"));
    REQUIRE(moves.size() == 8);
}
