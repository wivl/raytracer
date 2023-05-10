#include "sphere.hpp"
#include "src/Core/Matrix.h"


bool Sphere::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
    // A vector, from center to origin
    Vector3f oc = r.origin() - center;

    // get the root
    float a = r.direction().dot(r.direction());
    float half_b = oc.dot(r.direction());
    float c = oc.dot(oc) - radius * radius;
    float discriminant = half_b * half_b - a * c;

    // if delta(discriminant) < 0, means no root, no hit
    if (discriminant < 0) {
        return false;
    }

    // else, has root(s)
    auto sqrtd = sqrt(discriminant);

    float root = (-half_b - sqrtd) / a;

    // if both 2 roots (time) are not in [t_min, t_max], np hit
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root) {
            return false;
        }
    }

    // one root fits the situation, set record
    rec.t = root;
    rec.p = r.at(rec.t); // point at the sphere surface
    Vector3f outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;

    // valid hit
    return true;
}

bool SphereMoving::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
    // A vector, from center to origin
    Vector3f oc = r.origin() - center(r.time());

    // get the root
    float a = r.direction().dot(r.direction());
    float half_b = oc.dot(r.direction());
    float c = oc.dot(oc) - radius * radius;
    float discriminant = half_b * half_b - a * c;

    // if delta(discriminant) < 0, means no root, no hit
    if (discriminant < 0) {
        return false;
    }

    // else, has root(s)
    auto sqrtd = sqrt(discriminant);

    float root = (-half_b - sqrtd) / a;

    // if both 2 roots (time) are not in [t_min, t_max], np hit
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root) {
            return false;
        }
    }

    // one root fits the situation, set record
    rec.t = root;
    rec.p = r.at(rec.t); // point at the sphere surface
    Vector3f outward_normal = (rec.p - center(r.time())) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;

    // valid hit
    return true;
}


Vector3f SphereMoving::center(float time) const {
    return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
}

