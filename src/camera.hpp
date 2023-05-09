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
    Vector3f u, v, w;
    float lens_radius;

public:
    Camera(Vector3f lookfrom, Vector3f lookat, Vector3f vup,
            float fovy, float aspect_ratio, float aperture,
            float focus_dist) {
        float theta = degrees_to_radians(fovy);
        float h = tan(theta/2);
        float viewport_height = 2.0 * h;
        float viewport_width = aspect_ratio * viewport_height;

        w = (lookfrom - lookat).normalized(); // z
        u = (vup.cross(w)).normalized();      // x
        v = w.cross(u);                       // y

        origin = lookfrom;

        horizontal = focus_dist * viewport_width * u;
        vertical = focus_dist * viewport_height * v;
        lower_left_corner = origin - horizontal/2 - vertical/2 - focus_dist * w;

        lens_radius = aperture / 2;
        // the image plane
        //               vertical
        //                  ^
        // -2, 1, -1        |        2, 1, -1
        //        0, 0, -1  .---> horizontal
        // -2, -1, -1                2, -1, -1
        //
        // the coordinate is in world space
    }

    Ray get_ray(float s, float t) const {
        Vector3f rd = lens_radius * random_in_unit_disk();
        Vector3f offset = u * rd.x() + v * rd.y();

        return Ray(origin + offset,
                lower_left_corner + s*horizontal + t*vertical - origin - offset);
    }
};


#endif // !_CAMERA_HPP_
