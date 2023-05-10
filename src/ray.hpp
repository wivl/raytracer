#ifndef _RAY_HPP_
#define _RAY_HPP_

#include <Eigen/Dense>
#include "Eigen/src/Core/Matrix.h"
using namespace Eigen;

class Ray {
private:
    Vector3f orig;
    Vector3f dir;
    float tm;

public:
    Ray() {};
    Ray(const Vector3f &origin, const Vector3f &direction, float time = 0.0): 
        orig(origin),
        dir(direction),
        tm(time)
    {}

    Vector3f origin() const { return orig; }
    Vector3f direction() const { return dir; }
    float time() const { return tm; }

    Vector3f at(float t) const {
        return orig + t * dir;
    }
};

#endif // !_RAY_HPP_
