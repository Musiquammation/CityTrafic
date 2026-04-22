#ifdef COMPILE_SERVER

#include <iostream>
#include <string>
#include <thread>

#include "Server.hpp"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Port is missing" << std::endl;
        return 1;
    }

    int port = std::stoi(argv[1]);
   	int poolNum = std::thread::hardware_concurrency();
	if (poolNum > 1) {poolNum--;}

    Server server{poolNum};
    server.run(port); 

    return 0;
}

#endif