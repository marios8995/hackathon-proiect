#pragma once
#include "AccountDatabaseManager.h"

#include <iomanip>
#include <iostream>
#include <ostream>
#include "SHA256.h"

#include "DatabaseManager.h"

AccountDatabaseManager::AccountDatabaseManager(const std::string &dbName) {
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

AccountDatabaseManager::~AccountDatabaseManager() {
    bool closeStatus = closeDB();
    if (!closeStatus) {
        std::cerr << "Can't close database " << this->dbName << std::endl;
    }
}

bool AccountDatabaseManager::openDB() {
    bool openStatus = sqlite3_open(this->dbName.c_str(), &this->db);
    if (openStatus != SQLITE_OK) {
        std::cerr << "Error opening database " << this->dbName << std::endl;
        return false;
    }
    std::cout << "Opened database " << this->dbName << std::endl;
    return true;
}

bool AccountDatabaseManager::closeDB() const {
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

bool AccountDatabaseManager::setupDB() const {
    const char* sqlQuery =
        "CREATE TABLE IF NOT EXISTS accountData ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT NOT NULL,"
        "password TEXT NOT NULL,"
        "email TEXT NOT NULL UNIQUE"
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

bool AccountDatabaseManager::newAccount(const std::string& username, const std::string &password, const std::string& email) const {
    const char* sqlQuery = "INSERT INTO accountData (username, password, email) VALUES (?, ?, ?);";
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare(this->db, sqlQuery, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing newAccount " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, encryptPassword(password).c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, email.c_str(), -1, SQLITE_TRANSIENT);

    int stepVal = sqlite3_step(stmt);
    bool success = stepVal == SQLITE_DONE;
    if (!success) {
        if (stepVal == SQLITE_CONSTRAINT) {
            std::cerr << "Email is already used!" << std::endl;
        }
        else {
            std::cerr << "Error creating account " << sqlite3_errmsg(db) << std::endl;
        }
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    std::cout << "Created account" << std::endl;
    return true;
}

bool AccountDatabaseManager::deleteAccount(const std::string& email, const std::string &password) const {
    AccountData data = getAccountInfo(email);
    if (!checkPassword(email, password)) return false;
    const char* sqlQuery = "DELETE FROM accountData WHERE email = ?;";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sqlQuery, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing deleteAccount " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_TRANSIENT);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    if (!success) {
        std::cerr << "Error deleting account " << sqlite3_errmsg(db) << std::endl;
    }
    sqlite3_finalize(stmt);

    return success;
}

bool AccountDatabaseManager::updateAccount(const std::string &username, const std::string &password, const std::string &email, const std::string &oldEmail, const std::string &oldPassword) const {
    AccountData data = getAccountInfo(oldEmail);
    if (!checkPassword(oldEmail, oldPassword)) return false;
    const char* sqlQuery = "UPDATE accountData SET username = ?, password = ?, email = ? WHERE email = ?;";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sqlQuery, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing updateAccount " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, encryptPassword(password).c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, email.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, oldEmail.c_str(), -1, SQLITE_TRANSIENT);


    int stepVal = sqlite3_step(stmt);
    bool success = stepVal == SQLITE_DONE;
    if (!success) {
        if (stepVal == SQLITE_CONSTRAINT) {
            std::cerr << "Email is already used!" << std::endl;
        }
        else {
            std::cerr << "Error updating account " << sqlite3_errmsg(db) << std::endl;
        }
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    std::cout << "Updated account " << std::endl;
    return true;
}


AccountData AccountDatabaseManager::getAccountInfo(const std::string& email) const {
    AccountData accData;
    const char* sqlQuery =
        "SELECT username, password FROM accountData WHERE email = ?;";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare(this->db, sqlQuery, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing getAccountInfo: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return accData;
    }

    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_TRANSIENT);

    int stepCheck = sqlite3_step(stmt);

    if (stepCheck == SQLITE_ROW) {
        accData.username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        accData.password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        accData.email = email;
    }
    else if (stepCheck == SQLITE_DONE) {
        std::cerr << "Account not found: " << email << std::endl;
    }
    else {
        std::cerr << "Error getting account " << sqlite3_errmsg(db) << std::endl;
    }
    sqlite3_finalize(stmt);
    return accData;
}

std::string AccountDatabaseManager::encryptPassword(const std::string& password) {
    SHA256 sha;
    sha.update(password);
    const auto digest = sha.digest();

    std::string passwordEncoded = SHA256::toString(digest);

    return passwordEncoded;
}

bool AccountDatabaseManager::checkPassword(const std::string &email, const std::string &enteredPassword) const {
    AccountData accData = getAccountInfo(email);
    std::string enteredHash = encryptPassword(enteredPassword);
    if (enteredHash != accData.password) {
        return false;
    }
    return true;
}

