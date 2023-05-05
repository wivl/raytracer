#include "antialiasing.hpp"

void muliple_samples(Vector3f &intensity, int spp) {
    float scale = 1.0 / spp;
    // gamma correct

    intensity.x() = sqrt(intensity.x() * scale);
    intensity.y() = sqrt(intensity.y() * scale);
    intensity.z() = sqrt(intensity.z() * scale);
}
