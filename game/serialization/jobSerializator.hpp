#pragma  once
#include <unordered_map>

class Character;
struct ReadStream;
struct WriteStream;
class Job;

struct jobSerializator {
    static void save(WriteStream& stream, const Job *job);
    static Job *open(ReadStream& stream, const std::unordered_map<Character*, Character*>& characterMap);
};

