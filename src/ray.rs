extern crate nalgebra as na;
use na::{Point3, Vector3};

#[derive(Debug, Clone, Copy)]
pub struct Ray {
    pub orig: Point3<f32>,
    pub dir: Vector3<f32>,
}

impl Ray {
    pub fn new(origin: Point3<f32>, direction: Vector3<f32>) -> Self {
        Ray {
            orig: origin,
            dir: direction
        }
    }

    pub fn at(&self, t: f32) -> Point3<f32> {
        self.orig + self.dir * t
    }
}


