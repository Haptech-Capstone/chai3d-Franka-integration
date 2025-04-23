import grpc
import time
import franka_trial_pb2 as pb2
import franka_trial_pb2_grpc as pb2_grpc

# Start a trial and stream back data/status updates
def run_trial(stub: pb2_grpc.FrankaTrialServiceStub, trial_id: str, duration: float):
    request = pb2.TrialRequest(trial_id=trial_id, trialDuration=duration)
    response_stream = stub.RunTrial(request)
    handle_stream(response_stream)

# Process messages coming from the stream
def handle_stream(stream):
    for message in stream:
        if message.HasField("statusUpdate"):
            handle_status_update(message.statusUpdate)
        elif message.HasField("dataPoint"):
            handle_data_point(message.dataPoint)

# Handle a TrialStatusUpdate
def handle_status_update(status):
    pass  # TODO: Display status

# Handle a TrialDataPoint
def handle_data_point(data):
    pass  # TODO: Display data point, log to CSV file
