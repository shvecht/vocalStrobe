#include "strobe.h"

std::pair<int, int> computePauseInterval(double freq, float adjustment) {
    int interval = 1000000 / (freq + adjustment);
    int pause = interval / 5;
    interval -= pause;
    return {pause, interval};
}
