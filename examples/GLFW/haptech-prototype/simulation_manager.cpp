#include "simulation_manager.h"
#include <iostream>

using namespace chai3d;

void SimulationContext::reset() {
    window = nullptr;
    world = nullptr;
    camera = nullptr;
    width = height = 0;
}

bool SimulationManager::initialize() {
    if (initialized) return true;

    if (!glfwInit()) {
        std::cerr << "[SimManager] GLFW init failed\n";
        return false;
    }

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int w = 0.8 * mode->height;
    int h = 0.5 * mode->height;
    int x = 0.5 * (mode->width - w);
    int y = 0.5 * (mode->height - h);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    ctx.window = glfwCreateWindow(w, h, "CHAI3D Simulation", nullptr, nullptr);
    if (!ctx.window) {
        std::cerr << "[SimManager] Window creation failed\n";
        glfwTerminate();
        return false;
    }

    glfwSetWindowPos(ctx.window, x, y);
    glfwMakeContextCurrent(ctx.window);
    glfwSwapInterval(ctx.swapInterval);

    ctx.width = w;
    ctx.height = h;

    ctx.world = new cWorld();
    ctx.world->m_backgroundColor.setBlack();

    ctx.camera = new cCamera(ctx.world);
    ctx.world->addChild(ctx.camera);
    ctx.camera->set(cVector3d(3.0, 0.0, 0.0),
                    cVector3d(0.0, 0.0, 0.0),
                    cVector3d(0.0, 0.0, 1.0));
    ctx.camera->setClippingPlanes(0.01, 10.0);

    // create a haptic device handler
    ctx.handler = new cHapticDeviceHandler();

    // get access to the first available haptic device found
    ctx.handler->getDevice(ctx.hapticDevice, 0);

    // retrieve information about the current haptic device
    cHapticDeviceInfo hapticDeviceInfo = ctx.hapticDevice->getSpecifications();

    // create a tool (cursor) and insert into the world
    ctx.tool = new cToolCursor(ctx.world);
    ctx.world->addChild(ctx.tool);

    // connect the haptic device to the virtual tool
    ctx.tool->setHapticDevice(ctx.hapticDevice);

    // define a radius for the virtual tool (sphere)
    ctx.tool->setRadius(1);

    // map the physical workspace of the haptic device to a larger virtual workspace.
    ctx.tool->setWorkspaceRadius(1.0);

    // haptic forces are enabled only if small forces are first sent to the device;
    // this mode avoids the force spike that occurs when the application starts when 
    // the tool is located inside an object for instance. 
    ctx.tool->setWaitForSmallForce(true);

    // start the haptic tool
    ctx.tool->start();

    initialized = true;
    return true;
}

void SimulationManager::runMainLoop() {
    std::cout << "[SimManager] Running empty simulation loop\n";

    std::cout << "[SimManager] Starting haptics thread (nothing for now)\n";

    std::cout << "[SimManager] Starting graphics loop\n";

    while (!stopRequested && !shutdownRequested && !glfwWindowShouldClose(ctx.window)) {
        glfwGetWindowSize(ctx.window, &ctx.width, &ctx.height);

        updateGraphics();

        glfwSwapBuffers(ctx.window);
        glfwPollEvents();
    }

    ctx.tool->stop();

    if (shutdownRequested) {
        shutdown();
    }
    
    std::cout << "[SimManager] Exiting simulation loop\n";
}

void SimulationManager::shutdown() {
    std::lock_guard<std::mutex> lock(simMutex);

    std::cout << "[SimManager] Shutting down\n";

    std::cout << "[SimManager] Deleting camera\n";

    if (ctx.camera) {
        delete ctx.camera;
    }

    std::cout << "[SimManager] Deleting world\n";

    if (ctx.world) {
        delete ctx.world;
    }

    std::cout << "[SimManager] Deleting window\n";

    if (ctx.window && !glfwWindowShouldClose(ctx.window)) {
        glfwDestroyWindow(ctx.window);
        ctx.window = nullptr;
    } else {
        std::cout << "[SimManager] Window already closed by user or OS\n";
    }

    std::cout << "[SimManager] Terminating GLFW\n";

    glfwTerminate();

    std::cout << "[SimManager] Resetting context\n";
    ctx.reset();
    
    initialized = false;
}

void SimulationManager::requestShutdown() {
    std::lock_guard<std::mutex> lock(simMutex);
    stopRequested = true;
    shutdownRequested = true;
}

void SimulationManager::clearObjects() {
    std::lock_guard<std::mutex> lock(simMutex);
    std::cout << "[SimManager] Clearing all simulation objects..." << std::endl;

    if (ctx.world) {
        // remove all objects except camera and tool
        cCamera* cam = ctx.camera;
        cToolCursor* tool = ctx.tool;

        ctx.world->clearAllChildren();

        if (cam) ctx.world->addChild(cam);
        if (tool) ctx.world->addChild(tool);
    }
}

void SimulationManager::addObject(float innerRadius, float outerRadius, float stiffness) {
    std::lock_guard<std::mutex> lock(simMutex);
    std::cout << "[SimManager] Adding object: inner " << innerRadius
              << ", outer " << outerRadius << ", stiffness " << stiffness << std::endl;

    if (!ctx.world || !ctx.tool || !ctx.hapticDevice) {
        std::cerr << "[SimManager] Can't add object: sim not initialized" << std::endl;
        return;
    }

    auto object = new cShapeTorus(innerRadius, outerRadius);
    object->setLocalPos(0.0, 0.0, 0.0);
    object->rotateAboutGlobalAxisDeg(cVector3d(0, 1, 0), 90);
    ctx.world->addChild(object);

    double wsScale = ctx.tool->getWorkspaceScaleFactor();
    double maxStiffness = ctx.hapticDevice->getSpecifications().m_maxLinearStiffness;
    object->m_material->setStiffness(stiffness * maxStiffness / wsScale);

    // Optional: add texture or color
    object->m_material->m_ambient.set(0.9f, 0.9f, 0.9f);
    object->m_material->m_diffuse.set(0.9f, 0.9f, 0.9f);
    object->m_material->m_specular.set(1.0f, 1.0f, 1.0f);
    object->addEffect(new cEffectSurface(object));
}

void SimulationManager::updateGraphics(void)
{
    /////////////////////////////////////////////////////////////////////
    // RENDER SCENE
    /////////////////////////////////////////////////////////////////////

    // update shadow maps (if any)
    ctx.world->updateShadowMaps(false, false);

    // render world
    ctx.camera->renderView(ctx.width, ctx.height);

    // wait until all GL commands are completed
    glFinish();

    // check for any OpenGL errors
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) std::cout << "Error: " << gluErrorString(err) << std::endl;
}
