#pragma once

namespace chess {

enum class GameStatus {
    InProgress,
    Check,
    Checkmate,
    Stalemate,
    DrawFiftyMove,
    DrawInsufficientMaterial,
    DrawThreefold
};

}


