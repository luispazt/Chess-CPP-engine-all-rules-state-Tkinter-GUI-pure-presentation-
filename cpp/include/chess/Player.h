#pragma once

#include "chess/Game.h"
#include "chess/Move.h"

namespace chess {

class Player {
public:
    virtual ~Player() = default;
    virtual Move chooseMove(const Game& game) = 0;
};

class HumanPlayer : public Player {
public:
    Move chooseMove(const Game&) override;
};

class RandomAI : public Player {
public:
    Move chooseMove(const Game& g) override;
};

}


