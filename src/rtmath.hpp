#ifndef _RTMATH_HPP_
#define _RTMATH_HPP_

#include <Eigen/Dense>
#include "src/Core/Matrix.h"
#include <cmath>
#include <limits>
#include <memory>
#include <random>

// Constants

const float infinity = std::numeric_limits<float>::infinity();
const float pi = 3.1415926535897932385;

// Utility Functions

inline float degrees_to_radians(float degrees) {
    return degrees * pi / 180.0;
}

// generate float number in [0, 1)
inline float random_float() {
    static std::uniform_real_distribution<float> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double random_float(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random_float();
}

// clamp x in [min, max]
inline float clamp(float x, float min, float max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

inline Eigen::Vector3f v3f_random() {
    return Eigen::Vector3f(random_float(), random_float(), random_float());
}

inline Eigen::Vector3f v3f_random(float min, float max) {
    return Eigen::Vector3f(random_float(min,max), random_float(min,max), random_float(min,max));
}

inline Eigen::Vector3f random_in_unit_sphere() {
    while (true) {
        auto p = v3f_random(-1, 1);
        if (p.dot(p) >= 1) {
            continue;
        }
        return p;
    }
}

inline Eigen::Vector3f random_unit_vector() {
    return random_in_unit_sphere().normalized();
}

inline Eigen::Vector3f random_in_hemisphere(const Eigen::Vector3f &normal) {
    Eigen::Vector3f in_unit_sphere = random_in_unit_sphere();
    if (in_unit_sphere.dot(normal) > 0.0) // In the same hemisphere as the normal
        return in_unit_sphere;
    else
        return -in_unit_sphere;
}

// Common Headers

#include "ray.hpp"

#endif
