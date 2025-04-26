#include "franka_trial_server.h"

// Inside franka_server.cpp or similar
grpc::Status FrankaTrialServiceImpl::RunTrial(
    grpc::ServerContext* context,
    const TrialRequest* request,
    grpc::ServerWriter<TrialResponseStream>* writer
) {
    TrialResponseStream statusMsg;
    TrialResponseStream dataPointMsg;

    // make sure we can run a trial
    if (!SimulationManager::simContext.simulationRunning) {
        std::cout << "[ERROR] Simulation is not running." << std::endl;

        TrialStatusUpdate* status = statusMsg.mutable_statusupdate();
        status->set_status(TrialStatus::ERROR);
        status->set_message("Simulation is not running yet.");
        writer->Write(statusMsg);

        return grpc::Status::OK;
    }

    if (SimulationManager::trialContext.trialRunning) {
        std::cout << "[ERROR] There is already a trial running." << std::endl;

        TrialStatusUpdate* status = statusMsg.mutable_statusupdate();
        status->set_status(TrialStatus::ERROR);
        status->set_message("There is already a trial running.");
        writer->Write(statusMsg);

        return grpc::Status::OK;
    }

    // add objects (hard coded for now, load from config info later)
    SimulationManager::reset();

    if (!SimulationManager::addSphere("THE SPHERE", 0.1, 0.2, 0.0, 0.0)) {

        TrialStatusUpdate* status = statusMsg.mutable_statusupdate();
        status->set_status(TrialStatus::ERROR);
        status->set_message("Something went wrong loading the trial.");
        writer->Write(statusMsg);

        return grpc::Status::OK;
    }

    // set trial context
    SimulationManager::trialContext.trialId = request->trial_id();
    SimulationManager::trialContext.trialDuration = request->trialduration();
    SimulationManager::trialContext.time = 0.0;

    // start trial
    std::cout << "[TRIAL] Trial started: " << request->trial_id() << " for " << request->trialduration() << "s" << std::endl;

    TrialStatusUpdate* status = statusMsg.mutable_statusupdate();
    status->set_status(TrialStatus::STARTED);
    status->set_message("The trial has started.");
    writer->Write(statusMsg);

    SimulationManager::trialContext.trialRunning = true;

    cPrecisionClock clock;
    clock.start();

    // go until time limit reached
    while (SimulationManager::trialContext.time < SimulationManager::trialContext.trialDuration) {
        /*
            TODO: replace the 0.0's to actual values. how they will be collected idk. 
            maybe we can just pull the robot data straight from SimulationManager::simContext like in the dataPollingThread function
            but that sometimes doesn't work for some reason so idk, have fun!
        */
        TrialDataPoint* dataPoint = dataPointMsg.mutable_datapoint();

        dataPoint->set_timestamp(SimulationManager::trialContext.time);
        
        Vector3* devicePos = dataPoint->mutable_deviceposition();
        devicePos->set_x(0.0);
        devicePos->set_y(0.0);
        devicePos->set_z(0.0);

        Vector3* proxyPos = dataPoint->mutable_proxyposition();
        proxyPos->set_x(0.0);
        proxyPos->set_y(0.0);
        proxyPos->set_z(0.0);

        Vector3* force = dataPoint->mutable_force();
        force->set_x(0.0);
        force->set_y(0.0);
        force->set_z(0.0);

        writer->Write(dataPointMsg);

        if (SimulationManager::simContext.debug) {
            std::cout << "[DEBUG] [" << SimulationManager::trialContext.trialId << "] t = " << SimulationManager::trialContext.time << std::endl;
        }

        double dt = clock.getCurrentTimeSeconds();
        clock.reset();
        SimulationManager::trialContext.time += dt;

        // do not constantly get data, this would overflow everything
        cSleepMs(1000 / DATA_POLL_FREQUENCY);
    }

    // stop trial
    SimulationManager::trialContext.trialRunning = false;
    std::cout << "[TRIAL] Trial ended: " << SimulationManager::trialContext.trialId << ", lasted " << SimulationManager::trialContext.time << "s" << std::endl;

    status->set_status(TrialStatus::COMPLETED);
    status->set_message("The trial was completed.");
    writer->Write(statusMsg);

    // reset environment
    SimulationManager::reset();
    return grpc::Status::OK;
}


void runServer() {
    std::string server_address("0.0.0.0:50051");
    FrankaTrialServiceImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());

    std::cout << "[Server] Listening on " << server_address << std::endl;
    server->Wait();
}