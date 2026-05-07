#include "../declarations.hpp"

struct serialize {
    static void save(const Game& game, WriteStream& stream);
    static void open(Game& game, ReadStream& stream);
};
