#pragma once
#include "DatabaseManager.h"
#include <iostream>
#include <ostream>
#include <sstream>

DatabaseManager::DatabaseManager(const std::string &dbName) {
    this->dbName = dbName;
    this->db = nullptr;
    bool openStatus = openDB();
    if (!openStatus) {
        std::cerr << "Error opening database." << dbName << std::endl;
    }
    bool setupStatus = setupDB();
    if (!setupStatus) {
        std::cerr << "Error setting up database." << dbName << std::endl;
    }
}

DatabaseManager::~DatabaseManager() {
    bool closeStatus = this->closeDB();
    if (!closeStatus) {
        std::cerr << "Error closing database." << dbName << std::endl;
    }
}


bool DatabaseManager::openDB() {
    int openStatus = sqlite3_open(dbName.c_str(), &db);
    if (openStatus != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    std::cout << "Opened database " << dbName << std::endl;
    return true;
}

bool DatabaseManager::setupDB() const {
    const char* sqlQueryReadings =
        "CREATE TABLE IF NOT EXISTS readings ("
        "carbonDioxide REAL,"
        "methane REAL,"
        "ammonia REAL,"
        "inductivity REAL,"
        "reflectance REAL,"
        "user TEXT NOT NULL"
        ");";
    char* errMsg = nullptr;
    int execStatus = sqlite3_exec(db, sqlQueryReadings, nullptr, nullptr, &errMsg);
    if (execStatus != SQLITE_OK) {
        std::cerr << "Error creating table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    std::cout << "Table readings created successfully" << std::endl;
    return true;
}

bool DatabaseManager::closeDB() const {
    int exitStatus = sqlite3_close(db);
    if (exitStatus != SQLITE_OK) {
        std::cerr << "Failed to close database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    std::cout << "Closed database successfully" << std::endl;
    return true;
}

bool DatabaseManager::addReading(const float carbon, const float methane, const float ammonia, const float induct, const float reflect, const std::string& email) const {

    const char* sql =
        "INSERT INTO Readings (carbonDioxide, methane, ammonia, inductivity, reflectance, user) "
        "VALUES (?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing addReading: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_double(stmt, 1, carbon);
    sqlite3_bind_double(stmt, 2, methane);
    sqlite3_bind_double(stmt, 3, ammonia);
    sqlite3_bind_double(stmt, 4, induct);
    sqlite3_bind_double(stmt, 5, reflect);
    sqlite3_bind_text(stmt, 6, email.c_str(), -1, SQLITE_TRANSIENT);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!success)
        std::cerr << "Error executing addReading: " << sqlite3_errmsg(db) << std::endl;
    sqlite3_finalize(stmt);
    return success;
}

std::vector<Reading> DatabaseManager::getReadings(const std::string& user) const {
    std::vector<Reading> readings;
    const char* sqlQuery =
        "SELECT carbonDioxide, methane, ammonia, inductivity, reflectance FROM readings";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sqlQuery, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing getSensorsByDevice: " << sqlite3_errmsg(db) << std::endl;
        return readings;
    }
    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Reading r;
        r.carbonDioxide = sqlite3_column_double(stmt, 1);
        r.methane = sqlite3_column_double(stmt, 2);
        r.ammonia = sqlite3_column_double(stmt, 3);
        r.inductivity = sqlite3_column_double(stmt, 4);
        r.reflectance = sqlite3_column_double(stmt, 5);
        readings.push_back(r);
    }

    sqlite3_finalize(stmt);
    return readings;
}


