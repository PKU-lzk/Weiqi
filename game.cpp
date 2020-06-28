#include "game.h"

Game::Game(GameWindow *parent, const int &board_size, const TakeMode &takemode)
    : parent_(parent), board_size_(board_size), takemode_(takemode) {
    current_round_id_ = 0;
    current_round_player_ = Player::BLACK;
    round_.push_back(new ChessBoardGroup(board_size_));
}

Game::~Game() {
    for (auto group : round_)
        delete group;
    for (auto chess : chessgroup_)
        delete chess;
    round_.clear();
    chessgroup_.clear();
}

void Game::Click(const point &pos) {
    const int &id = current_round_id_;
    const Player &player = current_round_player_;
    auto round = round_[id];
    if (round->drop_mode_ == DropMode::CLASSIC) {
        if (round->ValidDrop(pos)) {
            BaseChess * chess = new ClassicChess(id + 1, pos, player, 1);
            chessgroup_.push_back(chess);
            round->DropChess(chess);
            return;
        }
    }
    if (round->drop_mode_ == DropMode::QUANTUM) {
        if (round->ValidDrop(pos)) {
            BaseChess *chess = new QuantumChess(id + 1, pos, player, 1);
            chessgroup_.push_back(chess);
            round->DropChess(chess);
            return;
        } else if (round->ValidChoose(pos)) {
            round->ChooseChess(pos);
            return;
        }
    }
    if (round->drop_mode_ == DropMode::SUPERPO) {
        if (round->ValidDrop(pos)) {
            auto ratio = round->superpo_ratio_;
            auto parent = round->last_entangled_chess_;
            int weight = static_cast<int>(round->dropped_chess_group_.size()) < ratio.first ? ratio.second : ratio.first;
            BaseChess *chess = new SuperpoChess(id + 1, pos, player, weight, parent);
            chessgroup_.push_back(chess);
            round->DropChess(chess);
            return;
        } else if (round->ValidChoose(pos)) {
            auto board = round->chessboard_group_[round->activate_chessboard_index_];
            auto chess = board->board_[pos.x()][pos.y()];
            round->last_entangled_chess_ = dynamic_cast<QuantumChess *>(chess);
        }
    }
    qInfo("unknown error");
    return;
}

bool Game::FinishRound() {
    auto round = round_[current_round_id_];
    if (round->drop_mode_ == DropMode::CLASSIC)
        return round->dropped_chess_group_.size() == 1;
    if (round->drop_mode_ == DropMode::QUANTUM)
        return round->dropped_chess_group_.size() == 2;
    if (round->drop_mode_ == DropMode::SUPERPO)
        return static_cast<int>(round->dropped_chess_group_.size()) == round->superpo_ratio_.first + round->superpo_ratio_.second;
    qInfo("known error");
    return false;
}

void Game::PrevRound() {
    round_.pop_back();
    --current_round_id_;
    if (current_round_player_ == Player::WHITE)
        current_round_player_ = Player::BLACK;
    else
        current_round_player_ = Player::WHITE;
    round_.back()->reset();
}

void Game::NextRound() {
    round_.push_back(new ChessBoardGroup(round_.back(), takemode_));
    ++current_round_id_;
    if (current_round_player_ == Player::WHITE)
        current_round_player_ = Player::BLACK;
    else
        current_round_player_ = Player::WHITE;
    qInfo(
                ("current round: "
                 + std::to_string(current_round_id_ + 1)
                 + ", player: "
                 + (current_round_player_ == Player::WHITE ? "WHITE" : "BLACK")
                 ).data()
                );
}
