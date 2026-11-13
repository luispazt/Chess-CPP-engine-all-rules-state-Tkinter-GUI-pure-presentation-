#include "test.h"
#include "test_square.cpp"
#include "test_pieces.cpp"
#include "test_board.cpp"
#include "test_game.cpp"
#include "test_perft.cpp"

int main() {
    std::cout << "square:" << std::endl;
    RUN_TEST(test_square_round_trip);
    RUN_TEST(test_square_invalid_throws);
    RUN_TEST(test_move_uci_parsing);
    RUN_TEST(test_move_invalid_uci_throws);

    std::cout << "pieces:" << std::endl;
    RUN_TEST(test_pieces_pawn_moves);
    RUN_TEST(test_pieces_knight_moves);
    RUN_TEST(test_pieces_bishop_moves);
    RUN_TEST(test_pieces_rook_moves);
    RUN_TEST(test_pieces_queen_moves);
    RUN_TEST(test_pieces_king_moves);

    std::cout << "board:" << std::endl;
    RUN_TEST(test_board_clone);
    RUN_TEST(test_board_find_king);
    RUN_TEST(test_board_set_take);
    RUN_TEST(test_board_is_attacked_by);
    RUN_TEST(test_board_from_fen);

    std::cout << "game:" << std::endl;
    RUN_TEST(test_game_initial_state);
    RUN_TEST(test_game_try_move);
    RUN_TEST(test_game_undo);
    RUN_TEST(test_game_legal_moves_from);
    RUN_TEST(test_game_fools_mate_checkmate);
    RUN_TEST(test_game_enpassant);
    RUN_TEST(test_game_en_passant_undo);

    std::cout << "perft:" << std::endl;
    RUN_TEST(test_perft_depths);

    std::cout << g_tests_passed << "/" << (g_tests_passed + g_tests_failed) << " tests passed" << std::endl;
    return g_tests_failed == 0 ? 0 : 1;
}
