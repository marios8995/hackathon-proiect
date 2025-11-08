#include <iostream>
#include <ostream>

#include "src/Dumbster.h"
using namespace std;

int main() {
    DumbsterDatabaseManager t("dumbTest.db");
    t.newDumbster("A", "B", "C", 0);
    Dumbster test("dumbTest.db",1);
    test.startMonitoring();
    std::this_thread::sleep_for(std::chrono::seconds(15));
    test.stopMonitoring();
    return 0;
}