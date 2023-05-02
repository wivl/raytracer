#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "hittable.hpp"
#include "src/Core/Matrix.h"

class Sphere : public Hittable {
private:
  Vector3f center;
  float radius;
  
public:
  Sphere() {}
  Sphere(Vector3f cen, float r): center(cen), radius(r) {};

  virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;
};

#endif
