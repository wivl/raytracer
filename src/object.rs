use crate::hittable::{Hittable, HitRecord};
use crate::ray::Ray;
use nalgebra::{Point3, Vector3};

struct Sphere {
    center: Point3<f32>,
    radius: f32,
}

impl Sphere {
    pub fn new(center: Point3<f32>, radius: f32) -> Self {
        Sphere {
            center,
            radius,
        }
    }
}

impl Hittable for Sphere {
    fn hit(&self, r: &Ray, t_min: f32, t_max: f32, rec: &mut HitRecord) -> bool {
         let oc: Vector3<f32> = r.orig - self.center;
         let a = r.dir.magnitude_squared();
         let half_b = oc.dot(&r.dir);
         let c = oc.magnitude_squared() - self.radius * self.radius;

         let discriminant = half_b * half_b - a * c;

         if discriminant < 0.0 {
            return false;
         }

         let sqrtd = discriminant.sqrt();
         let mut root = (-half_b - sqrtd) / a;
         if root < t_min || t_max < root {
             root = (-half_b + sqrtd) / a;
             if root < t_min || t_max < root {
                 return false;
             }
         }
         rec.t = root;
         rec.p = r.at(rec.t);
         let outward_normal = (rec.p - self.center) / self.radius;
         rec.set_face_normal(r, &outward_normal);

         return true;
    }
}
