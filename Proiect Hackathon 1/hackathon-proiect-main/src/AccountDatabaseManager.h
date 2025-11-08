#pragma once
#include "sqlite3.h"
#include "string"

struct AccountData {
    std::string username;
    std::string password;
    std::string email;
};

class AccountDatabaseManager {
    sqlite3 *db;
    std::string dbName;
public:
    explicit AccountDatabaseManager(const std::string &dbName);
    ~AccountDatabaseManager();

    bool openDB();
    bool closeDB() const;
    bool setupDB() const;

    bool newAccount(const std::string& username, const std::string &password, const std::string& email) const;
    bool deleteAccount(const std::string& email, const std::string &password) const;
    bool updateAccount(const std::string &username, const std::string &password, const std::string &email, const std::string &oldEmail, const std::string &oldPassword) const;
    AccountData getAccountInfo(const std::string& email) const;

    static std::string encryptPassword(const std::string &password);
    bool checkPassword(const std::string &email, const std::string &enteredPassword) const;
};