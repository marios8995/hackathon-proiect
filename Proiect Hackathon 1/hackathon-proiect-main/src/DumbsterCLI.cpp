#pragma once
#include "Dumbster.h"

void Dumbster::startMonitoring() {
    if (running) return; // already running
    running = true;
    monitorThread = std::jthread(&Dumbster::monitorLoop, this);
    std::cout << "[Monitor] Started monitoring dumpster ID: " << id << "\n";
}

void Dumbster::stopMonitoring() {
    if (!running) return;
    running = false;
    monitorThread.request_stop(); // ask thread to stop
    std::cout << "[Monitor] Stopped monitoring dumpster ID: " << id << "\n";
}

void Dumbster::monitorLoop(std::stop_token stopToken) {
    std::default_random_engine gen(std::random_device{}());
    std::uniform_real_distribution<float> dist(0.0f, 100.0f);

    while (!stopToken.stop_requested() && running) {
        fullness = simulateSensorReading();
        std::cout << "[Sensor] Dumpster " << id
            << " fullness: " << fullness << "%\n";

        bool isFull = fullness >= 80.0f;
        database.updateDumbsterFull(id, isFull);

        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}

float Dumbster::simulateSensorReading() {
    static std::default_random_engine gen(std::random_device{}());
    static std::uniform_real_distribution<float> dist(0.0f, 100.0f);
    return dist(gen);
}


