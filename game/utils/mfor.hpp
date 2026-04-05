#define mfor(array, length, i)\
    for (auto *i = (array), *const i##_end = i + length; i < i##_end; i++)
