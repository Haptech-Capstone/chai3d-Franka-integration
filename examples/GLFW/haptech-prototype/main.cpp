#include "simulation_manager.h"

int main() {
    SimulationManager sim;
    if (sim.initialize()) {
        sim.runMainLoop();
    }
    return 0;
}
