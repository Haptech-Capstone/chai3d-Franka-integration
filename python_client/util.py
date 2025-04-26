import grpc
import csv
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
    # TODO: Display status
    print(f"Status Update: {status.message} (Status: {status.status})")

# Handle a TrialDataPoint
def handle_data_point(data):
    # Display data point, log to CSV file
    timestamp = data.timestamp
    device_pos = data.device_position
    proxy_pos = data.proxy_position
    force = data.force

    # display data point
    print(f"Data Point: {timestamp}, Device Position: {device_pos}, Proxy Position: {proxy_pos}, Force: {force}")

    # log to CSV file
    with open('trial_data.csv', mode='a', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow([timestamp, device_pos, proxy_pos, force])

