import util
import grpc
import franka_trial_pb2_grpc as pb2_grpc

def main():
    # Take user input for trial ID, trial duration from config
    # TODO: load the config file

    # Get user input
    trial_id = input("Enter a trial ID: ")
    duration = float(input("Enter trial duration in seconds: "))

    # connect to the server
    channel = grpc.insecure_channel('localhost:50051')
    stub = pb2_grpc.FrankaTrialServiceStub(channel)

    # call run_trial with those values
    print(f"Starting trial '{trial_id}' for {duration} seconds...")
    util.run_trial(stub, trial_id, duration)
    print("Trial completed.")

if __name__ == "__main__":
    main()