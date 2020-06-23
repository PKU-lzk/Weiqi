#include "point.h"

point::point(const int &x, const int &y) : x_(x), y_(y) {}

int point::x() const {return x_;}

int point::y() const {return y_;}

void point::set_x(const int &x) {x_ = x;}

void point::set_y(const int &y) {y_ = y;}

bool point::operator==(const point &other) const {return x_ == other.x_ && y_ == other.y_; }

bool point::operator<(const point &other) const {return (x_ < other.x_) || (x_ == other.x_ && y_ < other.y_); }

bool point::operator>(const point &other) const {return (other.x_ < x_) || (other.x_ == x_ && other.y_ < y_); }

point point::operator+(const point &other) const {return point(x_ + other.x_, y_ + other.y_); }

point point::operator-(const point &other) const {return point(x_ - other.x_, y_ - other.y_); }

point point::operator*(const int &n) const {return point(x_ * n, y_ * n); }

point point::operator%(const int &n) const {return point(x_ % n, y_ % n); }

point point::operator/(const int &n) const {return point(x_ / n, y_ / n); }
