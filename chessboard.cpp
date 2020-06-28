#include "chessboard.h"
#include <queue>

ChessBoard::ChessBoard(const int &board_size, const int &board_weight)
    : board_size_(board_size), board_weight_(board_weight) {
    std::vector<BaseChess *> tmp;
    for (int i = 0; i < board_size_; ++i)
        tmp.push_back(nullptr);
    for (int i = 0; i < board_size_; ++i)
        board_.push_back(tmp);
}

ChessBoard::~ChessBoard() {board_.clear();}

/*
 * Using BFS
 */

bool ChessBoard::ChessDead(const int &x, const int &y) const {
    const int &size = board_size_;
    auto OutOfRange = [size](int u, int v) {return (u < 0 || u >= size || v < 0 || v >= size);};
#ifdef QT_DEBUG
    if (OutOfRange(x, y))
        qFatal("invalid coordinate: out of range");
//    if (board_[x][y] == nullptr)
//        qFatal("invalid coordinate: empty");
#endif
    if (board_[x][y] == nullptr)
        return false;
    bool visit[board_size_][board_size_];
    memset(visit, 0, sizeof(visit));
    std::queue<point> q;
    q.push(point(x, y));
    visit[x][y] = true;
    point dir[4] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    while (!q.empty()) {
        point p = q.front();
        q.pop();
        for (int i = 0; i < 4; ++i) {
            point t = p + dir[i];
            if (OutOfRange(t.x(), t.y())) continue;
            if (visit[t.x()][t.y()]) continue;
            visit[t.x()][t.y()] = true;
            if (board_[t.x()][t.y()] == nullptr)
                return false;
            if (board_[t.x()][t.y()]->player() == board_[x][y]->player())
                q.push(t);
        }
    }
    return true;
}

bool ChessBoard::ChessDead(const point &pos) const {return ChessDead(pos.x(), pos.y());}

bool ChessBoard::ChessMatch(const BaseChess *chess) const {
    if (typeid(*chess) == typeid(ClassicChess) || typeid(*chess) == typeid(QuantumChess))
        return true;
    if (typeid(*chess) == typeid(SuperpoChess)) {
        auto super_chess = dynamic_cast<const SuperpoChess*>(chess);
        auto parent = super_chess->parent();
        return board_[parent->pos().x()][parent->pos().y()] == parent;
    }
#ifdef QT_DEBUG
    qFatal("unknown error");
    return false;
#endif
}
