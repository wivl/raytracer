#ifndef _SPHERE_HPP_
#define _SPHERE_HPP_

#include "hittable.hpp"
#include "material.hpp"
#include "src/Core/Matrix.h"
#include <memory>

class Sphere : public Hittable {
private:
  Vector3f center;
  float radius;
  std::shared_ptr<Material> mat_ptr;
  
public:
  Sphere() {}
  Sphere(Vector3f cen, float r, std::shared_ptr<Material> m):
      center(cen), radius(r), mat_ptr(m) {};

  virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;
};

#endif
