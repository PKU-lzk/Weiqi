#include "gamewindow.h"
#include "ui_gamewindow.h"

GameWindow::GameWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::GameWindow),
    absolute_coordinate_(-1, -1), mapdisplaymode_(MapDisplayMode::GLOBAL), window_size_(-1, -1)
{
    ui->setupUi(this);
    StartGame();
    set_window_size();
}

GameWindow::~GameWindow() {
    delete game_;
    delete ui;
}

void GameWindow::paintEvent(QPaintEvent *event) {
    QPalette palette;
    palette.setColor(QPalette::Background, color_background);
    setPalette(palette);

    centralWidget()->setMouseTracking(true);
    setMouseTracking(true);

    DrawMap(game_->round_[game_->current_round_id_]);
}

void GameWindow::mouseMoveEvent(QMouseEvent *event) {
    absolute_coordinate_ = point(event->x(), event->y());
    auto relative_coord = get_relative_coordinate(get_chessboard_count(), absolute_coordinate_);
    if (relative_coord.first < 0 || relative_coord.first >= get_chessboard_count())
        return;
    if (relative_coord.second.x() >= board_size || relative_coord.second.y() >= board_size)
        return;
    update();
}

void GameWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        auto relative_coord = get_relative_coordinate(get_chessboard_count(), absolute_coordinate_);
        game_->round_[game_->current_round_id_]->set_activate_chessboard_index(relative_coord.first);
        game_->Click(relative_coord.second);
        if (game_->FinishRound())
            game_->NextRound(), update();
    }
}

void GameWindow::keyPressEvent(QKeyEvent *event) {}

void GameWindow::StartGame() {
    game_ = new Game(board_size, TakeMode::DEFAULT);
}

int GameWindow::get_chessboard_count() {return game_->round_[game_->current_round_id_]->chessboard_group_.size();}

void GameWindow::set_window_size() {
    int display_chessboard_count = (mapdisplaymode_ == MapDisplayMode::ONEBOARD) ? 1 : get_chessboard_count();
    setFixedSize(game_size[display_chessboard_count].window_w, game_size[display_chessboard_count].window_h + 25);
}

std::pair<int, point> GameWindow::get_relative_coordinate(const int &display_chessboard_count, const point &absolute_coordinate) {
    const int count = display_chessboard_count;
    const int board_w = game_size[count].board_w;
    const int block_w = board_w / board_size;
    const int bound_w = game_size[count].bound_w;
    const point coord = absolute_coordinate;
    const int total_w = board_w + 2 * bound_w;
    const int loc_x = coord.x() / total_w;
    const int loc_y = coord.y() / total_w;
    const int id = loc_y * game_size[count].board_x + loc_x;
    point pos_in_board = (coord % total_w - point(bound_w, bound_w)) / block_w;
    return std::make_pair(id, pos_in_board);
}

point GameWindow::get_absolute_coordinate(const int &display_chessboard_count, const int &id, const point &relative_coordinate) {
    const int count = display_chessboard_count;
    const int loc_x = id % game_size[count].board_x;
    const int loc_y = id / game_size[count].board_y;
    const int board_w = game_size[count].board_w;
    const int bound_w = game_size[count].bound_w;
    const int total_w = board_w + 2 * bound_w;
    point pos_in_board = (relative_coordinate * board_w + point(board_w / 2, board_w / 2)) / board_size;
    return point(total_w, 0) * loc_x + point(0, total_w) * loc_y + point(bound_w, bound_w) + pos_in_board;
}

void GameWindow::DrawMap(ChessBoardGroup *group) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPen pen = painter.pen();
    pen.setColor(color_boundary);
    pen.setWidth(5);
    painter.setPen(pen);
    int count = group->chessboard_group_.size();
    if (mapdisplaymode_ == MapDisplayMode::GLOBAL) {
        QBrush brush;
        brush.setColor(color_board);
        brush.setStyle(Qt::SolidPattern);
        painter.setBrush(brush);
        int board_w = game_size[count].board_w, bound_w = game_size[count].bound_w;
        int block_w = board_w / board_size;
        int total_w = board_w + 2 * bound_w;
        for (int i = 0; i < count; ++i) {
            int loc_x = i % game_size[count].board_x;
            int loc_y = i / game_size[count].board_y;
            point start_point = point(total_w, 0) * loc_x + point(0, total_w) * loc_y + point(bound_w, bound_w);
            painter.drawRect(start_point.x(), start_point.y(), board_w, board_w);
            /* draw lines */
            pen.setColor(color_black);
            pen.setWidth(1);
            painter.setPen(pen);
            for (int j = 0; j < board_size; ++j) {
                point p1 = get_absolute_coordinate(count, i, {0, j});
                point p2 = get_absolute_coordinate(count, i, {board_size - 1, j});
                point q1 = get_absolute_coordinate(count, i, {j, 0});
                point q2 = get_absolute_coordinate(count, i, {j, board_size - 1});
                painter.drawLine(p1.x(), p1.y(), p2.x(), p2.y());
                painter.drawLine(q1.x(), q1.y(), q2.x(), q2.y());
            }
            /* draw black points */
            brush.setColor(color_black);
            painter.setBrush(brush);
            std::vector<point> black_points = {point(3, 3), point(9, 9), point(3, 15), point(15, 3), point(15, 15)};
            for (point b : black_points) {
                point p = get_absolute_coordinate(count, i, b);
                int half_w = block_w / 8;
                painter.drawRect(p.x() - half_w, p.y() - half_w, half_w * 2, half_w * 2);
            }
            /* draw chess */
            auto chessboard = game_->round_[game_->current_round_id_]->chessboard_group_[i];
            for (int j = 0; j < board_size; ++j)
                for (int k = 0; k < board_size; ++k) {
                    auto chess = chessboard->board_[j][k];
                    if (chess != nullptr)
                        DrawChess(count, i, chess, &painter);
                }
            auto dropped = game_->round_[game_->current_round_id_]->dropped_chess_group_;
            for (auto chess : dropped)
                if (chessboard->ChessMatch(chess)) DrawChess(count, i, chess, &painter);
        }
        /* draw red rectangle */
        pen.setColor(color_red);
        pen.setWidth(1);
        painter.setPen(pen);
        auto relative_coord = get_relative_coordinate(count, absolute_coordinate_);
        point p = get_absolute_coordinate(count, relative_coord.first, relative_coord.second);
        int half_w = block_w / 2;
        for (int i = -half_w; i <= half_w; i += half_w * 2)
            for (int j = -half_w; j <= half_w; j += half_w * 2) {
                painter.drawLine(p.x() + i, p.y() + j, p.x() + i / 2, p.y() + j);
                painter.drawLine(p.x() + i, p.y() + j, p.x() + i, p.y() + j / 2);
            }
    }
}

void GameWindow::DrawChess(const int &display_chessboard_count, const int &id, BaseChess *chess, QPainter *painter) {
    int count = display_chessboard_count;
    auto absolute_coord = get_absolute_coordinate(count, id, chess->pos());
    int block_w = game_size[count].board_w / board_size;
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    if (chess->player() == Player::WHITE) brush.setColor(color_white), painter->setPen(color_white);
    if (chess->player() == Player::BLACK) brush.setColor(color_black), painter->setPen(color_black);
    painter->setBrush(brush);
    painter->drawEllipse(QPoint(absolute_coord.x(), absolute_coord.y()), block_w / 3, block_w / 3);
    if (chess->player() == Player::WHITE) painter->setPen(color_black);
    if (chess->player() == Player::BLACK) painter->setPen(color_white);
    QFont font;
    font.setPixelSize(game_size[count].board_w / 50);
    int half_w = game_size[count].board_w / 40;
    painter->drawText(QRectF(absolute_coord.x() - half_w, absolute_coord.y() - half_w, 2 * half_w, 2 * half_w),
                      Qt::AlignCenter, static_cast<QString>(*chess));
}
