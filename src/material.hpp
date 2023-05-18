#ifndef _MATERIAL_HPP_
#define _MATERIAL_HPP_

#include "ppmpp.hpp"
#include "ray.hpp"
#include "rtmath.hpp"
#include "src/Core/Matrix.h"
#include "hittable.hpp"
#include "texture.hpp"
#include <memory>


// virtual class
class Material {
public:
    virtual bool scatter(const Ray &r_in, const HitRecord &rec,
            Colorf &attenuation, Ray &scattered) const = 0;
    virtual Colorf emitted(double u, double v, const Vector3f p) const {
        return Colorf(0,0,0);
    }
};

// aka diffuse reflection material
class Lambertian: public Material {
private:
    std::shared_ptr<Texture> albedo;

public:
    Lambertian(const Colorf &a): albedo(std::make_shared<Solid>(a)) {}
    Lambertian(std::shared_ptr<Texture> a): albedo(a) {}

    virtual bool scatter(const Ray &r_in, const HitRecord &rec,
            Colorf &attenuation, Ray &scattered) const override {
        Vector3f scatter_direction = rec.normal + random_unit_vector();
        // catch degenerate scatter direction
        if (near_zero(scatter_direction)) {
            scatter_direction = rec.normal;
        }
        scattered = Ray(rec.p, scatter_direction, r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }
};


class Metal: public Material {
private:
    Colorf albedo;
    float fuzz;

public:
    Metal(const Vector3f &a, float f): albedo(a), fuzz(f < 1 ? f : 1) {}
    
    virtual bool scatter(const Ray &r_in, const HitRecord &rec,
            Colorf &attenuation, Ray &scattered) const override {
        Vector3f reflected = reflect(r_in.direction().normalized(), rec.normal);
        scattered = Ray(rec.p, reflected+fuzz*random_in_unit_sphere(), r_in.time());
        attenuation = albedo;
        return scattered.direction().dot(rec.normal) > 0;
    }
};

class Dielectric: public Material {
private:
    // index of refraction, e.g. glass / air = 1.5 / 1
    float ir;

    // 计算反射率
    static float reflectance(float cosine, float ref_idx) {
        // use schlick's approximation for reflectance.
        float r0 = (1-ref_idx) / (1+ref_idx);
        r0 = r0 * r0;
        return r0 + (1-r0) * pow((1-cosine), 5);
    }

public:
    Dielectric(float index_of_refraction): ir(index_of_refraction) {}

    virtual bool scatter(const Ray &r_in, const HitRecord &rec,
            Colorf &attenuation, Ray &scattered) const override {

        attenuation = Colorf(1.0, 1.0, 1.0);

        float refraction_ratio = rec.front_face ? (1.0/ir) : ir;

        Vector3f unit_direction = r_in.direction().normalized();

        float cos_theta = fmin((-unit_direction).dot(rec.normal), 1.0);
        float sin_theta = sqrt(1.0-cos_theta*cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        Vector3f direction;

        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_float()) {
            direction = reflect(unit_direction, rec.normal);
        } else {
            direction = refract(unit_direction, rec.normal, refraction_ratio);
        }

        scattered = Ray(rec.p, direction, r_in.time());
        return true;
    }
};

class DiffuseLight: public Material {
    private:
        std::shared_ptr<Texture> emit;
    public:
        DiffuseLight(std::shared_ptr<Texture> a): emit(a) {}
        DiffuseLight(Colorf c): emit(std::make_shared<Solid>(c)) {}

        virtual bool scatter(const Ray &r_in, const HitRecord &rec,
                Colorf &attenuation, Ray &scattered) const override {
            return false;
        }
        virtual Colorf emitted(double u, double v, const Vector3f p) const override {
            return emit->value(u, v, p);
        }

        
};

class Isotropic : public Material {
    public:
        Isotropic(Colorf c) : albedo(std::make_shared<Solid>(c)) {}
        Isotropic(std::shared_ptr<Texture> a) : albedo(a) {}

        virtual bool scatter(
            const Ray &r_in, const HitRecord &rec, Colorf &attenuation, Ray &scattered
        ) const override {
            scattered = Ray(rec.p, random_in_unit_sphere(), r_in.time());
            attenuation = albedo->value(rec.u, rec.v, rec.p);
            return true;
        }

    public:
        std::shared_ptr<Texture> albedo;
};

#endif // !_MATERIAL_HPP_
