#include "gate_server.hpp"
#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[])
{
	if(argc < 2){
		std::cout << "NEED PORT LIST" << std::endl;		
		return -1;
	}
	std::vector<short> port_list;
	for(int i = 2; i <= argc; ++i){
		port_list.push_back(short(std::atoi(argv[i - 1])));
	}
	Gate_Server gs(port_list, 1);
	gs.run();
	gs.join();
	return 0;
}
