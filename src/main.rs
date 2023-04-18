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
    const WIDTH: u32 = 800;
    const HEIGHT: u32 = 800;
    let aspect: f32 = WIDTH as f32 / HEIGHT as f32;

    let mut image: ImageBuffer<Rgba<u8>, _> = ImageBuffer::new(WIDTH, HEIGHT);

    // camera
    let viewport_height: f32 = 2.0;
    let viewport_width: f32 = aspect * viewport_height;
    let focal_length: f32 = 1.0;

    let origin: Point3<f32> = Point3::new(0.0, 0.0, 0.0);
    let horizontal: Vector3<f32> = Vector3::new(viewport_width, 0.0, 0.0);
    let vertical: Vector3<f32> = Vector3::new(0.0, viewport_height, 0.0);
    let lower_left_corner: Point3<f32> = origin - horizontal/2.0 - vertical/2.0 - Vector3::new(0.0, 0.0, focal_length);

    // render
    for h in 0..HEIGHT {
        for w in 0..WIDTH {
            let u = w as f32 / (WIDTH - 1) as f32;
            let v = h as f32 / (WIDTH - 1) as f32;
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
