#pragma once
#include "DumbsterDatabaseManager.h"
#include <ostream>
#include <iostream>

DumbsterDatabaseManager::DumbsterDatabaseManager(const std::string &dbName) {
    this->dbName = dbName;
    this->db = nullptr;
    bool openStatus = openDB();
    if (!openStatus) {
        std::cerr << "Can't open database " << this->dbName << std::endl;
    }
    bool setupStatus = setupDB();
    if (!setupStatus) {
        std::cerr << "Can't setup database " << this->dbName << std::endl;
    }
}

DumbsterDatabaseManager::~DumbsterDatabaseManager() {
    bool closeStatus = closeDB();
    if (!closeStatus) {
        std::cerr << "Can't close database " << this->dbName << std::endl;
    }
}

bool DumbsterDatabaseManager::openDB() {
    bool openStatus = sqlite3_open(this->dbName.c_str(), &this->db);
    if (openStatus != SQLITE_OK) {
        std::cerr << "Error opening database " << this->dbName << std::endl;
        return false;
    }
    std::cout << "Opened database " << this->dbName << std::endl;
    return true;
}

bool DumbsterDatabaseManager::closeDB() const {
    bool closeStatus = sqlite3_close(this->db);
    if (closeStatus != SQLITE_OK) {
        std::cerr << "Error closing database, rolling back " << this->dbName << std::endl;
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        closeStatus = sqlite3_close(this->db);
    }
    if (closeStatus != SQLITE_OK) {
        std::cerr << "Error closing database " << this->dbName << std::endl;
        return false;
    }
    std::cout << "Closed database " << this->dbName << std::endl;
    return true;
}

bool DumbsterDatabaseManager::setupDB() const {
    const char* sqlQuery =
        "CREATE TABLE IF NOT EXISTS dumbster ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "city TEXT NOT NULL,"
        "county TEXT NOT NULL,"
        "street TEXT NOT NULL,"
        "streetNumber INTEGER NOT NULL,"
        "isFull BOOLEAN,"
        "useNumber INTEGER"
        ");";
    char* errMsg = nullptr;
    int execStatus = sqlite3_exec(this->db, sqlQuery, nullptr, nullptr, &errMsg);
    if (execStatus != SQLITE_OK) {
        std::cerr << "Error creating database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    std::cout << "Created database " << this->dbName << std::endl;
    return true;
}

bool DumbsterDatabaseManager::newDumbster(const std::string& city, const std::string& county, const std::string& street, int streetNumber) const {
    const char* sqlQuery = "INSERT INTO dumbster (city, county, street, streetNumber, isFull, useNumber) VALUES (?, ?, ?, ?, FALSE, 0);";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare(this->db, sqlQuery, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing newDumbster " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_bind_text(stmt, 1, city.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, county.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, street.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, streetNumber);

    int stepVal = sqlite3_step(stmt);
    bool success = stepVal == SQLITE_DONE;
    if (!success) {
        std::cerr << "Error adding dumbster " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    std::cout << "Added dumbster" << std::endl;
    return true;
}

bool DumbsterDatabaseManager::deleteDumbster(const int id) const {
    const char* sqlQuery = "DELETE FROM dumbster WHERE id = ?;";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sqlQuery, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing deleteDumbster " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_bind_int(stmt, 1, id);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    if (!success) {
        std::cerr << "Error deleting dumbster " << sqlite3_errmsg(db) << std::endl;
    }
    sqlite3_finalize(stmt);

    return success;
}

bool DumbsterDatabaseManager::updateDumbster(const int id, const std::string& city, const std::string& county, const std::string& street, const int streetNumber) const {
    const char* sqlQuery = "UPDATE dumbster SET city = ?, county = ?, street = ?, streetNumber = ? WHERE id = ?;";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sqlQuery, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing updateDumbster " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_bind_text(stmt, 1, city.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, county.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, street.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, streetNumber);
    sqlite3_bind_int(stmt, 5, id);

    int stepVal = sqlite3_step(stmt);
    bool success = stepVal == SQLITE_DONE;
    if (!success) {
        std::cerr << "Error updating dumbster " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    std::cout << "Updated dumbster " << std::endl;
    return true;
}

bool DumbsterDatabaseManager::isDumbsterFull(const int id) const {
    const char* sqlQuery = "SELECT isFull FROM dumbster WHERE id = ?;";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sqlQuery, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing isDumbsterFull " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_bind_int(stmt, 1, id);

    int stepCheck = sqlite3_step(stmt);

    if (stepCheck == SQLITE_ROW) {
        int val = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return val == 1;
    }
    else if (stepCheck == SQLITE_DONE) {
        std::cerr << "Dumbster not found" << std::endl;
    }
    else {
        std::cerr << "Error isDumbsterFull " << sqlite3_errmsg(db) << std::endl;
    }
    sqlite3_finalize(stmt);
    return false;
}

DumbsterData DumbsterDatabaseManager::getDumbster(const int id) const {
    DumbsterData data;
    const char* sqlQuery =
        "SELECT * FROM dumbster WHERE id = ?;";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare(this->db, sqlQuery, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing getDumbster: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return data;
    }

    sqlite3_bind_int(stmt, 1, id);

    int stepCheck = sqlite3_step(stmt);

    if (stepCheck == SQLITE_ROW) {
        data.id = sqlite3_column_int(stmt, 0);
        data.city = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        data.county = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        data.street = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        data.streetNumber = sqlite3_column_int(stmt, 4);
        data.isFull = sqlite3_column_int(stmt, 5) == 1;
        data.useNumber = sqlite3_column_int(stmt, 6);
    }
    else if (stepCheck == SQLITE_DONE) {
        std::cerr << "Dumbster not found: " << id << std::endl;
    }
    else {
        std::cerr << "Error getting dumbster " << sqlite3_errmsg(db) << std::endl;
    }
    sqlite3_finalize(stmt);
    return data;
}

std::vector<DumbsterData> DumbsterDatabaseManager::getDumbstersCity(const std::string& city) const {
    std::vector<DumbsterData> data;
    DumbsterData temp;
    const char* sqlQuery =
        "SELECT * FROM dumbster WHERE city = ? ORDER BY street;";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare(this->db, sqlQuery, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing getDumbstersCity: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return data;
    }

    sqlite3_bind_text(stmt, 1, city.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        temp.id = sqlite3_column_int(stmt, 0);
        temp.city = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        temp.county = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        temp.street = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        temp.streetNumber = sqlite3_column_int(stmt, 4);
        temp.isFull = sqlite3_column_int(stmt, 5) == 1;
        temp.useNumber = sqlite3_column_int(stmt, 6);
        data.push_back(temp);
    }
    sqlite3_finalize(stmt);
    return data;
}

std::vector<DumbsterData> DumbsterDatabaseManager::getDumbstersStreet(const std::string& street) const {
    std::vector<DumbsterData> data;
    DumbsterData temp;
    const char* sqlQuery =
        "SELECT * FROM dumbster WHERE street = ? ORDER BY streetNumber;";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare(this->db, sqlQuery, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing getDumbstersStreet: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return data;
    }

    sqlite3_bind_text(stmt, 1, street.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        temp.id = sqlite3_column_int(stmt, 0);
        temp.city = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        temp.county = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        temp.street = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        temp.streetNumber = sqlite3_column_int(stmt, 4);
        temp.isFull = sqlite3_column_int(stmt, 5) == 1;
        temp.useNumber = sqlite3_column_int(stmt, 6);
        data.push_back(temp);
    }
    sqlite3_finalize(stmt);
    return data;
}

std::vector<DumbsterData> DumbsterDatabaseManager::getDumbstersCounty(const std::string& county) const {
    std::vector<DumbsterData> data;
    DumbsterData temp;
    const char* sqlQuery =
        "SELECT * FROM dumbster WHERE county = ? ORDER BY city;";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare(this->db, sqlQuery, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing getDumbstersCounty: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return data;
    }

    sqlite3_bind_text(stmt, 1, county.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        temp.id = sqlite3_column_int(stmt, 0);
        temp.city = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        temp.county = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        temp.street = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        temp.streetNumber = sqlite3_column_int(stmt, 4);
        temp.isFull = sqlite3_column_int(stmt, 5) == 1;
        temp.useNumber = sqlite3_column_int(stmt, 6);
        data.push_back(temp);
    }
    sqlite3_finalize(stmt);
    return data;
}

bool DumbsterDatabaseManager::updateDumbsterFull(int id, const bool isFull) const {
    const char* sqlQuery = "UPDATE dumbster SET isFull = ? WHERE id = ?;";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sqlQuery, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing updateDumbster " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_bind_int(stmt, 1, id);
    sqlite3_bind_int(stmt, 2, isFull);

    int stepVal = sqlite3_step(stmt);
    bool success = stepVal == SQLITE_DONE;
    if (!success) {
        std::cerr << "Error updating dumbster " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    std::cout << "Updated dumbsterFull " << std::endl;
    return true;
}
