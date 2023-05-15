#ifndef _BVH_H_
#define _BVH_H_

#include <Eigen/Dense>
#include <cstddef>
#include <memory>
#include <vector>
#include "hittable.hpp"
#include "ray.hpp"
#include "src/Core/Matrix.h"

using namespace Eigen;

// axis-aligned bounding box
class AABB {
    private:
        Vector3f minimum;
        Vector3f maximum;
    public:
        AABB() {}
        AABB(const Vector3f &a, const Vector3f &b): minimum(a), maximum(b) {}

        // getters n setters
        Vector3f min() const { return minimum; };
        Vector3f max() const { return maximum; };

        // judge if a ray hits this aabb in period (t_min, t_max)
        bool hit(const Ray &r, float t_min, float t_max) const {
            // optimized
            for (int a = 0; a < 3; a++) {
                auto invD = 1.0f / r.direction()[a];
                auto t0 = (min()[a] - r.origin()[a]) * invD;
                auto t1 = (max()[a] - r.origin()[a]) * invD;
                if (invD < 0.0f)
                    std::swap(t0, t1);
                t_min = t0 > t_min ? t0 : t_min;
                t_max = t1 < t_max ? t1 : t_max;
                if (t_max <= t_min)
                    return false;
            }
            return true;
        }
};

// get bounding box of 2 bounding boxes
inline AABB surrounding_box(AABB box0, AABB box1) {
    Vector3f small(fmin(box0.min().x(), box1.min().x()),
                 fmin(box0.min().y(), box1.min().y()),
                 fmin(box0.min().z(), box1.min().z()));

    Vector3f big(fmax(box0.max().x(), box1.max().x()),
               fmax(box0.max().y(), box1.max().y()),
               fmax(box0.max().z(), box1.max().z()));

    return AABB(small,big);
}

// a bvh bi-tree node
class BVHNode: public Hittable {
    private:
        std::shared_ptr<Hittable> left;
        std::shared_ptr<Hittable> right;
        // self
        AABB box;
    public:
        BVHNode () {}
        // TODO: constructor
        BVHNode (
                const std::vector<std::shared_ptr<Hittable>> src_objects,
                size_t start, size_t end, float time0, float time1
                );

        BVHNode (const HittableList &list, float time0, float time1)
            : BVHNode(list.objects, 0, list.objects.size(), time0, time1) {}

        virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;
        virtual bool bounding_box(float time0, float time1, AABB &output_box) const override;
};

#endif // !_BVH_H_
