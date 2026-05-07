#include "streams.hpp"



WriteStream::WriteStream(const std::string& path):
	file(path, std::ios::binary) {}


WriteStream::~WriteStream() {
	this->file.close();
}

void WriteStream::copy(const void* src, size_t length) {
	this->file.write((char*)src, length);
}

ReadStream::ReadStream(const std::string& path):
	file(path, std::ios::binary) {}


void ReadStream::copy(void* dst, size_t length) {
	this->file.read((char*)dst, length);
}


ReadStream::~ReadStream() {
	this->file.close();
}