#include "antialiasing.hpp"

void muliple_samples(Vector3f &intensity, int spp) {
    float scale = 1.0 / spp;
    intensity.x() *= scale;
    intensity.y() *= scale;
    intensity.z() *= scale;
}
