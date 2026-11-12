#include "chess/Player.h"
#include <random>

namespace chess {

Move HumanPlayer::chooseMove(const Game&) {
    throw std::logic_error("human move via GUI");
}

Move RandomAI::chooseMove(const Game& g) {
    std::vector<Move> moves = g.allLegalMoves();
    if (moves.empty()) throw std::logic_error("no legal moves");
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<size_t> dist(0, moves.size() - 1);
    return moves[dist(rng)];
}

}


