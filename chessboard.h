#ifndef CHESSBOARD_H
#define CHESSBOARD_H
#include "chess.h"
#include <vector>

/*
 * ChessBoard class:
 *     A 2d-board, containing a square of chess(pointer), with weight(probability)
 *
 * Objects:
 * board_size_:
 *     count of chess in one line, default 19
 * board_weight_:
 *     represents of the chance to appear, default 1
 *
 * Methods:
 * ChessDead(x, y):
 *     true if the chess(pointer) board[x][y] is dead(has no qi)
 * ChessDead(pos):
 *     same as above
 * ChessMatch(chess):
 *     true if the dropped chess will appear in the chessboard in next turn
 */

class ChessBoardGroup;
class Game;
class GameWindow;

class ChessBoard {
public:
    ChessBoard(const int &board_size, const int &board_weight);
    ~ChessBoard();
    bool ChessDead(const int &x, const int &y) const;
    bool ChessDead(const point &pos) const;
    bool ChessMatch(const BaseChess *chess) const;
private:
    const int board_size_;
    int board_weight_;
    std::vector<std::vector<BaseChess *>> board_;
    friend class ChessBoardGroup;
    friend class Game;
    friend class GameWindow;
};

#endif // CHESSBOARD_H
