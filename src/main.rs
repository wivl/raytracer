use image::{Rgba, ImageBuffer, imageops};
use nalgebra::{Point3, Vector3};

mod ray;
use crate::ray::Ray;
mod render;
use render::ray_color;
mod hittable;
mod object;


fn main() {
    // image
    let aspect: f32 = 16.0 / 9.0;
    let width: u32 = 1080;
    let height: u32 = (width as f32 / aspect + 0.5) as u32;

    let mut image: ImageBuffer<Rgba<u8>, _> = ImageBuffer::new(width, height);

    // camera
    let viewport_height: f32 = 2.0;
    let viewport_width: f32 = aspect * viewport_height;
    let focal_length: f32 = 1.0;

    let origin: Point3<f32> = Point3::new(0.0, 0.0, 0.0);
    let horizontal: Vector3<f32> = Vector3::new(viewport_width, 0.0, 0.0);
    let vertical: Vector3<f32> = Vector3::new(0.0, viewport_height, 0.0);
    let lower_left_corner: Point3<f32> = origin - horizontal/2.0 - vertical/2.0 - Vector3::new(0.0, 0.0, focal_length);

    // render
    for h in 0..height {
        for w in 0..width {
            let u = w as f32 / (width - 1) as f32;
            let v = h as f32 / (height - 1) as f32;
            // 将像素坐标系的原点从左上角转换为左下角，使得渲染结果与标准的图像坐标系一致。
            // let v = (height - h - 1) as f32 / (height - 1) as f32;
            let r = Ray::new(origin, lower_left_corner + u*horizontal + v*vertical - origin);
            let pixel = ray_color(&r);
            println!("{:?}", pixel);
            image.put_pixel(w, h, pixel);
        }
    }

    let flipped = imageops::flip_vertical(&image);

    image.save("image.png").unwrap();
    flipped.save("flipped.png").unwrap();
}
