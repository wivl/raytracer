#ifndef _SPHERE_HPP_
#define _SPHERE_HPP_

#include "hittable.hpp"
#include "material.hpp"
#include "src/Core/Matrix.h"
#include <memory>

class Sphere : public Hittable {
    private:
        Vector3f center;
        float radius;
        std::shared_ptr<Material> mat_ptr;

    public:
        Sphere() {}
        Sphere(Vector3f cen, float r, std::shared_ptr<Material> m):
            center(cen), radius(r), mat_ptr(m) {};

        virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;
        virtual bool bounding_box(float time0, float time1, AABB &output_box) const override;
};


class SphereMoving: public Hittable {
    private:
        Vector3f center0, center1;
        float time0, time1;
        float radius;
        std::shared_ptr<Material> mat_ptr;

    public:
        SphereMoving() {}
        SphereMoving(Vector3f cen0, Vector3f cen1,
                float _time0, float _time1,
                float r,
                std::shared_ptr<Material> m
                ):
            center0(cen0),
            center1(cen1),
            time0(_time0),
            time1(_time1),
            radius(r),
            mat_ptr(m) {}

        virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;
        virtual bool bounding_box(float time0, float time1, AABB &output_box) const override;

        // moving along straight line
        Vector3f center(float time) const;

};

#endif
