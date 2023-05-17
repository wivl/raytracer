#ifndef _RECT_H_
#define _RECT_H_

#include "hittable.hpp"
#include "bvh.hpp"
#include "src/Core/Matrix.h"
#include <memory>


class XYRect: public Hittable {
    private:
        std::shared_ptr<Material> mp;
        // k: z value
        float x0, x1, y0, y1, k;
    public:
        XYRect() {}
        XYRect(float _x0, float _x1, float _y0, float _y1, float _k,
                std::shared_ptr<Material> mat)
        : x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat) {}

        virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;
        virtual bool bounding_box(float time0, float time1, AABB &output_box) const override {
            output_box = AABB(Vector3f(x0, y0, k-0.0001), Vector3f(x1, y1, k+0.0001));
            return true;
        }
        
};

class XZRect : public Hittable {
    public:
        XZRect() {}

        XZRect(float _x0, float _x1, float _z0, float _z1, float _k,
            std::shared_ptr<Material> mat)
            : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat) {};

        virtual bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const override;

        virtual bool bounding_box(float time0, float time1, AABB& output_box) const override {
            // The bounding box must have non-zero width in each dimension, so pad the Y
            // dimension a small amount.
            output_box = AABB(Vector3f(x0,k-0.0001,z0), Vector3f(x1, k+0.0001, z1));
            return true;
        }

    private:
        std::shared_ptr<Material> mp;
        float x0, x1, z0, z1, k;
};

class YZRect : public Hittable {
    public:
        YZRect() {}

        YZRect(float _y0, float _y1, float _z0, float _z1, float _k,
            std::shared_ptr<Material> mat)
            : y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat) {};

        virtual bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const override;

        virtual bool bounding_box(float time0, float time1, AABB& output_box) const override {
            // The bounding box must have non-zero width in each dimension, so pad the X
            // dimension a small amount.
            output_box = AABB(Vector3f(k-0.0001, y0, z0), Vector3f(k+0.0001, y1, z1));
            return true;
        }

    private:
        std::shared_ptr<Material> mp;
        float y0, y1, z0, z1, k;
};
#endif // !_RECT_H_
