#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "chess/Game.h"
#include "chess/Color.h"
#include "chess/Board.h"
#include "chess/Piece.h"
#include "chess/Square.h"

namespace py = pybind11;
using namespace chess;

PYBIND11_MODULE(chess_core, m) {
    m.doc() = "Chess rules engine";

    py::class_<Game>(m, "Game")
        .def(py::init<>())
        .def("reset", &Game::reset)
        .def("undo", &Game::undo)
        .def("try_move", &Game::tryMoveFromUci, py::arg("uci"))
        .def("legal_moves_from", &Game::legalMovesFromUci, py::arg("square"))
        .def("all_legal_moves", [](const Game& g) {
            std::vector<std::string> result;
            for (const Move& m : g.allLegalMoves()) {
                result.push_back(m.toUci());
            }
            return result;
        })
        .def("side_to_move", [](const Game& g) {
            return g.sideToMove() == Color::White ? std::string("white") : std::string("black");
        })
        .def("in_check", [](const Game& g) {
            return g.isInCheck(g.sideToMove());
        })
        .def("material_score", [](const Game& g) {
            int score = 0;
            const Board& board = g.board();
            for (int i = 0; i < 64; ++i) {
                Piece* p = board.at(Square{i});
                if (p) {
                    int v = p->value();
                    score += (p->color() == Color::White) ? v : -v;
                }
            }
            return score;
        })
        .def("state_json", &Game::toJson, "Full board state as JSON string");
}
