#ifndef _BOX_H_
#define _BOX_H_

#include "material.hpp"
#include "rtmath.hpp"
#include "rect.hpp"
#include "hittable.hpp"
#include "src/Core/Matrix.h"
#include <memory>

class Box: public Hittable {
    private:
        Vector3f box_min;
        Vector3f box_max;
        HittableList sides;
    public:
        Box() {}
        Box(const Vector3f &p0, const Vector3f &p1, std::shared_ptr<Material> ptr);

        virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;
        // calculate bounding box
        virtual bool bounding_box(float time0, float time1, AABB &output_box) const override {
            output_box = AABB(box_min, box_max);
            return true;
        }
};

class Translate : public Hittable {
    public:
        // actual hittable object
        std::shared_ptr<Hittable> ptr;
        Vector3f offset;
    public:
        Translate(std::shared_ptr<Hittable> p, const Vector3f& displacement)
            : ptr(p), offset(displacement) {}

        virtual bool hit(
            const Ray &r, float t_min, float t_max, HitRecord &rec) const override;

        virtual bool bounding_box(float time0, float time1, AABB& output_box) const override;

};

class RotateY : public Hittable {
    public:
        RotateY(std::shared_ptr<Hittable> p, float angle);

        virtual bool hit(
            const Ray& r, float t_min, float t_max, HitRecord& rec) const override;

        virtual bool bounding_box(float time0, float time1, AABB& output_box) const override {
            output_box = bbox;
            return hasbox;
        }

    public:
        std::shared_ptr<Hittable> ptr;
        float sin_theta;
        float cos_theta;
        bool hasbox;
        AABB bbox;
};

#endif // !_BOX_H_
