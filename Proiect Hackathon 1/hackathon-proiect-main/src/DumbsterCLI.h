// DumbsterCLI.h
#pragma once

#include "Dumbster.h" // Clasa nativă C++
#include <msclr/marshal_cppstd.h>

using namespace System;

// ==========================================================
// Clasa Wrapper C++/CLI pentru Dumbster
// ==========================================================
public ref class DumbsterCLI
{
private:
    // Pointer către instanța nativă C++
    Dumbster* nativeDumbster;
    msclr::interop::marshal_context context;

public:
    // Constructorul ia parametrii C# și îi transmite constructorului C++
    DumbsterCLI(String^ dbName, int id);

    // Destructor și Finalizer pentru curățarea memoriei
    ~DumbsterCLI() { this->!DumbsterCLI(); }
    !DumbsterCLI();

    // Proprietate C# pentru a accesa fullness-ul C++
    // Notă: Aceasta ar necesita o metodă getter publică în clasa nativă Dumbster.h (ex: float getFullness() const;)
    property float Fullness {
        float get();
    }

    // Metoda de start care apelează funcția nativă
    void StartMonitoring();

    // Metoda de stop care oprește thread-ul nativ
    void StopMonitoring();


};