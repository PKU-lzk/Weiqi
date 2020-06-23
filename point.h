#ifndef POINT_H
#define POINT_H
#include <cmath>
/*
 * point class:
 * 2d-point in chessboard or coordinate in window,
 * with natural operator
 */

class point
{
private:
    int x_, y_;
public:
    point(const int &x, const int &y);
    int x() const;
    int y() const;
    void set_x(const int &x);
    void set_y(const int &y);
    bool operator==(const point &other) const;
    bool operator<(const point &other) const;
    bool operator>(const point &other) const;
    point operator+(const point &other) const;
    point operator-(const point &other) const;
    point operator*(const int &n) const;
    point operator%(const int &n) const;
    point operator/(const int &n) const;
};

#endif // POINT_H
