#include "chessboardgroup.h"

ChessBoardGroup::ChessBoardGroup(const int &board_size, bool reverse)
    : board_size_(board_size) {
    if (reverse)
        current_player_ = WHITE;
    else
        current_player_ = BLACK;
    chessboard_group_.push_back(new ChessBoard(board_size_, 1));
    activate_chessboard_index_ = 0;
    drop_mode_ = DropMode::CLASSIC;
    last_entangled_chess_ = nullptr;
    superpo_ratio_ = {1, 1};
}

ChessBoardGroup::ChessBoardGroup(ChessBoardGroup const *previous_group, const TakeMode &takemode)
    : board_size_(previous_group->board_size_) {
    if (previous_group->current_player_ == Player::WHITE)
        current_player_ = Player::BLACK;
    else
        current_player_ = Player::WHITE;
    GroupGenerate(previous_group, takemode);
    activate_chessboard_index_ = 0;
    drop_mode_ = previous_group->drop_mode_;
    last_entangled_chess_ = nullptr;
    superpo_ratio_ = previous_group->superpo_ratio_;
}

ChessBoardGroup::~ChessBoardGroup() {
    chessboard_group_.clear();
    dropped_chess_group_.clear();
}

bool ChessBoardGroup::ValidChoose(const point &pos) const {
    auto chess = chessboard_group_[activate_chessboard_index_]->board_[pos.x()][pos.y()];
    if (chess == nullptr) {
        qInfo("invalid position: empty");
        return false;
    }
    if (typeid(*chess) != typeid(QuantumChess)) {
        qInfo("invalid position: not quantum chess");
        return false;
    }
    if (typeid(*chess) == typeid(QuantumChess)) {
        if (dropped_chess_group_.empty()) {
            if (last_entangled_chess_ == nullptr)
                qInfo("select successful");
            else
                qInfo("revise successful");
            return true;
        } else {
            qInfo("invalid position: a quantum chess has been chosen");
            return false;
        }
    }
    qInfo("unknown error");
    return false;
}

bool ChessBoardGroup::ValidDrop(const point &pos) const {
    if (drop_mode_ == DropMode::CLASSIC) {
        for (auto board : chessboard_group_)
            if (board->board_[pos.x()][pos.y()] != nullptr) {
                qInfo("invalid position: a chess has already been set");
                return false;
            }
        return true;
    }
    if (drop_mode_ == DropMode::QUANTUM) {
        for (auto board : chessboard_group_)
            if (board->board_[pos.x()][pos.y()] != nullptr) {
                qInfo("invalid position: a chess has already been set");
                return false;
            }
        for (auto prev_chess : dropped_chess_group_)
            if (prev_chess->pos() == pos) {
                qInfo("invalid position: a chess has already been set");
                return false;
            }
        return true;
    }
    if (drop_mode_ == DropMode::SUPERPO) {
        auto parent = last_entangled_chess_;
        if (last_entangled_chess_ == nullptr) {
            qInfo("entangled chess not set yet");
            return false;
        }
        for (auto board : chessboard_group_)
            if (board->board_[parent->pos().x()][parent->pos().y()] == parent && board->board_[pos.x()][pos.y()] != nullptr) {
                qInfo("a chess has already been set");
                return false;
            }
        for (auto prev_chess : dropped_chess_group_) {
            auto super_chess = dynamic_cast<SuperpoChess *>(prev_chess);
            if (super_chess->parent() == parent && super_chess->pos() == pos) {
                qInfo("a chess has already been set");
                return false;
            }
        }
        return true;
    }
    qInfo("unknown error");
    return false;
}

void ChessBoardGroup::ScrollBoard(bool reverse) {
    if (!reverse)
        (activate_chessboard_index_ += 1) %= chessboard_group_.size();
    else
        (activate_chessboard_index_ += (chessboard_group_.size() - 1)) %= chessboard_group_.size();
}

void ChessBoardGroup::set_activate_chessboard_index(const int &activate_chessboard_index) {
    activate_chessboard_index_ = activate_chessboard_index;
}

void ChessBoardGroup::set_drop_mode(const DropMode &drop_mode) {drop_mode_ = drop_mode;}

void ChessBoardGroup::set_superpo_ratio(const std::pair<int, int> &superpo_ratio) {superpo_ratio_ = superpo_ratio;}

void ChessBoardGroup::reset() {
    last_entangled_chess_ = nullptr;
    dropped_chess_group_.clear();
}

void ChessBoardGroup::ChooseChess(const point &pos) {
    last_entangled_chess_ = dynamic_cast<QuantumChess *>(chessboard_group_[activate_chessboard_index_]->board_[pos.x()][pos.y()]);
}

void ChessBoardGroup::DropChess(BaseChess *chess) {
    auto pos = chess->pos();
    dropped_chess_group_.push_back(chess);
    if (drop_mode_ == DropMode::QUANTUM && dropped_chess_group_.size() == 2) {
        auto first_chess = dynamic_cast<QuantumChess *>(dropped_chess_group_.front());
        auto second_chess = dynamic_cast<QuantumChess *>(dropped_chess_group_.back());
        first_chess->set_dual(second_chess);
        second_chess->set_dual(first_chess);
    }
    if (drop_mode_ == DropMode::SUPERPO && superpo_ratio_.first == static_cast<int>(dropped_chess_group_.size())) {
        last_entangled_chess_ = last_entangled_chess_->dual();
    }
}

/*
 * only called when construct the object
 */

void ChessBoardGroup::GroupGenerate(const ChessBoardGroup *previous_group, const TakeMode &takemode) {
    // generate group without take any chess
    DropMode dropmode = previous_group->drop_mode_;
    const auto &group = previous_group->chessboard_group_;
    const auto &dropped = previous_group->dropped_chess_group_;
    for (auto board : group)
        for (auto chess : dropped)
            if (board->ChessMatch(chess)) {
                auto new_board = new ChessBoard(*board);
                new_board->board_weight_ *= chess->chess_weight();
                new_board->board_[chess->pos().x()][chess->pos().y()] = chess;
                chessboard_group_.push_back(new_board);
            }
    // take chess
    if (takemode == TakeMode::NONE) return;
    if (takemode == TakeMode::NORMAL) {
        for (auto board : chessboard_group_) {
            const int &size = board_size_;
            std::vector<point> dead_chess_loc;
            for (int i = 0; i < size; ++i)
                for (int j = 0; j < size; ++j)
                    if (board->ChessDead(i, j))
                        dead_chess_loc.push_back({i, j});
            for (auto loc : dead_chess_loc) {
                auto chess = board->board_[loc.x()][loc.y()];
                if (typeid(*chess) == typeid(QuantumChess)) {
                    auto quantum_chess = dynamic_cast<QuantumChess *>(chess);
                    auto dual = quantum_chess->dual();
                    quantum_chess->Kill();
                    dual->Kill();
                }
                board->board_[loc.x()][loc.y()] = nullptr;
#ifdef QT_DEBUG
                qInfo("chess has been destroyed");
#endif
            }
        }
        return;
    }
    if (takemode == TakeMode::DEFAULT) {
        const int &size = board_size_;
        int gate[10][size][size];
        memset(gate, 0, sizeof(gate));
        std::vector<point> arr;
        for (int i = 0; i < size; ++i)
            for (int j = 0; j < size; ++j)
                arr.push_back({i, j});
        for (int i = 0; i < 10; ++i) {
            // find all gate
            for (auto board : chessboard_group_)
                for (auto pos : arr)
                    if (board->ChessDead(pos)) {
                        if (board->board_[pos.x()][pos.y()]->player() == Player::WHITE) gate[i][pos.x()][pos.y()] |= 1;
                        if (board->board_[pos.x()][pos.y()]->player() == Player::BLACK) gate[i][pos.x()][pos.y()] |= 2;
                    }
            bool change = false;
            for (auto pos : arr)
                change |= gate[i][pos.x()][pos.y()];
            if (!change)
                return;
            // find all quantum chess to be killed
            for (auto board : chessboard_group_)
                for (auto pos : arr)
                    if (gate[i][pos.x()][pos.y()] && board->board_[pos.x()][pos.y()] != nullptr
                            && typeid(*board->board_[pos.x()][pos.y()]) == typeid(QuantumChess)) {
                        auto quantum_chess = dynamic_cast<QuantumChess *>(board->board_[pos.x()][pos.y()]);
                        auto dual = quantum_chess->dual();
                        quantum_chess->Kill();
                        dual->Kill();
                    }
            // gate effect
            for (auto pos : arr) {
                int tag = gate[i][pos.x()][pos.y()];
                if (tag == 0) continue;
                if (tag == 1) {
                    // white gate
                    BaseChess *first = nullptr;
                    for (auto board : chessboard_group_)
                        if (board->board_[pos.x()][pos.y()] != nullptr && board->board_[pos.x()][pos.y()]->player() == Player::WHITE)
                            if (first == nullptr || first->id() > board->board_[pos.x()][pos.y()]->id()) first = board->board_[pos.x()][pos.y()];
                    for (auto board : chessboard_group_) {
                        if (board->board_[pos.x()][pos.y()] == nullptr)
                            board->board_[pos.x()][pos.y()] = first;
                        else if (board->board_[pos.x()][pos.y()]->player() == Player::WHITE)
                            board->board_[pos.x()][pos.y()] = nullptr;
                    }
                }
                if (tag == 2) {
                    // black gate
                    BaseChess *first = nullptr;
                    for (auto board : chessboard_group_)
                        if (board->board_[pos.x()][pos.y()] != nullptr && board->board_[pos.x()][pos.y()]->player() == Player::BLACK)
                            if (first == nullptr || first->id() > board->board_[pos.x()][pos.y()]->id()) first = board->board_[pos.x()][pos.y()];
                    for (auto board : chessboard_group_) {
                        if (board->board_[pos.x()][pos.y()] == nullptr)
                            board->board_[pos.x()][pos.y()] = first;
                        else if (board->board_[pos.x()][pos.y()]->player() == Player::BLACK)
                            board->board_[pos.x()][pos.y()] = nullptr;
                    }
                }
                if (tag == 3) {
                    // destroy gate
                    for (auto board : chessboard_group_) board->board_[pos.x()][pos.y()] = nullptr;
                }
            }
            if (i == 9) {
                // destroy last 2 gate
                for (int j = 8; j < 10; ++j)
                    for (auto pos : arr)
                        if (gate[j][pos.x()][pos.y()])
                            for (auto board : chessboard_group_)
                                board->board_[pos.x()][pos.y()] = nullptr;
            }
        }
    }
}

