#pragma once
#include <string>
#include "sqlite3.h"
#include <vector>

struct DumbsterData {
    int id;
    std::string city;
    std::string county;
    std::string street;
    int streetNumber;
    bool isFull;
    int useNumber;
    DumbsterData() {
        id = 0;
        city = "";
        county = "";
        streetNumber = 0;
        isFull = false;
        useNumber = 0;
    }
    DumbsterData(const std::string &city, const std::string &county, const std::string &street, const int streetNumber) {
        id = 0;
        this->city = city;
        this->county = county;
        this->street = street;
        this->streetNumber = streetNumber;
        isFull = false;
        useNumber = 0;
    }
};

class DumbsterDatabaseManager {
    sqlite3* db;
    std::string dbName;
public:
    explicit DumbsterDatabaseManager(const std::string& dbName);
    ~DumbsterDatabaseManager();

    bool openDB();
    bool closeDB() const;
    bool setupDB() const;

    bool newDumbster(const std::string& city, const std::string& county, const std::string& street, int streetNumber) const;
    bool deleteDumbster(int id) const;
    bool updateDumbster(int id, const std::string& city, const std::string& county, const std::string& street, int streetNumber) const;

    bool isDumbsterFull(int id) const;
    bool updateDumbsterFull(int id, bool isFull) const;

    DumbsterData getDumbster(int id) const;
    std::vector<DumbsterData> getDumbstersCity(const std::string& city) const;
    std::vector<DumbsterData> getDumbstersCounty(const std::string& county) const;
    std::vector<DumbsterData> getDumbstersStreet(const std::string& street) const;

};
