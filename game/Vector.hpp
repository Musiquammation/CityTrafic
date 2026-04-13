#pragma once

template<typename T>
struct Vector {
    T x;
    T y;

    bool operator==(const Vector<T>& op) const noexcept {
        return this->x == op.x && this->y == op.y;
    }
};


