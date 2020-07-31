#ifndef CHESS_H
#define CHESS_H
#include "point.h"
#include "player.h"

/*
 * BaseChess class:
 *     A virtual chess class, base of the following 3 class: ClassicChess, QuantumChess, SuperpoChess
 *
 * Objects:
 * id_:
 *     the drop order of the chess, start at 1
 * pos_:
 *     the relative coordinate of the chess, from (0, 0) to (size - 1, size - 1)
 * player_:
 *     the player of the chess, WHITE or BLACK
 * chess_weight_:
 *     represents of the chance to appear, default 1
 *
 * Methods:
 * operator QString():
 *     type conversion
 */

class BaseChess {
public:
    BaseChess(const int &id, /* relative coordinates */ const point &pos, const Player &player, const int &chess_weight);
    virtual ~BaseChess();
    int id() const;
    point pos() const;
    Player player() const;
    int chess_weight() const;
    explicit operator QString() const;
private:
    int id_;
    point pos_;
    Player player_;
    int chess_weight_;
};

/*
 * ClassicChess class:
 *     A vanilla chess class, appears in original go games.
 */

class ClassicChess : public BaseChess {
public:
    ClassicChess(const int &id, const point &pos, const Player &player, const int &chess_weight);
    ~ClassicChess();
};

/*
 * QuantumChess class:
 *     a new chess in this game, it split the game into 2 parts,
 *     and appears in different place in both parts
 *
 * Objects:
 * dual_:
 *     its dual chess, appears in a different chessboard and take different place
 * alive_:
 *     true if the place has never been influenced by any gate effect
 *
 * WARNING: a quantum chess is able to be entangled by a superpo chess only if it is alive
 */

class QuantumChess : public BaseChess {
public:
    QuantumChess(const int &id, const point &pos, const Player &player, const int &chess_weight);
    ~QuantumChess();
    void set_dual(QuantumChess *dual);
    bool alive() const;
    void Kill();
    QuantumChess *dual() const;
private:
    QuantumChess *dual_;
    bool alive_;
};

/*
 * SuperpoChess class:
 *     a new chess in this game, it entangles a (pair of) chess;
 *     that is, it appears in the chessboard that its parent(a quantum chess) exists.
 *
 * WARNING: ONE SHOULD CHECK IF ITS PARENT IS ALIVE
 */

class SuperpoChess : public BaseChess {
public:
    SuperpoChess(const int &id, const point &pos, const Player &player, const int &chess_weight, QuantumChess const *parent);
    ~SuperpoChess();
    QuantumChess const *parent() const;
private:
    QuantumChess const *parent_;
};

#endif // CHESS_H
