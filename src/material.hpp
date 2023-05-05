#ifndef _MATERIAL_HPP_
#define _MATERIAL_HPP_

#include "ppmpp.hpp"
#include "ray.hpp"
#include "rtmath.hpp"
#include "src/Core/Matrix.h"
#include "hittable.hpp"


class Material {
public:
    virtual bool scatter(const Ray &r_in, const HitRecord &rec,
            Vector3f &attenuation, Ray &scattered) const = 0;
};

class Lambertian: public Material {
private:
    Vector3f albedo;

public:
    Lambertian(const Vector3f &a): albedo(a) {}

    virtual bool scatter(const Ray &r_in, const HitRecord &rec,
            Vector3f &attenuation, Ray &scattered) const override {
        Vector3f scatter_direction = rec.normal + random_unit_vector();
        scattered = Ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }
};

#endif // !_MATERIAL_HPP_
