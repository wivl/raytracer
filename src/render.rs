use image::{Rgba};
use nalgebra::{Point3, Vector3};

use crate::ray::Ray;

pub fn color_intensity(color: &Rgba<u8>, intensity: f32) -> Rgba<u8> {
    let mut res: Rgba<u8> = Rgba([255, 255, 255, 255]);
    res.0[0] = (color.0[0] as f32 * intensity) as u8;
    res.0[1] = (color.0[1] as f32 * intensity) as u8;
    res.0[2] = (color.0[2] as f32 * intensity) as u8;
    res.0[3] = color.0[3];
    
    res
}

pub fn add_color(c1: &Rgba<u8>, c2: &Rgba<u8>) -> Rgba<u8> {
    let mut res: Rgba<u8> = Rgba([255, 255, 255, 255]);
    res.0[0] = (c1.0[0] + c2.0[0]) as u8;
    res.0[1] = (c1.0[1] + c2.0[1]) as u8;
    res.0[2] = (c1.0[2] + c2.0[2]) as u8;
    // res.0[3] = (c1.0[3] + c2.0[0]) as u8;
    
    res
}
pub fn ray_color(r: &Ray) -> Rgba<u8> {
    let t = hit_sphere(&Point3::new(0.0, 0.0, -4.0), 1.0, r);
    if t > 0.0 {
        let n: Vector3<f32> = (r.at(t) - Vector3::new(0.0, 0.0, -1.0).normalize()).coords.into();
        return color_intensity(
            &Rgba([255 - ((n.x + 1.0) / 2.0 * 255.0) as u8,
      255 - ((n.y + 1.0) / 2.0 * 255.0) as u8,
      255 - ((n.z + 1.0) / 2.0 * 255.0) as u8,
      255]), 1.0);
    }
    let unit_direction = r.dir.normalize();
    let t = 0.5 * (unit_direction.y + 1.0);
    let white = Rgba([255, 255, 255, 255]);
    let blue = Rgba([127, 178, 255, 255]);
    // let color = (1.0 - t) * white + t * blue;
    let color = add_color(&color_intensity(&white, 1.0-t), &color_intensity(&blue, t));

    color 
}

pub fn hit_sphere(center: &Point3<f32>, radius: f32, r: &Ray) -> f32 {
    // get delta to test if equition has root
    let oc: Vector3<f32> = r.orig - center;
    let a = r.dir.dot(&r.dir);
    let b: f32 = 2.0 * oc.dot(&r.dir);
    let c: f32 = oc.dot(&oc) - radius * radius;
    let discriminant = b * b - 4.0 * a * c;

    if discriminant < 0.0 {
        return -1.0;
    } else {
        return (-b - discriminant.sqrt()) / (2.0 * a);
    }
}
