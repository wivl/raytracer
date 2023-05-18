#ifndef _HITTABLE_HPP_
#define _HITTABLE_HPP_


#include "ray.hpp"
#include "src/Core/Matrix.h"
#include <memory>
#include <vector>

class Material;
class AABB;

struct HitRecord {
    Vector3f p;
    Vector3f normal;
    std::shared_ptr<Material> mat_ptr;
    float t;
    float u;
    float v;
    bool front_face;

    inline void set_face_normal(const Ray &r, const Vector3f &outward_normal) {
        front_face = r.direction().dot(outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};


class Hittable {
public:
    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const = 0;
    // calculate bounding box
    virtual bool bounding_box(float time0, float time1, AABB &output_box) const = 0;
};


class HittableList: public Hittable {
public:
    std::vector<std::shared_ptr<Hittable>> objects;

public:
    HittableList() {}
    HittableList(std::shared_ptr<Hittable> object) {
        add(object);
    };

    void clear() { 
        objects.clear();
    }
    void add(std::shared_ptr<Hittable> object) {
        objects.push_back(object);
    };

    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;
    virtual bool bounding_box(float time0, float time1, AABB &output_box) const override;
};



#endif // !_HITTABLE_HPP_
