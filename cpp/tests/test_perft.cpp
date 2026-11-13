#include "test.h"
#include "chess/Game.h"

using namespace chess;

static int perft(Game& g, int depth) {
    if (depth == 0) return 1;
    int nodes = 0;
    for (const Move& m : g.allLegalMoves()) {
        REQUIRE(g.tryMove(m));
        nodes += perft(g, depth - 1);
        g.undo();
    }
    return nodes;
}

static void test_perft_depths() {
    Game g;
    REQUIRE(perft(g, 1) == 20);
    REQUIRE(perft(g, 2) == 400);
    REQUIRE(perft(g, 3) == 8902);
    REQUIRE(perft(g, 4) == 197281);
}
