// DumbsterManagerCLI.h (Proiect C++/CLI)
#pragma once

#include "DumbsterDatabaseManager.h" // Include header-ul C++ nativ
#include <string>
#include <vector>

// 1. Definește Structura .NET/CLI echivalentă cu DumbsterData
public ref struct DumbsterDataCLI
{
    // Tipuri .NET (System::String, int, bool)
    property int Id;
    property System::String^ City;
    property System::String^ County;
    property System::String^ Street;
    property int StreetNumber;
    property bool IsFull;
    property int UseNumber;

    // Metodă pentru a converti DumbsterData nativă în DumbsterDataCLI .NET
    static DumbsterDataCLI^ ToCLI(const DumbsterData& nativeData);
};

// 2. Clasa Wrapper C++/CLI
public ref class DumbsterDatabaseManagerCLI
{
private:
    // Ptr la clasa C++ nativă (Handle-ul Bridge-ului)
    DumbsterDatabaseManager* nativeManager;

public:
    // Constructor
    DumbsterDatabaseManagerCLI(System::String^ dbName);
    // Destructor (pentru a curăța memoria)
    ~DumbsterDatabaseManagerCLI() { this->!DumbsterDatabaseManagerCLI(); }
    !DumbsterDatabaseManagerCLI(); // Finalizer

    // --- Metode care folosesc tipuri .NET ---

    bool OpenDB();

    bool NewDumbster(System::String^ city, System::String^ county, System::String^ street, int streetNumber);

    // Returnează o listă de tipuri .NET/CLI
    System::Collections::Generic::List<DumbsterDataCLI^>^ GetDumbstersCity(System::String^ city);
};