#include "RandomRange.h"

float rand_range(float min, float max)
{
    float random = (float)rand() / RAND_MAX;
    float range = max - min;
    return (random * range) + min;
}

int rand_int(int min, int max) {
    float random = (float)rand() / RAND_MAX;
    int range = max - min;
    return (int)(random * range) + min;
}