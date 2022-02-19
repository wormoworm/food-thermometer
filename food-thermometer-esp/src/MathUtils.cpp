#include "MathUtils.h"

double roundDouble(double value, uint8_t places) {
    int factor = pow(10, places);
    return ((int) ((value * factor) + 0.5)) / (double) factor;
}