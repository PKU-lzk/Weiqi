#ifndef CHESSBOARDGROUP_H
#define CHESSBOARDGROUP_H
#include "chessboard.h"
#include <set>

/*
 * ChessBoardGroup class:
 *     control ChessBoard objects in one turn of this game
 *
 * Objects:
 * board_size_:
 *     count of chess in a line, default 19
 * current_player_:
 *     the player to drop chess
 * chessboard_group_:
 *     group of all possible chessboard
 * activate_chessboard_index_:
 *     1. under one-board mode, the board to be displayed, //affected by last turn
 *     2. under global    mode, the board the mouse located
 * drop_mode_:
 *     the chess class the player wanted to drop, affected by last turn
 * last_entangled_chess_:
 *     if drop_mode_ = SUPERPO, the entangled quantum chess, default nullptr
 * dropped_chess_group_:
 *     the chess that will be dropped in this turn
 * superpo_ratio_:
 *     if drop_mode_ = SUPERPO, the ratio of chess that will be dropped in this turn, 1 : 1 or 1 : 2 or 2 : 2
 *
 * Methods:
 * ChessBoardGroup(board_size, reverse):
 *     generate the first chessboardgroup, BLACK first if reverse = false
 * ChessBoardGroup(previous_group, takemode):
 *     succeed the group from previous one, takemode represents the eat method
 * ValidChoose(pos):
 *     if drop_mode_ = QUANTUM, true if it select a place satisfying the following conditions:
 *         1. the place has a quantum chess C
 *         2. C.player != current_player_
 * ValidDrop(pos):
 *     true if it select a place that is empty for all $relative-chessboard
 *     $relative-chessboard:
 *         1. drop_mode_ = CLASSIC:
 *             all chessboard
 *         2. drop_mode_ = QUANTUM:
 *             all chessboard
 *             not last chess
 *         3. drop_mode_ = SUPERPO:
 *             the chessboard its parent exists
 * ScrollBoard(reverse):
 *     under one-board mode
 *     1. reverse = 0:
 *         ++activate_chessboard_index_
 *     2. reverse = 1:
 *         --activate_chessboard_index_
 * reset():
 *     remove all dropped, choose chess in this turn
 */

enum DropMode {CLASSIC, QUANTUM, SUPERPO};
enum TakeMode {NONE, NORMAL, DEFAULT, TAKEALL};

class Game;
class GameWindow;

class ChessBoardGroup
{
public:
    ChessBoardGroup(const int &board_size, bool reverse = false);
    ChessBoardGroup(ChessBoardGroup const *previous_group, const TakeMode &takemode);
    ~ChessBoardGroup();
    bool ValidChoose(const point &pos) const;
    bool ValidDrop(const point &pos) const;
    void ScrollBoard(bool reverse = false);
    void set_activate_chessboard_index(const int &activate_chessboard_index);
    void set_drop_mode(const DropMode &drop_mode);
    void set_superpo_ratio(const std::pair<int, int> &superpo_ratio);
    void reset();
    void ChooseChess(const point &pos);
    void DropChess(BaseChess *chess);
private:
    void Relax();
    void GroupGenerate(ChessBoardGroup const *previous_group, const TakeMode &takemode);
    const int board_size_;
    Player current_player_;
    std::vector<ChessBoard *> chessboard_group_;
    int activate_chessboard_index_;
    int group_weight_;
    DropMode drop_mode_;
    QuantumChess *last_entangled_chess_;
    std::vector<BaseChess *> dropped_chess_group_;
    std::pair<int, int> superpo_ratio_;
    friend class Game;
    friend class GameWindow;
};

#endif // CHESSBOARDGROUP_H
