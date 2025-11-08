cmake_minimum_required(VERSION 3.25)
project(untitled)

set(CMAKE_CXX_STANDARD 20)

# SQLite library
add_library(sqlite3 STATIC database/sqlite3.c
        src/DatabaseManager.cpp
        src/DatabaseManager.h
        src/AccountDatabaseManager.cpp
        src/AccountDatabaseManager.h
        src/DumbsterDatabaseManager.cpp
        src/DumbsterDatabaseManager.h
        src/Dumbster.cpp
        src/Dumbster.h)
target_include_directories(sqlite3 PUBLIC ${CMAKE_SOURCE_DIR}/database)
include_directories(${CMAKE_SOURCE_DIR}/sha256)

# Main executable
add_executable(untitled main.cpp
        src/DatabaseManager.cpp
        src/DatabaseManager.h
        sha256/SHA256.cpp
        src/DumbsterDatabaseManager.cpp
        src/DumbsterDatabaseManager.h
        src/Dumbster.cpp
        src/Dumbster.h)

# --- FIX: Add include path for the main target too ---
target_include_directories(untitled PRIVATE ${CMAKE_SOURCE_DIR}/database)

# Link SQLite
target_link_libraries(untitled PRIVATE sqlite3)