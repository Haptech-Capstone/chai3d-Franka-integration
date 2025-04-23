#include "simulation_manager.h"
#include <thread>
#include <chrono>
#include <iostream>

void runTestsAfterDelay() {
    // Give the simulation a few seconds to boot
    std::this_thread::sleep_for(std::chrono::seconds(5));

    std::cout << "[Test Thread] Running test code..." << std::endl;

    // Test 1: change background color
    bool success = SimulationManager::setBackgroundColor(0.2, 0.3, 0.8);
    if (success) {
        std::cout << "[Test Thread] Background color updated." << std::endl;
    } else {
        std::cout << "[Test Thread] Changing background color failed." << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Test 2: Add a sphere
    success = SimulationManager::addSphere("THE SPHERE", 0.1, 0.0, 0.3, 0.0);
    if (success) {
        std::cout << "[Test Thread] Sphere added." << std::endl;
    } else {
        std::cout << "[Test Thread] Adding a sphere failed." << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Test 3: Remove the sphere
    success = SimulationManager::removeObject("THE SPHERE");
    if (success) {
        std::cout << "[Test Thread] Sphere removed." << std::endl;
    } else {
        std::cout << "[Test Thread] Removing the sphere failed." << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Test 4: Add a sphere again
    success = SimulationManager::addSphere("THE SPHERE", 0.1, 0.0, 0.3, 0.0);
    if (success) {
        std::cout << "[Test Thread] Sphere added again." << std::endl;
    } else {
        std::cout << "[Test Thread] Adding a sphere again failed." << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Test 5: Reset simulation
    success = SimulationManager::reset();
    if (success) {
        std::cout << "[Test Thread] Simulation reset." << std::endl;
    } else {
        std::cout << "[Test Thread] Resetting simulation failed." << std::endl;
    }
}

int main() {
    // Launch test thread (so that simulation is on main thread)
    std::thread testThread(runTestsAfterDelay);

    // Run simulation on main thread
    SimulationManager::setDebugMode(false);
    SimulationManager::run();

    // Join test thread after simulation ends
    testThread.join();

    return 0;
}
