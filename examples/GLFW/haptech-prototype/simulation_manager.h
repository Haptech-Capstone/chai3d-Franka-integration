#pragma once
#include "chai3d.h"
#include <GLFW/glfw3.h>
#include <mutex>

using namespace chai3d;

struct SimulationContext {
    GLFWwindow* window = nullptr;
    chai3d::cWorld* world = nullptr;
    chai3d::cCamera* camera = nullptr;
    cToolCursor* tool = nullptr;
    cHapticDeviceHandler* handler = nullptr;
    cGenericHapticDevicePtr hapticDevice = nullptr;
    cThread hapticsThread;

    int width = 0;
    int height = 0;
    int swapInterval = 1;

    void reset();
};

class SimulationManager {
public:
    bool initialize();
    void runMainLoop();
    void shutdown();
    void requestShutdown();
    void addObject(float innerRadius, float outerRadius, float stiffness);
    void clearObjects();

private:
    bool initialized = false;
    bool stopRequested = false;
    bool shutdownRequested = false;
    std::mutex simMutex;
    SimulationContext ctx;

    void updateGraphics();
    void updateHaptics();
};
