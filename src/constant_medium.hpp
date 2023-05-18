#ifndef _CONSTANT_MEDIUM_HPP_
#define _CONSTANT_MEDIUM_HPP_

#include "hittable.hpp"
#include "texture.hpp"
#include "material.hpp"


class ConstantMedium: public Hittable {
    public:
        ConstantMedium(std::shared_ptr<Hittable> b, float d, std::shared_ptr<Texture> a)
            : boundary(b),
              neg_inv_density(-1/d),
              phase_function(std::make_shared<Isotropic>(a))
            {}

        ConstantMedium(std::shared_ptr<Hittable> b, float d, Colorf c)
            : boundary(b),
              neg_inv_density(-1/d),
              phase_function(std::make_shared<Isotropic>(c))
            {}

        virtual bool hit(
            const Ray& r, float t_min, float t_max, HitRecord& rec) const override;

        virtual bool bounding_box(float time0, float time1, AABB& output_box) const override {
            return boundary->bounding_box(time0, time1, output_box);
        }

    public:
        std::shared_ptr<Hittable> boundary;
        std::shared_ptr<Material> phase_function;
        float neg_inv_density;
};

#endif // !_CONSTANT_MEDIUM_HPP_
