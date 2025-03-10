#include <chrono>
#include "UTime.h"

using namespace std::chrono;

long long Time::millis() {
    long long ms = duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
        ).count();
    return ms;
}
