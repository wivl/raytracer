#include "rect.hpp"


bool XYRect::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
    // get hit time t
    auto t = (k-r.origin().z()) / r.direction().z();

    if (t < t_min || t > t_max) {
        // invalid hit
        return false;
    }

    // get hit point x, y
    auto x = r.origin().x() + t*r.direction().x();
    auto y = r.origin().y() + t*r.direction().y();

    // hit point is in plane but not in rectangle
    if (x < x0 || x > x1 || y < y0 || y > y1) {
        return false;
    }

    rec.u = (x-x0) / (x1-x0);
    rec.v = (y-y0) / (y1-y0);
    rec.t = t;
    auto outward_normal = Vector3f(0, 0, 1);
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mp;
    rec.p = r.at(t);
    return true;
}

bool XZRect::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
    // get hit time t
    auto t = (k-r.origin().z()) / r.direction().z();

    if (t < t_min || t > t_max) {
        // invalid hit
        return false;
    }

    // get hit point x, y
    auto x = r.origin().x() + t*r.direction().x();
    auto z = r.origin().z() + t*r.direction().z();

    // hit point is in plane but not in rectangle
    if (x < x0 || x > x1 || z < z0 || z > z1) {
        return false;
    }

    rec.u = (x-x0) / (x1-x0);
    rec.v = (z-z0) / (z1-z0);
    rec.t = t;
    auto outward_normal = Vector3f(0, 1, 0);
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mp;
    rec.p = r.at(t);
    return true;
}
bool YZRect::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
    // get hit time t
    auto t = (k-r.origin().z()) / r.direction().z();

    if (t < t_min || t > t_max) {
        // invalid hit
        return false;
    }

    // get hit point x, y
    auto y = r.origin().y() + t*r.direction().y();
    auto z = r.origin().z() + t*r.direction().z();

    // hit point is in plane but not in rectangle
    if (y < y0 || y > y1 || z < y0 || z > y1) {
        return false;
    }

    rec.u = (y-y0) / (y1-y0);
    rec.v = (z-z0) / (z1-z0);
    rec.t = t;
    auto outward_normal = Vector3f(1, 0, 0);
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mp;
    rec.p = r.at(t);
    return true;
}
