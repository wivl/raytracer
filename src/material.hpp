#ifndef _MATERIAL_HPP_
#define _MATERIAL_HPP_

#include "ppmpp.hpp"
#include "ray.hpp"
#include "rtmath.hpp"
#include "src/Core/Matrix.h"
#include "hittable.hpp"


// virtual class
class Material {
public:
    virtual bool scatter(const Ray &r_in, const HitRecord &rec,
            Colorf &attenuation, Ray &scattered) const = 0;
};

// aka diffuse reflection material
class Lambertian: public Material {
private:
    Colorf albedo;

public:
    Lambertian(const Vector3f &a): albedo(a) {}

    virtual bool scatter(const Ray &r_in, const HitRecord &rec,
            Colorf &attenuation, Ray &scattered) const override {
        Vector3f scatter_direction = rec.normal + random_unit_vector();
        // catch degenerate scatter direction
        if (near_zero(scatter_direction)) {
            scatter_direction = rec.normal;
        }
        scattered = Ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }
};


class Metal: public Material {
private:
    Colorf albedo;

public:
    Metal(const Vector3f &a): albedo(a) {}
    
    virtual bool scatter(const Ray &r_in, const HitRecord &rec,
            Colorf &attenuation, Ray &scattered) const override {
        Vector3f reflected = reflect(r_in.direction().normalized(), rec.normal);
        scattered = Ray(rec.p, reflected);
        attenuation = albedo;
        return scattered.direction().dot(rec.normal) > 0;
    }
};

#endif // !_MATERIAL_HPP_
