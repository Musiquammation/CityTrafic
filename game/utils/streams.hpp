#pragma once

#include <fstream>
#include <string>

struct WriteStream {
private:
	std::ofstream file;

public:
	WriteStream(const std::string& path);
	~WriteStream();

	void copy(const void* src, size_t length);

	template<typename T>
	inline void write(const T& value) {
		file.write(
			reinterpret_cast<const char*>(&value),
			sizeof(T)
		);
	}

};


struct ReadStream {
private:
	std::ifstream file;

public:
	ReadStream(const std::string& path);
	~ReadStream();

	void copy(void* dst, size_t length);

	template<typename T>
	void read(T& value) {
		file.read(
			reinterpret_cast<char*>(&value),
			sizeof(T)
		);
	}

	template<typename T>
	inline T read() {
		T value;
		file.read(
			reinterpret_cast<char*>(&value),
			sizeof(T)
		);

		return value;

	}
};