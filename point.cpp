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

point point::strip() const {
    std::function<int(int, int)> gcd = [&](int x, int y) {return y ? gcd(y, x % y) : x;};
    int t = gcd(x_, y_);
    if (t)
        return point(x_ / t, y_ / t);
    else
        return *this;
}

point::operator QString() const {return QString(std::to_string(x_).data()) + '/' + std::to_string(y_).data(); }
