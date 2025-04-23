#include "franka_trial_server.h"

// Inside franka_server.cpp or similar
grpc::Status FrankaTrialServiceImpl::RunTrial(
    grpc::ServerContext* context,
    const TrialRequest* request,
    grpc::ServerWriter<TrialResponseStream>* writer
) {
    // load trial with SimulationManager, send trial started update

    // stream data points for the given amount of time

    // reset environment (SimulationManager::reset())

    // send trial completed update

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