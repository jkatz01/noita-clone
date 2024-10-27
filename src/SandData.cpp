#include "SandData.h"

Color GenerateParticleColor(ParticleType type) {
    if (type < 0 || type >= PARTICLE_TYPE_COUNT) {
        return RED;
    }
    Color ret = ColorBrightness(color_ref[type].colour, rand_range(color_ref[type].bright_min, color_ref[type].bright_max));
    return ColorTint(color_ref[type].tint, ret);
}