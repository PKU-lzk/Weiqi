#include "frac.h"

frac::frac(const int &x, const int &y) : x_(x), y_(y) {}
int frac::x() const { return x_; }
int frac::y() const { return y_; }
void frac::set_x(const int &x) { x_ = x; }
void frac::set_y(const int &y) { y_ = y; }
bool frac::operator==(const frac &other) const { return x_ * other.y_ == y_ * other.x_; }
bool frac::operator<(const frac &other) const { return x_ * other.y_ < y_ * other.x_; }
bool frac::operator>(const frac &other) const { return x_ * other.y_ > y_ * other.x_; }
frac frac::operator+(const frac &other) const { return frac(x_ * other.y_ + y_ * other.x_, y_ * other.y_); }
frac frac::operator-(const frac &other) const { return frac(x_ * other.y_ - y_ * other.x_, y_ * other.y_); }
frac frac::operator*(const frac &other) const { return frac(x_ * other.x_, y_ * other.y_); }
frac& frac::Reduce() {
    std::function<int(int, int)> gcd = [&](int x, int y) { return y ? gcd(y, x % y) : x; };
    int t = gcd(x_, y_);
    x_ /= t, y_ /= t;
    return *this;
}
frac::operator QString() const { return QString(std::to_string(x_).data()) + '/' + std::to_string(y_).data(); }
