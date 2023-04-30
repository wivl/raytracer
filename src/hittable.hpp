#ifndef _HITTABLE_H_
#define _HITTABLE_H_

#include "ray.hpp"
#include "src/Core/Matrix.h"

typedef struct {
    Vector3f p;
    Vector3f normal;
    float t;
} HitRecord;


class Hittable {
public:
    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const = 0;
};

#endif // !_HITTABLE_H_



