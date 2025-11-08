#pragma once
#include "DumbsterDatabaseManager.h"
#include <atomic>
#include <thread>
#include <chrono>
#include <random>
#include <iostream>

class Dumbster {
    DumbsterDatabaseManager database;
    std::string dbName;
    int id;

    float fullness = 0.0f;//
    std::atomic<bool> running{false};
    std::jthread monitorThread;

public:
    Dumbster(const std::string& dbName, int id)
        : dbName(dbName), database(dbName), id(id) {}

    void startMonitoring();
    void stopMonitoring();
    float getFullness() const {
        return fullness;

private:
    void monitorLoop(std::stop_token stopToken);
    static float simulateSensorReading();
};