#include "chess.h"

BaseChess::BaseChess(const int &id, const point &pos, const Player &player, const int &chess_weight)
    : id_(id), pos_(pos), player_(player), chess_weight_(chess_weight) {}

BaseChess::~BaseChess() {}

int BaseChess::id() const {return id_;}

point BaseChess::pos() const {return pos_;}

Player BaseChess::player() const {return player_;}

int BaseChess::chess_weight() const {return chess_weight_;}

BaseChess::operator QString() const {
    QString res(std::to_string(id_).data());
    return res;
    if (typeid(*this) == typeid(ClassicChess)) return res + 'C';
    if (typeid(*this) == typeid(QuantumChess)) return res + 'Q';
    if (typeid(*this) == typeid(SuperpoChess)) return res + 'S';
#ifdef QT_DEBUG
    qFatal("invalid chess type");
#endif
    return QString();
}

ClassicChess::ClassicChess(const int &id, const point &pos, const Player &player, const int &chess_weight)
    : BaseChess(id, pos, player, chess_weight) {}

ClassicChess::~ClassicChess() {}

QuantumChess::QuantumChess(const int &id, const point &pos, const Player &player, const int &chess_weight)
    : BaseChess(id, pos, player, chess_weight), dual_(nullptr), alive_(true) {}

QuantumChess::~QuantumChess() {}

void QuantumChess::set_dual(QuantumChess *dual) {dual_ = dual;}

bool QuantumChess::alive() const {return alive_;}

void QuantumChess::Kill() {
    alive_ = false;
#ifdef QT_DEBUG
#include <string>
    std::string res = "kill";
    res = res + " " + std::to_string(pos().x()) + " " + std::to_string(pos().y());
    qInfo(res.c_str());
#endif
}

QuantumChess *QuantumChess::dual() const {return dual_;}

SuperpoChess::SuperpoChess(const int &id, const point &pos, const Player &player, const int &chess_weight, QuantumChess const *parent)
    : BaseChess(id, pos, player, chess_weight), parent_(parent) {
#ifdef QT_DEBUG
    if (!parent->alive())
        qFatal("linked to a dead quantum chess");
#endif
}

SuperpoChess::~SuperpoChess() {}

QuantumChess const *SuperpoChess::parent() const {return parent_;}
