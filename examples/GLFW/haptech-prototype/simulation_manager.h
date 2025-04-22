#pragma once
#include "chai3d.h"
#include <GLFW/glfw3.h>
#include <mutex>

using namespace chai3d;

#define DATA_POLL_FREQUENCY 10 // Hz

struct SimulationContext {
    // stereo Mode
    /*
        C_STEREO_DISABLED:            Stereo is disabled 
        C_STEREO_ACTIVE:              Active stereo for OpenGL NVDIA QUADRO cards
        C_STEREO_PASSIVE_LEFT_RIGHT:  Passive stereo where L/R images are rendered next to each other
        C_STEREO_PASSIVE_TOP_BOTTOM:  Passive stereo where L/R images are rendered above each other
    */
    cStereoMode stereoMode = C_STEREO_DISABLED;

    // fullscreen mode
    bool fullscreen = false;

    // mirrored display
    bool mirroredDisplay = false;

    // a world that contains all objects of the virtual environment
    cWorld* world;

    // a camera to render the world in the window display
    cCamera* camera;

    // a light source to illuminate the objects in the world
    cDirectionalLight *light;

    // a haptic device handler
    cHapticDeviceHandler* handler;

    // a pointer to the current haptic device
    cGenericHapticDevicePtr hapticDevice;

    // a label to display the rates [Hz] at which the simulation is running
    cLabel* labelRates;

    // a virtual tool representing the haptic device in the scene
    cToolCursor* tool;

    // flag to indicate if the haptic simulation currently running
    bool simulationRunning = false;

    // flag to indicate if the haptic simulation has terminated
    bool simulationFinished = false;

    // flag to indicate data thread running
    bool dataThreadRunning = false;

    // a frequency counter to measure the simulation graphic rate
    cFrequencyCounter freqCounterGraphics;

    // a frequency counter to measure the simulation haptic rate
    cFrequencyCounter freqCounterHaptics;

    // haptic thread
    cThread* hapticsThread;

    // debug thread
    cThread* dataThread;

    // a handle to window display context
    GLFWwindow* window = NULL;

    // current width of window
    int width  = 0;

    // current height of window
    int height = 0;

    // swap interval for the display context (vertical synchronization)
    int swapInterval = 1;
};

// Singleton SimulationManager class
class SimulationManager {
public:
    // data structure with "global variables" needed
    static SimulationContext simContext;

    // runs the simulation
    static bool run();

    // SCENE SETTINGS
    static bool setBackgroundColor(double r, double g, double b);

    // OBJECTS
    static bool addObject(cGenericObject* obj, double x, double y, double z);
    static bool addSphere(double radius, double x, double y, double z);
private:
    // callback when the window display is resized
    static void windowSizeCallback(GLFWwindow* a_window, int a_width, int a_height);

    // callback when an error GLFW occurs
    static void errorCallback(int error, const char* a_description);

    // callback when a key is pressed
    static void keyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods);

    // this function renders the scene
    static void updateGraphics(void);

    // this function contains the main haptics simulation loop
    static void updateHaptics(void);

    // Thread to poll for data
    static void dataPollingThread();

    // this function closes the application
    static void close(void);
};