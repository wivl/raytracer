#ifndef _CAMERA_HPP_
#define _CAMERA_HPP_

#include "rtmath.hpp"
#include "src/Core/Matrix.h"

class Camera {
private:
    Vector3f origin;
    Vector3f lower_left_corner;
    Vector3f horizontal;
    Vector3f vertical;

public:
    Camera() {
        float aspect_ratio = 16.0 / 9.0;
        float viewport_height = 2.0;
        float viewport_width = aspect_ratio * viewport_height;
        float focal_length = 1.0;

        origin = Vector3f(0, 0, 0);
        horizontal = Vector3f(viewport_width, 0, 0);
        vertical = Vector3f(0, viewport_height, 0);
        lower_left_corner = origin - horizontal/2 - vertical/2 - Vector3f(0, 0, focal_length);
        // the image plane
        //               vertical
        //                  ^
        // -2, 1, -1        |        2, 1, -1
        //        0, 0, -1  .---> horizontal
        // -2, -1, -1                2, -1, -1
        //
        // the coordinate is in world space
    }

    Ray get_ray(float u, float v) const {
        return Ray(origin, lower_left_corner + u*horizontal + v*vertical - origin);
    }
};


#endif // !_CAMERA_HPP_
