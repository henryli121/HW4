#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <random>
#include <condition_variable>
#include <atomic>
#include <unordered_set>

const int numFloors = 20;
const int numElevators = 3;
const int maxCapacity = 5;

std::mutex consoleMutex;
std::vector<int> elevatorCurrentFloors(numElevators, 0);
std::vector<int> elevatorDirections(numElevators, 1);
std::vector<int> elevatorPassengerCount(numElevators, 0);
std::vector<std::unordered_set<int>> elevatorPassengers(numElevators);
std::vector<std::vector<int>> waitingPassengers(numFloors);
std::condition_variable servicedEnough;
std::mutex servicedEnoughMutex;
std::atomic<int> totalServicedCount(0);
std::atomic<bool> done(false);

void elevator(int id) {
    while (!done) {
        int currentFloor = elevatorCurrentFloors[id];

        if (!waitingPassengers[currentFloor].empty() && elevatorPassengerCount[id] < maxCapacity) {
            std::unique_lock<std::mutex> lock(consoleMutex);
            int destinationFloor = -1;
            for (int floor : waitingPassengers[currentFloor]) {
                if (elevatorPassengers[id].find(floor) == elevatorPassengers[id].end()) {
                    destinationFloor = floor;
                    break;
                }
            }
            if (destinationFloor != -1) {
                waitingPassengers[currentFloor].erase(std::remove(waitingPassengers[currentFloor].begin(), waitingPassengers[currentFloor].end(), destinationFloor), waitingPassengers[currentFloor].end());
                elevatorPassengers[id].insert(destinationFloor);
                elevatorPassengerCount[id]++;
                std::cout << "Elevator " << id << " picked up a passenger from floor " << currentFloor << ", destination floor: " << destinationFloor << ", total passengers: " << elevatorPassengerCount[id] << "\n";
            }
        } else if (elevatorPassengerCount[id] > 0) {
            std::unique_lock<std::mutex> lock(consoleMutex);
            auto it = elevatorPassengers[id].begin();
            while (it != elevatorPassengers[id].end()) {
                int floor = *it;
                if (floor == currentFloor) {
                    it = elevatorPassengers[id].erase(it);
                    elevatorPassengerCount[id]--;
                    totalServicedCount++;
                    std::cout << "Elevator " << id << " dropped off a passenger at floor " << currentFloor << ", total passengers: " << elevatorPassengerCount[id] << "\n";
                } else {
                    ++it;
                }
            }
        }

        if (currentFloor == 0)
            elevatorDirections[id] = 1;
        else if (currentFloor == numFloors - 1)
            elevatorDirections[id] = -1;

        int nextFloor = currentFloor + elevatorDirections[id];
        if (nextFloor >= 0 && nextFloor < numFloors) {
            currentFloor = nextFloor;
            elevatorCurrentFloors[id] = currentFloor;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void person(int id) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, numFloors - 1);

    int currentFloor = dis(gen);
    int destinationFloor = dis(gen);
    while (destinationFloor == currentFloor) {
        destinationFloor = dis(gen);
    }
    std::unique_lock<std::mutex> lock(consoleMutex);
    std::cout << "Person " << id << " wants to go from floor " << currentFloor << " to floor " << destinationFloor << "\n";

    waitingPassengers[currentFloor].push_back(destinationFloor);
}
int main(int argc, char* argv[]) {
    int numPeople = std::stoi(argv[1]);
    std::vector<std::thread> elevatorThreads;
    for (int i = 0; i < numElevators; ++i) {
        elevatorThreads.push_back(std::thread(elevator, i));
    }

    std::vector<std::thread> personThreads;
    for (int i = 0; i < numPeople; ++i) {
        personThreads.push_back(std::thread(person, i));
        personThreads.back().detach();
    }

    // Wait for all people to be serviced
    while (totalServicedCount < numPeople) {
        std::this_thread::yield();
    }

    done = true;
    servicedEnough.notify_all();

    for (auto& t : elevatorThreads) {
        if (t.joinable()) {
            t.join();
        }
    }
std::cout << "All people have served! " << "\n";
return 0;
}





