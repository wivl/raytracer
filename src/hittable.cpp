#include "hittable.hpp"
#include "bvh.hpp"

bool HittableList::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
    HitRecord temp_rec;
    bool hit_anything = false;
    float closest_so_far = t_max;

    for (const auto &object: objects) {
        if (object->hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    return hit_anything;
}


// get the bb of the list
bool HittableList::bounding_box(float time0, float time1, AABB &output_box) const {
    // if empty no bb
    if (objects.empty()) {
        return false;
    }

    AABB temp_box;
    bool first_box = true;

    // TODO: analyse
    for (const auto &object : objects) {
        if (!object->bounding_box(time0, time1, temp_box)) {
            return false;
        }
        output_box = first_box ? temp_box : surrounding_box(output_box, temp_box);
        first_box = false;
    }
    return true;
}

