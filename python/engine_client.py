import json
import chess_core


class EngineClient:
    def __init__(self):
        self._game = chess_core.Game()

    def state(self) -> dict:
        return json.loads(self._game.state_json())

    def legal_moves(self, square: str) -> list[str]:
        return self._game.legal_moves_from(square)

    def try_move(self, uci: str) -> bool:
        return self._game.try_move(uci)

    def undo(self) -> None:
        self._game.undo()

    def reset(self) -> None:
        self._game.reset()
