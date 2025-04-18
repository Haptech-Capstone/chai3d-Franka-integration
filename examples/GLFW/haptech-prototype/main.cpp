#include "prototype_sim.h"
#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "Press Enter to start the sim:";
    std::cin.get();
    
    load(argc, argv);
    return 0;
}