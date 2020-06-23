#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H
#include "game.h"
#include <QMainWindow>
#include <QKeyEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>

enum MapDisplayMode {ONEBOARD, GLOBAL};

const int board_size = 19;
const MapDisplayMode default_display_mode = GLOBAL;

//const char * color_background = "#B1723C";
//const char * color_boundary = "#B8D582";
//const char * color_board = "#EEC085";
//const char * color_black = "#000000";
//const char * color_white = "#FFFFFF";
//const char * color_red = "#FF0000";

#define color_background "#B1723C"
#define color_boundary "#8D5822"
#define color_board "#EEC085"
#define color_black "#000000"
#define color_white "#FFFFFF"
#define color_red "#FF0000"

struct Size {
    int board_w;
    int bound_w;
    int window_w;
    int window_h;
    int board_x;
    int board_y;
};

const std::vector<Size> game_size = {
    {-1, -1, -1, -1, -1, -1},
    {760, 20, 800, 800, 1, 1},
    {760, 20, 1600, 800, 2, 1},
    {570, 15, 1800, 600, 3, 1},
    {494, 13, 1040, 1040, 2, 2},
    {-1, -1, -1, -1, -1, -1},
    {496, 13, 1560, 1040, 3, 2},
    {-1, -1, -1, -1, -1, -1},
    {380, 10, 1600, 800, 4, 2}
};

/*
 * GameWindow class:
 *
 * Objects:
 * absolute_coordinate_:
 *     the coordinate of mouse
 * window_size_:
 *     current size of window(w, h)
 *
 * Methods:
 * StartGame():
 *     activate when construct GameWindow, start an empty chessboard(group)
 * get_relative_coordinate(chessboardgroup_size, absolute_coordinate):
 *     using window_size_ to determine the graphic and location of each chessboard,
 *     return the chessboard id & relative coordinate
 *     DEFAULT (-1, -1)
 * get_absolute_coordinate(chessboardgroup_size, id, relative_coordinate):
 *     inverse function of get_relative_coordinate, return the center of the point
 *
 * PS:
 * chessboardgroup map in MapDisplayMode::GLOBAL:
 *
 * | 1 | 2 |  3  | 4 |  6  |   8   |
 *   *  **   ***  **   ***   ****
 *                **   ***   ****
 */

namespace Ui {
class GameWindow;
}

class GameWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GameWindow(QWidget *parent = nullptr);
    ~GameWindow();

    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

    void StartGame();
	int get_chessboard_count();
	void set_window_size();
    std::pair<int, point> get_relative_coordinate(const int &display_chessboard_count, const point &absolute_coordinate);
    point get_absolute_coordinate(const int &display_chessboard_count, const int &id, const point &relative_coordinate);
    void DrawMap(ChessBoardGroup *group);
    void DrawChess(const int &display_chesssboard_count, const int &id, BaseChess *chess, QPainter *painter);
private:
    Ui::GameWindow *ui;
    Game *game_;
    point absolute_coordinate_;
    MapDisplayMode mapdisplaymode_;
    point window_size_;
};

#endif // GAMEWINDOW_H
