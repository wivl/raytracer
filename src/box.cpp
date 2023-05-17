#include "box.hpp"
#include "rect.hpp"
#include <memory>


Box::Box(const Vector3f &p0, const Vector3f &p1, std::shared_ptr<Material> ptr) {
    box_min = p0;
    box_max = p1;

    sides.add(std::make_shared<XYRect>(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr));
    sides.add(std::make_shared<XYRect>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr));

    sides.add(std::make_shared<XZRect>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr));
    sides.add(std::make_shared<XZRect>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr));

    sides.add(std::make_shared<YZRect>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr));
    sides.add(std::make_shared<YZRect>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr));
}

bool Box::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
    return sides.hit(r, t_min, t_max, rec);
}
