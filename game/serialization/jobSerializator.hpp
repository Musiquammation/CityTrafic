#pragma  once

struct ReadStream;
struct WriteStream;
class Job;

struct jobSerializator {
    static void save(WriteStream& stream, const Job *job);
    static Job *load(ReadStream& stream);
};

