#pragma once
#include "sqlite3.h"
#include <string>
#include <vector>

struct Reading {
    std::string timestamp;
    double carbonDioxide;
    double methane;
    double ammonia;
    double inductivity;
    double reflectance;
};

class DatabaseManager {
    sqlite3 *db;
    std::string dbName;
public:
    explicit DatabaseManager(const std::string &dbName);
    ~DatabaseManager();

    bool openDB();
    bool setupDB() const;
    bool closeDB() const;

    bool addReading(float carbon, float methane, float ammonia, float induct, float reflect, const std::string& email) const;

    std::vector<Reading> getReadings(const std::string& user) const;
};