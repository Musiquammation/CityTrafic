#pragma once

template<typename T>
struct Vector {
    T x;
    T y;

    bool operator==(const Vector<T>& op) const noexcept {
        return this->x == op.x && this->y == op.y;
    }

    bool operator<(const Vector<T>& o) const noexcept {
        return (x < o.x) || (x == o.x && y < o.y);
    }
};


