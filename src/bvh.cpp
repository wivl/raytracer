#include "bvh.hpp"
#include "rtmath.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <memory>

// sort function
inline bool box_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axis) {
    AABB box_a;
    AABB box_b;

    // if any of 2 boxes has no bounding box attribute
    if (!a->bounding_box(0,0, box_a) || !b->bounding_box(0,0, box_b)) {
        std::cerr << "No bounding box in bvh_node constructor.\n";
        exit(1);
    }
    return box_a.min()[axis] < box_b.min()[axis];
}


bool box_x_compare (const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
    return box_compare(a, b, 0);
}

bool box_y_compare (const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
    return box_compare(a, b, 1);
}

bool box_z_compare (const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
    return box_compare(a, b, 2);
}

// generate a BVH tree using objects from src_objects[start] to src_objects[end]
BVHNode::BVHNode (
        const std::vector<std::shared_ptr<Hittable>> src_objects,
        size_t start, size_t end, float time0, float time1
        ) {
    auto objects = src_objects;
    // divide by random axis
    int axis = random_int(0, 2);
    // a function for sorting
    auto comparator = (axis==0) ? box_x_compare
                    : (axis==1) ? box_y_compare
                                : box_z_compare;

    size_t object_span = end - start;

    // if only one node in the list
    if (object_span == 1) {
        left = right = objects[start];
    // 2 nodes
    } else if (object_span == 2) {
        // determin left and right order
        if (comparator(objects[start], objects[start+1])) {
            left = objects[start];
            right = objects[start+1];
        } else {
            left = objects[start+1];
            right = objects[start];
        }
    // multiple objects
    } else {
        std::sort(objects.begin()+start, objects.begin()+end, comparator);
        auto mid = start + object_span / 2;
        // sub bvh tree
        left = std::make_shared<BVHNode>(objects, start, mid, time0, time1);
        right = std::make_shared<BVHNode>(objects, mid, end, time0, time1);
    }
    AABB box_left, box_right;
    if (!left->bounding_box(time0, time1, box_left) ||
            !right->bounding_box(time0, time1, box_right)) {
        std::cerr << "No bounding box in BVH node" << std::endl;
    }

    box = surrounding_box(box_left, box_right);
}

// if hit, check left and right child
bool BVHNode::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
    if (!box.hit(r, t_min, t_max))
        return false;

    bool hit_left = left->hit(r, t_min, t_max, rec);
    bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);

    return hit_left || hit_right;
}

bool BVHNode::bounding_box(float time0, float time1, AABB &output_box) const {
    output_box = box;
    return true;
}

